#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "operation.h"

#include <dirent.h>
#include <dlfcn.h>

// Структура для хранения загруженной операции и дескриптора библиотеки
typedef struct {
    Operation op;
    void *lib_handle;
} LoadedOperation;

static LoadedOperation *loaded_ops = NULL;
static int num_ops = 0;

// Загрузка одного конкретного плагина
void load_single_plugin(const char *full_path, const char *filename) {
    void *handle = dlopen(full_path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки %s: %s\n", filename, dlerror());
        return;
    }

    // Ищем функцию get_operation внутри библиотеки
    typedef const Operation* (*get_op_fn)(void);
    get_op_fn get_op = (get_op_fn)dlsym(handle, "get_operation");
    
    if (!get_op) {
        fprintf(stderr, "В библиотеке %s не найдена функция get_operation\n", filename);
        dlclose(handle);
        return;
    }

    const Operation *op = get_op();
    
    LoadedOperation *temp = realloc(loaded_ops, (num_ops + 1) * sizeof(LoadedOperation));
    if (!temp) {
        fprintf(stderr, "Ошибка выделения памяти для %s\n", filename);
        dlclose(handle);
        return;
    }
    
    loaded_ops = temp;
    loaded_ops[num_ops].op = *op;
    loaded_ops[num_ops].lib_handle = handle;
    num_ops++;
    
    printf("Успешно загружен плагин: %s\n", op->name);
}

// Сканирование папки с плагинами
void load_plugins(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        printf("Предупреждение: Папка '%s' не найдена.\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".so")) {
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
            load_single_plugin(full_path, entry->d_name);
        }
    }
    closedir(dir);
}

// Очистка памяти и выгрузка библиотек
void cleanup_plugins(void) {
    for (int i = 0; i < num_ops; i++) {
        if (loaded_ops[i].lib_handle) {
            dlclose(loaded_ops[i].lib_handle);
        }
    }
    free(loaded_ops);
    loaded_ops = NULL;
    num_ops = 0;
}

void printMenu(void) {
    system("clear"); // Для Windows было бы "cls"
    printf("\n=== КАЛЬКУЛЯТОР ===\n");
    printf("Загружено операций: %d\n\n", num_ops);
    
    for (int i = 0; i < num_ops; i++) {
        printf("%d. %s\n", i + 1, loaded_ops[i].op.name);
    }
    printf("%d. Выход\n", num_ops + 1);
}

void waitForEnter(void) {
    printf("\nНажмите Enter для продолжения...");
    int c;
    // Чистим буфер ввода
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

// Обрабатывает одно действие пользователя. 
// Возвращает 1, если нужно продолжить работу, 0 - если пользователь выбрал выход.
int process_user_action(void) {
    char buffer[256];
    int action;
    double a, b, result;
    char err_msg[256];
    
    printMenu();
    printf("\nВведите номер операции и два числа через пробел (напр. 1 5.5 2)\n");
    printf("Или введите %d для выхода: ", num_ops + 1);

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        return 0; // EOF или ошибка чтения
    }

    if (sscanf(buffer, "%d", &action) != 1) {
        printf("\nОшибка: не удалось прочитать номер операции!\n");
        waitForEnter();
        return 1;
    }

    if (action == num_ops + 1) {
        return 0; // Пользователь хочет выйти
    }

    if (action < 1 || action > num_ops) {
        printf("\nОшибка: неизвестная операция.\n");
        waitForEnter();
        return 1;
    }

    if (sscanf(buffer, "%*d %lf %lf", &a, &b) != 2) {
        printf("\nОшибка: необходимо ввести ровно два числа!\n");
        waitForEnter();
        return 1;
    }

    // Выполнение операции
    int idx = action - 1;
    
    if (loaded_ops[idx].op.validate != NULL) {
        if (!loaded_ops[idx].op.validate(a, b, err_msg, sizeof(err_msg))) {
            printf("\nОшибка: %s\n", err_msg);
            waitForEnter();
            return 1;
        }
    }

    result = loaded_ops[idx].op.execute(a, b);

    printf("\nРезультат: ");
    if (fmod(result, 1.0) == 0.0 && fabs(result) < 1e15) {
        printf("%.0f\n", result);
    } else {
        printf("%.4f\n", result);
    }

    waitForEnter();
    return 1;
}

int main(void) {
    load_plugins("plugins");

    if (num_ops == 0) {
        printf("Не удалось загрузить ни одной операции. Проверьте папку plugins/.\n");
        return 1;
    }

    // Главный цикл программы
    while (process_user_action()) {
    }

    cleanup_plugins();
    printf("\nРабота программы завершена. До свидания!\n");
    return 0;
}