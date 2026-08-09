# Написать программу для расчета маски прав доступа к файлу. 
1)Пользователь может ввести права доступа в буквенном или цифровом
обозначении, ему должно быть показано соответствующее битовое
представление.
2)Пользователь может ввести имя файла, и ему отобразится буквенное,
цифровое и битовое представление прав доступа. Использовать функцию stat
для получения информации о файле. Сравнить результат с выводом,
например, ls -l.
3)Пользователь может изменить права доступа, определенные в первом или
втором пункте, введя команды модификации атрибутов (подобно команде
chmod). При этом отображается буквенное, цифровое и битовое
представление прав доступа. Изменение прав доступа не нужно применять к
файлу.

---

## **Структуры и прототипы:**

```c
#define BUF_SIZE 1024 // Размер буфера для безопасного считывания строк

// Структура для хранения информации о правах доступа одновременно в трех форматах
typedef struct {
    mode_t binary;    // Битовое представление (используется для побитовых операций)
    int octal;        // Восьмеричное представление (например, 755)
    char symbolic[10];// Символьное представление (например, rwxr-xr-x + '\0')
} Permissions;

// Прототипы функций бизнес-логики
void parse_symbolic(const char *sym, Permissions *perm);
void parse_octal(int oct, Permissions *perm);
void update_symbolic(Permissions *perm);
void print_permissions(const Permissions *perm);
void process_chmod_commands(Permissions *perm, const char *command);
void get_file_permissions(const char *filename, Permissions *perm);

// Прототип функции запуска юнит-тестов
void run_all_tests(void);

#endif // PERMISSIONS_H
```

---

## **Бизнес-логика (permissions.c):**

```c
// Парсинг прав из символьного формата (например, "rwxr-xr--") в битовую маску
void parse_symbolic(const char *sym, Permissions *perm) {
    mode_t modes = 0; // Переменная для накопления битов прав
    const char *p = sym; // Указатель для последовательного посимвольного прохода по строке
    
    // Считываем первые три символа, отвечающие за права владельца (User)
    // Если символ совпадает с ожидаемым, добавляем соответствующий системный макрос
    // через побитовое ИЛИ (|=), иначе добавляем 0 (состояние бита не меняется)
    modes |= (*p++ == 'r') ? S_IRUSR : 0; // Бит чтения для владельца
    modes |= (*p++ == 'w') ? S_IWUSR : 0; // Бит записи для владельца
    modes |= (*p++ == 'x') ? S_IXUSR : 0; // Бит исполнения для владельца
    
    // Проверяем следующие три символа для прав группы (Group)
    modes |= (*p++ == 'r') ? S_IRGRP : 0;
    modes |= (*p++ == 'w') ? S_IWGRP : 0;
    modes |= (*p++ == 'x') ? S_IXGRP : 0;
    
    // Проверяем последние три символа для прав остальных пользователей (Other)
    modes |= (*p++ == 'r') ? S_IROTH : 0;
    modes |= (*p++ == 'w') ? S_IWOTH : 0;
    modes |= (*p++ == 'x') ? S_IXOTH : 0;
    
    // Сохраняем собранные биты в структуру
    perm->binary = modes;
    perm->octal = modes & 0777; // Маска 0777 оставляет только младшие 9 бит (стандартные права rwx)
    update_symbolic(perm); // Генерируем строковое представление на основе новых битов
}

// Парсинг прав из восьмеричного (цифрового) формата (например, 755)
void parse_octal(int oct, Permissions *perm) {
    perm->octal = oct & 0777; // Применяем маску 0777 для отсечения старших битов
    perm->binary = oct & 0777; // Битовое представление совпадает с восьмеричным числом
    update_symbolic(perm);     // Пересчитываем символьное представление
}

// Обновление строкового (символьного) представления прав на основе битовой маски
void update_symbolic(Permissions *perm) {
    // Форматируем строку из 9 символов, проверяя каждый отдельный бит через побитовое И (&)
    // Если бит установлен, ставим букву (r, w, x), иначе ставим прочерк (-)
    snprintf(perm->symbolic, 10, "%c%c%c%c%c%c%c%c%c",
        (perm->binary & S_IRUSR) ? 'r' : '-', // Чтение владельца
        (perm->binary & S_IWUSR) ? 'w' : '-', // Запись владельца
        (perm->binary & S_IXUSR) ? 'x' : '-', // Исполнение владельца
        (perm->binary & S_IRGRP) ? 'r' : '-', // Чтение группы
        (perm->binary & S_IWGRP) ? 'w' : '-', // Запись группы
        (perm->binary & S_IXGRP) ? 'x' : '-', // Исполнение группы
        (perm->binary & S_IROTH) ? 'r' : '-', // Чтение остальных
        (perm->binary & S_IWOTH) ? 'w' : '-', // Запись остальных
        (perm->binary & S_IXOTH) ? 'x' : '-');// Исполнение остальных
}

// Вывод текущих прав доступа во всех трех форматах для наглядности
void print_permissions(const Permissions *perm) {
    printf("\nТекущие права:\n");
    printf("Символьный формат: %s\n", perm->symbolic); // Например, rwxr-xr-x
    printf("Цифровой формат:  %04o\n", perm->octal);    // %o выводит число в восьмеричном формате
    
    printf("Битовое представление: ");
    // Проходимся по 9 битам (от старшего 8-го к младшему 0-му) и выводим их состояние
    for(int i = 8; i >= 0; i--) {
        // Сдвигаем число на i позиций вправо и применяем побитовое И с 1, 
        // чтобы изолировать и получить значение конкретного бита (0 или 1)
        printf("%d", (perm->binary >> i) & 1);
        
        // Для удобства визуального восприятия добавляем пробел после каждой тройки битов 
        // (разделяем права владельца, группы и остальных), кроме самого конца строки
        if(i % 3 == 0 && i != 0) printf(" ");
    }
    printf("\n\n");
}

// Обработка команд изменения прав в стиле утилиты chmod (например, u+rwx,g-x,o=r)
void process_chmod_commands(Permissions *perm, const char *command) {
    // Создаем динамическую копию строки команды в куче, так как функция strtok
    // разрушает исходную строку, вставляя в нее символы конца строки '\0'
    char *cmd_copy = strdup(command);
    
    // Разбиваем строку на отдельные токены, используя запятую как разделитель.
    // Каждый токен представляет собой независимую команду (например, "u+rwx" или "g-x")
    char *token = strtok(cmd_copy, ",");
    
    // Перебираем все токены (команды) по очереди в цикле
    while(token != NULL) {
        // Пропускаем возможные пробелы в начале текущей подстроки-команды
        char *part = token;
        while(*part == ' ') part++;
        
        // Ищем символ математической операции (+, - или =), чтобы разделить команду 
        // на целевую аудиторию (who) и сами права (rights)
        char *op_ptr = part;
        while(*op_ptr && !strchr("+-=", *op_ptr)) op_ptr++;
        
        // Если символ операции не найден (достигнут конец строки), 
        // переходим к следующему токену
        if(*op_ptr == '\0') {
            token = strtok(NULL, ",");
            continue;
        }
        
        char op = *op_ptr; // Сохраняем сам символ операции (+, - или =)
        *op_ptr = '\0';    // Разрываем строку на месте операции 
                           // (теперь 'part' содержит только флаги u/g/o/a)
        char *rights = op_ptr + 1; // Указатель на строку с устанавливаемыми правами (r, w, x, s, t)
        
        // ШАГ 1: Определяем область действия (создаем маску категорий пользователей)
        mode_t mask = 0;
        char *who = part; // Строка с флагами аудитории (u, g, o, a)
        while(*who) {
            switch(*who) {
                case 'u': mask |= S_IRWXU; break; // Маска для владельца (User)
                case 'g': mask |= S_IRWXG; break; // Маска для группы (Group)
                case 'o': mask |= S_IRWXO; break; // Маска для остальных (Other)
                // Флаг 'a' (all) объединяет маски всех трех категорий через побитовое ИЛИ
                case 'a': mask |= S_IRWXU | S_IRWXG | S_IRWXO; break;
            }
            who++; // Переход к следующему символу в строке аудитории
        }
        
        // ШАГ 2: Определяем конкретные биты прав, которые нужно изменить
        mode_t bits = 0;
        while(*rights) {
            switch(*rights) {
                case 'r': bits |= S_IRUSR | S_IRGRP | S_IROTH; break; // Биты чтения для всех
                case 'w': bits |= S_IWUSR | S_IWGRP | S_IWOTH; break; // Биты записи для всех
                case 'x': bits |= S_IXUSR | S_IXGRP | S_IXOTH; break; // Биты исполнения для всех
                case 's': bits |= S_ISUID | S_ISGID; break;           // SetUID / SetGID
                case 't': bits |= S_ISVTX; break;                     // Sticky bit
            }
            rights++; // Переход к следующему символу права
        }
        
        // ШАГ 3: Применяем выбранную операцию с учетом маски категории
        // Выражение (bits & mask) "отсекает" те права, которые не относятся 
        // к выбранной категории (u, g, o)
        switch(op) {
            case '+': // Добавление прав: устанавливаем биты через побитовое ИЛИ
                perm->binary |= (bits & mask);
                break;
            case '-': // Удаление прав: сбрасываем биты через побитовое И 
                      // с инвертированной маской
                perm->binary &= ~(bits & mask);
                break;
            case '=': // Точное присвоение прав
                // Сначала обнуляем все биты в заданной категории (perm->binary & ~mask),
                // затем принудительно устанавливаем новые значения ((bits & mask))
                perm->binary = (perm->binary & ~mask) | (bits & mask);
                break;
        }
        
        // Переходим к следующей команде (токену) после запятой
        token = strtok(NULL, ",");
    }
    
    // Освобождаем динамическую память, выделенную функцией strdup под копию строки
    free(cmd_copy);
    
    // Сохраняем только валидные биты прав (младшие 12 бит, маска 07777)
    perm->binary &= 07777;  
    perm->octal = perm->binary; // Обновляем восьмеричное представление
    update_symbolic(perm);      // Пересчитываем символьное представление
}

// Получение реальных прав доступа указанного файла из файловой системы
void get_file_permissions(const char *filename, Permissions *perm) {
    struct stat st; // Системная структура для хранения метаданных файла
    
    // Вызываем системный вызов stat, который заполняет структуру st информацией о файле.
    // В случае ошибки (например, файл не существует) stat вернет ненулевое значение
    if (stat(filename, &st)) {
        perror("Ошибка получения информации о файле");
        return;
    }
    
    // Поле st_mode содержит тип файла и его права. 
    // Маска 0777 оставляет только базовые 9 битов прав (rwxrwxrwx)
    perm->binary = st.st_mode & 0777;
    perm->octal = perm->binary;
    update_symbolic(perm);
    
    // Сравнение с выводом системной утилиты ls -l (требование ТЗ, пункт 2)
    printf("\nСравнение с ls -l:\n");
    char command[BUF_SIZE];
    // Формируем строку команды для терминала, подставляя имя файла
    snprintf(command, BUF_SIZE, "ls -l %s", filename);
    // Выполняем команду в оболочке для наглядного сравнения
    system(command);
}
```

---

## **Пользовательский интерфейс (main.c):**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permissions.h"

int main() {
    char input[BUF_SIZE]; // Буфер для считывания пользовательского ввода
    Permissions perm = {0}; // Инициализация структуры прав нулями
    int running = 1; // Флаг для управления главным циклом программы

    // Главный цикл работает до тех пор, пока пользователь не выберет выход
    while(running) {
        // Вывод интерфейса главного меню
        printf("\n=== Меню управления правами ===\n");
        printf("1. Ввод прав в символьном/цифровом формате\n");
        printf("2. Показать права файла\n");
        printf("3. Изменить права (chmod-подобный синтаксис)\n");
        printf("4. Запустить юнит-тесты\n");
        printf("5. Выход\n");
        printf("Выберите действие: ");
        
        // Считываем выбор пользователя в текстовом виде
        fgets(input, BUF_SIZE, stdin);
        int choice = atoi(input); // Преобразуем считанную строку в целое число

        // Обработка выбранного пункта меню через конструкцию switch
        switch(choice) {
            case 1: {
                // Запрос ручного ввода прав от пользователя
                printf("Введите права (например, 755 или rwxr-xr-x): ");
                fgets(input, BUF_SIZE, stdin);
                // Находим символ перевода строки '\n' и заменяем его на '\0'
                input[strcspn(input, "\n")] = '\0';

                // Определяем формат ввода: если первый символ является цифрой, 
                // значит, используется восьмеричный формат
                if (input[0] >= '0' && input[0] <= '9') {
                    // strtol преобразует строку в число по основанию 8
                    parse_octal((int)strtol(input, NULL, 8), &perm);
                } else {
                    // Иначе считаем, что введен символьный формат
                    parse_symbolic(input, &perm);
                }
                print_permissions(&perm); // Выводим результат во всех форматах
                break;
            }
            case 2: {
                // Запрос имени файла для чтения его реальных прав из ОС
                printf("Введите имя файла: ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = '\0';
                
                // Считываем метаданные файла из файловой системы
                get_file_permissions(input, &perm);
                print_permissions(&perm);
                break;
            }
            case 3: {
                // Запрос команды изменения прав в стиле утилиты chmod
                printf("Введите команду изменения прав (например, u+rwx,g-x): ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = '\0';
                
                // Парсим строку команды и модифицируем битовую маску в структуре
                process_chmod_commands(&perm, input);
                print_permissions(&perm); // Показываем обновленные права
                break;
            }
            case 4:
                // Запуск юнит-тестов непосредственно из приложения
                run_all_tests();
                break;
            case 5:
                running = 0; // Завершаем цикл и выходим из программы
                break;
            default:
                printf("Неверный выбор!\n"); // Обработка некорректного ввода
        }
    }
    return 0;
}
```

---

## **Тесты (tests.c):**

Представленные тесты предназначены для проверки корректности работы 
функций управления правами доступа: парсинг восьмеричного и символьного 
форматов, обработка chmod-подобных команд (добавление, удаление, точное 
присвоение прав), работа с множественными командами через запятую, 
корректность обновления символьного представления при ручном изменении 
битовой маски.

```c
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "permissions.h"

// Глобальные счетчики для статистики тестов
static int tests_passed = 0;  // Количество успешно пройденных проверок
static int tests_failed = 0;  // Количество проваленных проверок
static int tests_total = 0;   // Общее количество выполненных проверок

// --- Макросы для проверок с детальным логированием ---

// Проверка числового равенства: если значения совпадают, тест пройден
#define ASSERT_EQ(expected, actual, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] Тест: %s\n", tests_total, test_name); \
        if ((expected) == (actual)) { \
            printf("     [PASS] Ожидалось: %d, Получено: %d\n", expected, actual); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Ожидалось: %d, Получено: %d\n", expected, actual); \
            tests_failed++; \
        } \
    } while(0)

// Проверка строкового равенства: если строки совпадают, тест пройден
#define ASSERT_STR_EQ(expected, actual, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] Тест: %s\n", tests_total, test_name); \
        if (strcmp((expected), (actual)) == 0) { \
            printf("     [PASS] Ожидалось: '%s', Получено: '%s'\n", expected, actual); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Ожидалось: '%s', Получено: '%s'\n", expected, actual); \
            tests_failed++; \
        } \
    } while(0)

// --- Группа тестов: Парсинг восьмеричного формата ---

// Тест 1: Проверка парсинга прав 755 из восьмеричного формата
static void test_parse_octal_755() {
    printf("\n--- Группа: Парсинг восьмеричного формата ---\n");
    Permissions perm = {0}; // Создаем чистую структуру, заполненную нулями
    parse_octal(0755, &perm); // Парсим восьмеричное значение 755
    
    // Проверяем, что восьмеричное представление сохранилось корректно
    ASSERT_EQ(0755, perm.octal, "Octal 755 - Восьмеричное значение");
    // Проверяем, что символьное представление сгенерировано правильно: rwxr-xr-x
    ASSERT_STR_EQ("rwxr-xr-x", perm.symbolic, "Octal 755 - Символьный вид");
}

// Тест 2: Проверка парсинга прав 644 из восьмеричного формата
static void test_parse_octal_644() {
    Permissions perm = {0};
    parse_octal(0644, &perm);
    // Проверяем символьное представление: rw-r--r--
    ASSERT_STR_EQ("rw-r--r--", perm.symbolic, "Octal 644 - Символьный вид");
}

// --- Группа тестов: Парсинг символьного формата ---

// Тест 3: Проверка парсинга полных прав rwxrwxrwx из символьного формата
static void test_parse_symbolic_full() {
    printf("\n--- Группа: Парсинг символьного формата ---\n");
    Permissions perm = {0};
    parse_symbolic("rwxrwxrwx", &perm); // Парсим строку с максимальными правами
    
    // Проверяем, что восьмеричное значение стало 777
    ASSERT_EQ(0777, perm.octal, "Symbolic rwxrwxrwx - Восьмеричное значение");
}

// Тест 4: Проверка парсинга пустых прав --------- из символьного формата
static void test_parse_symbolic_none() {
    Permissions perm = {0};
    parse_symbolic("---------", &perm); // Парсим строку без каких-либо прав
    
    // Проверяем, что восьмеричное значение стало 000
    ASSERT_EQ(0000, perm.octal, "Symbolic --------- - Восьмеричное значение");
}

// --- Группа тестов: Обработка chmod-команд ---

// Тест 5: Добавление права на исполнение владельцу (u+x)
static void test_chmod_add_user_exec() {
    printf("\n--- Группа: Обработка chmod-команд ---\n");
    Permissions perm = {0};
    parse_octal(0000, &perm); // База: --------- (нет никаких прав)
    process_chmod_commands(&perm, "u+x"); // Добавляем x владельцу
    
    // Проверяем символьное представление: --x------
    ASSERT_STR_EQ("--x------", perm.symbolic, "chmod u+x - Добавление x владельцу");
    // Проверяем восьмеричное значение: 100
    ASSERT_EQ(0100, perm.octal, "chmod u+x - Восьмеричный вид");
}

// Тест 6: Удаление права на запись у группы (g-w)
static void test_chmod_remove_group_write() {
    Permissions perm = {0};
    parse_octal(0777, &perm); // База: rwxrwxrwx (полные права)
    process_chmod_commands(&perm, "g-w"); // Удаляем w у группы
    
    // Проверяем символьное представление: rwxr-xrwx
    ASSERT_STR_EQ("rwxr-xrwx", perm.symbolic, "chmod g-w - Удаление w у группы");
    // Проверяем восьмеричное значение: 757
    ASSERT_EQ(0757, perm.octal, "chmod g-w - Восьмеричный вид");
}

// Тест 7: Точное присвоение права на чтение остальным (o=r)
static void test_chmod_set_other_read() {
    Permissions perm = {0};
    parse_octal(0777, &perm); // База: rwxrwxrwx (полные права)
    process_chmod_commands(&perm, "o=r"); // Устанавливаем только r для остальных
    
    // Проверяем символьное представление: rwxrwxr--
    ASSERT_STR_EQ("rwxrwxr--", perm.symbolic, "chmod o=r - Жесткое задание r для остальных");
    // Проверяем восьмеричное значение: 774
    ASSERT_EQ(0774, perm.octal, "chmod o=r - Восьмеричный вид");
}

// Тест 8: Добавление права на исполнение всем категориям (a+x)
static void test_chmod_all_exec() {
    Permissions perm = {0};
    parse_octal(0666, &perm); // База: rw-rw-rw- (чтение и запись для всех)
    process_chmod_commands(&perm, "a+x"); // Добавляем x всем
    
    // Проверяем символьное представление: rwxrwxrwx
    ASSERT_STR_EQ("rwxrwxrwx", perm.symbolic, "chmod a+x - Добавление x всем");
}

// Тест 9: Обработка множественных команд через запятую
static void test_chmod_multiple_commands() {
    Permissions perm = {0};
    parse_octal(0664, &perm); // База: rw-rw-r--
    // u+x (станет 7), g-w (станет 4), o-r (станет 0) => Итог: 740
    process_chmod_commands(&perm, "u+x,g-w,o-r");
    
    // Проверяем символьное представление: rwxr-----
    ASSERT_STR_EQ("rwxr-----", perm.symbolic, "Множественные команды (u+x,g-w,o-r) - Символьный вид");
    // Проверяем восьмеричное значение: 740
    ASSERT_EQ(0740, perm.octal, "Множественные команды (u+x,g-w,o-r) - Восьмеричный вид");
}

// --- Группа тестов: Обновление символьного представления ---

// Тест 10: Ручное изменение битовой маски и вызов update_symbolic
static void test_update_symbolic_manual() {
    printf("\n--- Группа: Обновление символьного представления ---\n");
    Permissions perm = {0};
    // Устанавливаем биты вручную: чтение и запись для владельца, исполнение для группы
    perm.binary = S_IRUSR | S_IWUSR | S_IXGRP;
    update_symbolic(&perm); // Пересчитываем символьное представление
    
    // Проверяем, что получилось: rw---x---
    ASSERT_STR_EQ("rw---x---", perm.symbolic, "Ручное изменение binary и вызов update_symbolic");
}

// --- Основная функция запуска всех тестов ---
// Вызывается из main.c при выборе пункта 4 в меню
void run_all_tests(void) {
    printf("\n");
    printf("==================================================\n");
    printf("              ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("==================================================\n");
    
    // Сброс счетчиков перед каждым запуском
    tests_passed = 0;
    tests_failed = 0;
    tests_total = 0;

    // Запуск тестов парсинга
    test_parse_octal_755();
    test_parse_octal_644();
    test_parse_symbolic_full();
    test_parse_symbolic_none();
    
    // Запуск тестов chmod-логики
    test_chmod_add_user_exec();
    test_chmod_remove_group_write();
    test_chmod_set_other_read();
    test_chmod_all_exec();
    test_chmod_multiple_commands();
    
    // Запуск теста обновления символьного представления
    test_update_symbolic_manual();

    // Итоговый отчет с подробной статистикой
    printf("\n==================================================\n");
    printf("                    РЕЗУЛЬТАТЫ\n");
    printf("==================================================\n");
    printf("Всего тестов выполнено: %d\n", tests_total);
    printf("Успешно пройдено:     %d\n", tests_passed);
    printf("Провалено:            %d\n", tests_failed);
    printf("==================================================\n");
    
    if (tests_failed > 0) {
        printf("СТАТУС: [FAIL] Обнаружены ошибки в реализации\n");
    } else {
        printf("СТАТУС: [PASS] Все тесты пройдены успешно\n");
    }
    printf("==================================================\n\n");
}
```