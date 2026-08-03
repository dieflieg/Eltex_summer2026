#include <stdio.h>      /* printf(), fprintf(), stderr */
#include <stdlib.h>     /* malloc(), free(), EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>     /* strcmp(), strlen(), memcpy(), memset(), strerror() */
#include <errno.h>      /* errno, EINTR */
#include <unistd.h>     /* fork(), pipe(), read(), write(), close(), unlink(), _exit() */
#include <fcntl.h>      /* open(), O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_RDWR */
#include <signal.h>     /* signal(), SIGPIPE, SIG_IGN */
#include <sys/types.h>  /* pid_t и другие системные типы */
#include <sys/wait.h>   /* waitpid(), WIFEXITED, WEXITSTATUS */
#include <sys/stat.h>   /* mkfifo(), fstat(), struct stat, S_ISREG */

#define BUFFER_SIZE 4096

struct FileHeader {
    char orig_name[256]; // Имя исходного файла без пути
    off_t file_size; // Размер файла в байтах
};

void run_child(int pipe_read_fd);

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Использование: %s <файл1> [файл2 ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Игнорируем SIGPIPE
    signal(SIGPIPE, SIG_IGN); 

    // Проверка существования всех ргументов - файлов и их типа
    for (int i = 1; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) == -1) {
            fprintf(stderr, "Ошибка получения информации о файле '%s': %s\n", argv[i], strerror(errno));
            return EXIT_FAILURE;
        }
        if (!S_ISREG(st.st_mode)) { 
            fprintf(stderr, "'%s' не является обычным файлом\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // Создаем канал (pipe)
    int pipe_fds[2]; // pipe_fds[0] - чтение, pipe_fds[1] - запись
    if (pipe(pipe_fds) == -1) {
        perror("Ошибка создания pipe");
        return EXIT_FAILURE;
    }

    // Порождаем дочерний процесс
    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка вызова fork");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        close(pipe_fds[1]); // Закрываем ненужный конец на запись
        run_child(pipe_fds[0]);
        close(pipe_fds[0]);
        _exit(EXIT_SUCCESS); // Используем _exit в дочернем процессе
    }

    close(pipe_fds[0]); // Закрываем ненужный конец на чтение

    char buffer[BUFFER_SIZE];

    // По очереди обрабатываем каждый файл
    for (int i = 1; i < argc; i++) {
        struct stat st;
        stat(argv[i], &st);

        int src_fd = open(argv[i], O_RDONLY);
        if (src_fd == -1) {
            fprintf(stderr, "[Родитель] Не удалось открыть '%s': %s\n", argv[i], strerror(errno));
            continue; // Пропускаем, если что-то пошло не так в процессе
        }

        // Подготавливаем заголовок для ребенка
        struct FileHeader header;
        memset(&header, 0, sizeof(header));
        strncpy(header.orig_name, argv[i], sizeof(header.orig_name) - 1);
        header.file_size = st.st_size;

        // Отправляем заголовок в pipe
        if (write(pipe_fds[1], &header, sizeof(header)) != sizeof(header)) {
            fprintf(stderr, "[Родитель] Ошибка отправки заголовка для '%s'\n", argv[i]);
            close(src_fd);
            break; 
        }

        // Читаем файл блоками и перенаправляем в pipe
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            ssize_t bytes_written = write(pipe_fds[1], buffer, bytes_read);
            if (bytes_written == -1) {
                if (errno == EINTR) continue; // Если прервал сигнал, пробуем еще раз
                fprintf(stderr, "[Родитель] Ошибка записи в pipe: %s\n", strerror(errno));
                break;
            }
        }
        close(src_fd);
        printf("[Родитель] Файл '%s' успешно передан дочернему процессу.\n", argv[i]);
    }

    // Когда все файлы прочитаны, закрываем pipe. 
    // (для ребенка это станет сигналом EOF)
    close(pipe_fds[1]);

    // Ждем окончания работы ребенка
    int status;
    waitpid(pid, &status, 0);
    
    printf("[Родитель] Дочерний процесс завершился. Работа окончена.\n");
    return EXIT_SUCCESS;
}

// Функция, которую выполняет дочерний процесс
void run_child(int pipe_read_fd) {
    struct FileHeader header;
    char buffer[BUFFER_SIZE];

    // Цикл работает, пока из pipe можно прочитать полноценный заголовок файла
    while (read(pipe_read_fd, &header, sizeof(header)) == sizeof(header)) {
        
        // Формируем имя нового файла (добавляем .copy)
        char copy_name[512];
        snprintf(copy_name, sizeof(copy_name), "%s.copy", header.orig_name);

        // Открываем файл на запись
        int dest_fd = open(copy_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd == -1) {
            fprintf(stderr, "[Ребенок] Ошибка создания файла '%s': %s\n", copy_name, strerror(errno));
            // Если не можем создать файл, вычитываем данные из пайпа в пустоту, чтобы не ломать поток
            off_t remaining = header.file_size;
            while (remaining > 0) {
                ssize_t to_read = (remaining > (off_t)sizeof(buffer)) ? (off_t)sizeof(buffer) : remaining;
                ssize_t n = read(pipe_read_fd, buffer, to_read);
                if (n <= 0) return;
                remaining -= n;
            }
            continue;
        }

        // Читаем ровно столько байт, сколько указано в заголовке размера файла
        off_t remaining = header.file_size;
        while (remaining > 0) {
            // Вычисляем, сколько байт нам нужно прочитать в этот заход
            off_t to_read = (remaining > (off_t)sizeof(buffer)) ? (off_t)sizeof(buffer) : remaining;
            
            ssize_t bytes_read = read(pipe_read_fd, buffer, to_read);
            if (bytes_read == -1) {
                if (errno == EINTR) continue;
                perror("[Ребенок] Ошибка чтения из pipe");
                close(dest_fd);
                return;
            }
            if (bytes_read == 0) {
                fprintf(stderr, "[Ребенок] Неожиданный конец pipe!\n");
                close(dest_fd);
                return;
            }

            // Записываем данные в файл .copy
            write(dest_fd, buffer, bytes_read);
            remaining -= bytes_read;
        }

        close(dest_fd);
        printf("[Ребенок] Создан файл-копия: '%s'\n", copy_name);
    }
}
