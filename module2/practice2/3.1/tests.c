#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "permissions.h"

extern void parse_symbolic(const char *sym, Permissions *perm);
extern void parse_octal(int oct, Permissions *perm);
extern void update_symbolic(Permissions *perm);
extern void process_chmod_commands(Permissions *perm, const char *command);

static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0; 

// Макрос для проверки числового равенства
#define ASSERT_EQ(expected, actual, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] Тест: %s\n", tests_total, test_name); \
        if ((expected) == (actual)) { \
            printf("     [PASS] Ожидалось: %d (0%o), Получено: %d (0%o)\n", \
                   expected, expected, actual, actual); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Ожидалось: %d (0%o), Получено: %d (0%o)\n", \
                   expected, expected, actual, actual); \
            tests_failed++; \
        } \
    } while(0)

// Макрос для проверки строк
// strcmp возвращает 0, если строки совпадают
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

// ==========================================================================
// ТЕСТ 1: ПАРСИНГ ВОСЬМЕРИЧНОГО ФОРМАТА 755
// ==========================================================================

static void test_parse_octal_755() {
    printf("\n=== ТЕСТ 1: Парсинг восьмеричного формата 755 ===\n");
    
    printf("Исходные данные: восьмеричное число 755\n");
    printf("Разбираем по цифрам:\n");
    printf("  7 (владелец) = rwx = 4+2+1 = 111 в двоичном\n");
    printf("  5 (группа)   = r-x = 4+0+1 = 101 в двоичном\n");
    printf("  5 (остальные)= r-x = 4+0+1 = 101 в двоичном\n");
    printf("Ожидаемая строка прав: 'rwxr-xr-x'\n\n");
    
    printf("Создаём пустую структуру Permissions...\n");
    Permissions perm = {0};
    
    printf("Вызываем parse_octal(0755, &perm)...\n");
    parse_octal(0755, &perm);
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.binary   = 0x%X\n", perm.binary);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем, что восьмеричное значение сохранилось корректно
    ASSERT_EQ(0755, perm.octal, "Восьмеричное значение равно 755");
    
    // Проверяем, что символьное представление сгенерировано правильно
    ASSERT_STR_EQ("rwxr-xr-x", perm.symbolic, "Символьный вид = 'rwxr-xr-x'");
}

// ==========================================================================
// ТЕСТ 2: ПАРСИНГ ВОСЬМЕРИЧНОГО ФОРМАТА 644
// ==========================================================================

static void test_parse_octal_644() {
    printf("\n=== ТЕСТ 2: Парсинг восьмеричного формата 644 ===\n");
    
    printf("Исходные данные: восьмеричное число 644\n");
    printf("Это стандартные права для обычных файлов в Linux\n");
    printf("Разбираем по цифрам:\n");
    printf("  6 (владелец) = rw- = 4+2+0 = 110 в двоичном\n");
    printf("  4 (группа)   = r-- = 4+0+0 = 100 в двоичном\n");
    printf("  4 (остальные)= r-- = 4+0+0 = 100 в двоичном\n");
    printf("Ожидаемая строка прав: 'rw-r--r--'\n\n");
    
    Permissions perm = {0};
    printf("Вызываем parse_octal(0644, &perm)...\n");
    parse_octal(0644, &perm);
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем символьное представление
    ASSERT_STR_EQ("rw-r--r--", perm.symbolic, "Символьный вид = 'rw-r--r--'");
}

// ==========================================================================
// ТЕСТ 3: ПАРСИНГ СИМВОЛЬНОГО ФОРМАТА "rwxrwxrwx"
// ==========================================================================

static void test_parse_symbolic_full() {
    printf("\n=== ТЕСТ 3: Парсинг символьного формата 'rwxrwxrwx' ===\n");
    
    printf("Исходные данные: строка 'rwxrwxrwx'\n");
    printf("Это максимальные права - все могут читать, писать и исполнять\n");
    printf("Разбираем по символам:\n");
    printf("  rwx (владелец) = 4+2+1 = 7\n");
    printf("  rwx (группа)   = 4+2+1 = 7\n");
    printf("  rwx (остальные)= 4+2+1 = 7\n");
    printf("Ожидаемое восьмеричное значение: 777\n\n");
    
    Permissions perm = {0};
    printf("Вызываем parse_symbolic('rwxrwxrwx', &perm)...\n");
    parse_symbolic("rwxrwxrwx", &perm);
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.binary   = 0x%X\n", perm.binary);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем, что восьмеричное значение равно 777
    ASSERT_EQ(0777, perm.octal, "Восьмеричное значение = 777");
}

// ==========================================================================
// ТЕСТ 4: ПАРСИНГ СИМВОЛЬНОГО ФОРМАТА "---------"
// ==========================================================================

static void test_parse_symbolic_none() {
    printf("\n=== ТЕСТ 4: Парсинг символьного формата '---------' ===\n");
    
    printf("Исходные данные: строка '---------'\n");
    printf("Это полное отсутствие прав - никто ничего не может делать\n");
    printf("Все 9 символов - прочерки, значит все биты = 0\n");
    printf("Ожидаемое восьмеричное значение: 000\n\n");
    
    Permissions perm = {0};
    printf("Вызываем parse_symbolic('---------', &perm)...\n");
    parse_symbolic("---------", &perm);
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.binary   = 0x%X\n\n", perm.binary);
    
    // Проверяем, что восьмеричное значение равно 0
    ASSERT_EQ(0000, perm.octal, "Восьмеричное значение = 000");
}

// ==========================================================================
// ТЕСТ 5: CHMOD u+x (ДОБАВЛЕНИЕ ИСПОЛНЕНИЯ ВЛАДЕЛЬЦУ)
// ==========================================================================

static void test_chmod_add_user_exec() {
    printf("\n=== ТЕСТ 5: chmod u+x (добавление x владельцу) ===\n");
    
    printf("Начальное состояние: 000 (--------)\n");
    printf("Команда: u+x\n");
    printf("Расшифровка:\n");
    printf("  u - user (владелец)\n");
    printf("  + - добавить право\n");
    printf("  x - execute (исполнение)\n\n");
    
    printf("Пошаговое выполнение:\n");
    printf("  1. Парсим команду: who='u', op='+', rights='x'\n");
    printf("  2. Маска категории u: 111000000 (S_IRWXU)\n");
    printf("  3. Биты права x:      001000000 (S_IXUSR)\n");
    printf("  4. bits & mask = 001000000 (пересечение)\n");
    printf("  5. Операция '+': perm.binary |= 001000000\n");
    printf("  6. Результат: 000000000 | 001000000 = 001000000\n\n");
    
    Permissions perm = {0};
    parse_octal(0000, &perm);  // Начинаем с нуля
    
    printf("Вызываем process_chmod_commands(&perm, 'u+x')...\n");
    process_chmod_commands(&perm, "u+x");
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем, что символьное представление = "--x------"
    ASSERT_STR_EQ("--x------", perm.symbolic, "Символьный вид = '--x------'");
    
    // Проверяем, что восьмеричное значение = 100
    ASSERT_EQ(0100, perm.octal, "Восьмеричный вид = 100");
}

// ==========================================================================
// ТЕСТ 6: CHMOD g-w (УДАЛЕНИЕ ЗАПИСИ У ГРУППЫ)
// ==========================================================================

static void test_chmod_remove_group_write() {
    printf("\n=== ТЕСТ 6: chmod g-w (удаление w у группы) ===\n");
    
    printf("Начальное состояние: 777 (rwxrwxrwx)\n");
    printf("Команда: g-w\n");
    printf("Расшифровка:\n");
    printf("  g - group (группа)\n");
    printf("  - - удалить право\n");
    printf("  w - write (запись)\n\n");
    
    printf("Пошаговое выполнение:\n");
    printf("  1. Парсим команду: who='g', op='-', rights='w'\n");
    printf("  2. Маска категории g: 000111000 (S_IRWXG)\n");
    printf("  3. Биты права w:      000010000 (S_IWGRP)\n");
    printf("  4. bits & mask = 000010000\n");
    printf("  5. Инвертируем: ~000010000 = 111101111\n");
    printf("  6. Операция '-': perm.binary &= 111101111\n");
    printf("  7. Результат: 111111111 & 111101111 = 111101111\n\n");
    
    Permissions perm = {0};
    parse_octal(0777, &perm);  // Начинаем с полных прав
    
    printf("Вызываем process_chmod_commands(&perm, 'g-w')...\n");
    process_chmod_commands(&perm, "g-w");
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем символьное представление
    ASSERT_STR_EQ("rwxr-xrwx", perm.symbolic, "Символьный вид = 'rwxr-xrwx'");
    
    // Проверяем восьмеричное значение = 757
    ASSERT_EQ(0757, perm.octal, "Восьмеричный вид = 757");
}

// ==========================================================================
// ТЕСТ 7: CHMOD o=r (ТОЧНОЕ ПРИСВОЕНИЕ ЧТЕНИЯ ОСТАЛЬНЫМ)
// ==========================================================================

static void test_chmod_set_other_read() {
    printf("\n=== ТЕСТ 7: chmod o=r (присвоение r для остальных) ===\n");
    
    printf("Начальное состояние: 777 (rwxrwxrwx)\n");
    printf("Команда: o=r\n");
    printf("Расшифровка:\n");
    printf("  o - other (остальные)\n");
    printf("  = - точно присвоить (заменить старые права)\n");
    printf("  r - read (чтение)\n\n");
    
    printf("Пошаговое выполнение:\n");
    printf("  1. who='o', op='=', rights='r'\n");
    printf("  2. Маска категории o: 000000111 (S_IRWXO)\n");
    printf("  3. Биты права r:      000000100 (S_IROTH)\n");
    printf("  4. Обнуляем категорию: perm.binary & ~000000111\n");
    printf("     111111111 & 111111000 = 111111000\n");
    printf("  5. Устанавливаем новое: | 000000100\n");
    printf("     111111000 | 000000100 = 111111100\n\n");
    
    Permissions perm = {0};
    parse_octal(0777, &perm);
    
    printf("Вызываем process_chmod_commands(&perm, 'o=r')...\n");
    process_chmod_commands(&perm, "o=r");
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем символьное представление
    ASSERT_STR_EQ("rwxrwxr--", perm.symbolic, "Символьный вид = 'rwxrwxr--'");
    
    // Проверяем восьмеричное значение = 774
    ASSERT_EQ(0774, perm.octal, "Восьмеричный вид = 774");
}

// ==========================================================================
// ТЕСТ 8: CHMOD a+x (ДОБАВЛЕНИЕ ИСПОЛНЕНИЯ ВСЕМ)
// ==========================================================================

static void test_chmod_all_exec() {
    printf("\n=== ТЕСТ 8: chmod a+x (добавление x всем) ===\n");
    
    printf("Начальное состояние: 666 (rw-rw-rw-)\n");
    printf("Команда: a+x\n");
    printf("Расшифровка:\n");
    printf("  a - all (все категории: u, g, o)\n");
    printf("  + - добавить\n");
    printf("  x - execute (исполнение)\n\n");
    
    printf("Флаг 'a' эквивалентен 'ugo' (все три категории)\n");
    printf("Это значит, что x будет добавлен:\n");
    printf("  - владельцу (u)\n");
    printf("  - группе (g)\n");
    printf("  - остальным (o)\n\n");
    
    Permissions perm = {0};
    parse_octal(0666, &perm);  // Начинаем с rw-rw-rw-
    
    printf("Вызываем process_chmod_commands(&perm, 'a+x')...\n");
    process_chmod_commands(&perm, "a+x");
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем, что у всех добавился x
    ASSERT_STR_EQ("rwxrwxrwx", perm.symbolic, "Символьный вид = 'rwxrwxrwx'");
}

// ==========================================================================
// ТЕСТ 9: МНОЖЕСТВЕННЫЕ КОМАНДЫ u+x,g-w,o-r
// ==========================================================================

static void test_chmod_multiple_commands() {
    printf("\n=== ТЕСТ 9: Множественные команды (u+x,g-w,o-r) ===\n");
    
    printf("Начальное состояние: 664 (rw-rw-r--)\n");
    printf("Команды: u+x,g-w,o-r (через запятую)\n\n");
    
    printf("Пошаговое выполнение:\n");
    printf("  Шаг 1: u+x (добавить x владельцу)\n");
    printf("    664 -> 764 (rw-rw-r-- -> rwxrw-r--)\n");
    printf("  Шаг 2: g-w (убрать w у группы)\n");
    printf("    764 -> 744 (rwxrw-r-- -> rwxr--r--)\n");
    printf("  Шаг 3: o-r (убрать r у остальных)\n");
    printf("    744 -> 740 (rwxr--r-- -> rwxr-----)\n\n");
    
    Permissions perm = {0};
    parse_octal(0664, &perm);
    
    printf("Вызываем process_chmod_commands(&perm, 'u+x,g-w,o-r')...\n");
    process_chmod_commands(&perm, "u+x,g-w,o-r");
    
    printf("После вызова в структуре:\n");
    printf("  perm.octal    = 0%o\n", perm.octal);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем итоговое символьное представление
    ASSERT_STR_EQ("rwxr-----", perm.symbolic, "Символьный вид = 'rwxr-----'");
    
    // Проверяем итоговое восьмеричное значение
    ASSERT_EQ(0740, perm.octal, "Восьмеричный вид = 740");
}

// ==========================================================================
// ТЕСТ 10: РУЧНОЕ ОБНОВЛЕНИЕ СИМВОЛЬНОГО ПРЕДСТАВЛЕНИЯ
// ==========================================================================

static void test_update_symbolic_manual() {
    printf("\n=== ТЕСТ 10: Ручное обновление символьного представления ===\n");
    
    printf("Создаём структуру Permissions и вручную задаём биты:\n");
    printf("  S_IRUSR (чтение владельца)  = 100000000\n");
    printf("  S_IWUSR (запись владельца)  = 010000000\n");
    printf("  S_IXGRP (исполнение группы) = 000001000\n");
    printf("  Сумма (через |):            = 110001000\n\n");
    
    Permissions perm = {0};
    // Устанавливаем биты вручную через побитовое ИЛИ
    perm.binary = S_IRUSR | S_IWUSR | S_IXGRP;
    
    printf("Вызываем update_symbolic(&perm)...\n");
    update_symbolic(&perm);
    
    printf("После вызова в структуре:\n");
    printf("  perm.binary   = 0x%X\n", perm.binary);
    printf("  perm.symbolic = '%s'\n\n", perm.symbolic);
    
    // Проверяем, что строка сгенерировалась правильно
    ASSERT_STR_EQ("rw---x---", perm.symbolic, "Символьный вид = 'rw---x---'");
}

// ==========================================================================
// ГЛАВНАЯ ФУНКЦИЯ ЗАПУСКА ТЕСТОВ
// ==========================================================================

void run_all_tests(void) {
    printf("\n");
    printf("==================================================\n");
    printf("              ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("        (Управление правами доступа chmod)\n");
    printf("==================================================\n");
    
    // Обнуляем счётчики перед запуском
    tests_passed = 0;
    tests_failed = 0;
    tests_total = 0;

    // Группа 1: Парсинг восьмеричного формата
    test_parse_octal_755();
    test_parse_octal_644();
    
    // Группа 2: Парсинг символьного формата
    test_parse_symbolic_full();
    test_parse_symbolic_none();
    
    // Группа 3: Обработка chmod-команд
    test_chmod_add_user_exec();
    test_chmod_remove_group_write();
    test_chmod_set_other_read();
    test_chmod_all_exec();
    test_chmod_multiple_commands();
    
    // Группа 4: Обновление символьного представления
    test_update_symbolic_manual();

    // Итоговый отчёт
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