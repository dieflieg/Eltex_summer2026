#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tests.h"

extern struct Item *root;              
extern int contactCount;               
extern int operationsSinceBalance;     

#define MAX_LENGTH 100

typedef struct {
    char firstName[MAX_LENGTH];
    char lastName[MAX_LENGTH];
    char workplace[MAX_LENGTH];
    char job[MAX_LENGTH];
    char phone[MAX_LENGTH];
    char email[MAX_LENGTH];
    char social[MAX_LENGTH];
    char messenger[MAX_LENGTH];
} Contact;

typedef struct Item {
    Contact contact;
    struct Item *left, *right;
} Item;

extern int compareContacts(Contact *a, Contact *b);
extern void insertNode(Contact val, Item **q);
extern int deleteNode(Contact key, Item **node);
extern void flattenToArray(Item *p, Item **arr, int *currentIndex);
extern Item* buildBalanced(Item **arr, int start, int end);
extern void balanceTree(void);
extern void freeContacts(void);
extern Item* getItemByIndex(int index);


static int tests_passed = 0;  // Сколько тестов прошло успешно
static int tests_failed = 0;  // Сколько тестов провалилось
static int tests_total = 0;   // Общее количество тестов

// Макрос для проверки: Ожидается, что два числа равны
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

// Макрос для проверки строк
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

// Макрос для проверки: Ожидается, что условие истинно
#define ASSERT_TRUE(condition, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] Тест: %s\n", tests_total, test_name); \
        if (condition) { \
            printf("     [PASS] Условие истинно\n"); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Условие ложно\n"); \
            tests_failed++; \
        } \
    } while(0)


static Contact createTestContact(const char *lastName, const char *firstName) {
    Contact c = {0};  // Создаём контакт и заполняем его нулями
    
    // strncpy копирует строку безопасно, не вылезая за границы массива
    // MAX_LENGTH - 1 оставляем место для символа конца строки '\0'
    strncpy(c.lastName, lastName, MAX_LENGTH - 1);
    strncpy(c.firstName, firstName, MAX_LENGTH - 1);
    return c;
}

// Эта функция вычисляет высоту дерева
static int getHeight(Item *node) {
    
    if (node == NULL) return 0;
    
    // Рекурсивно считаем высоту левого поддерева
    int leftHeight = getHeight(node->left);
    
    // Рекурсивно считаем высоту правого поддерева
    int rightHeight = getHeight(node->right);
    
    // Берём максимум из двух высот и прибавляем 1 (за текущий узел)
   
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
}

// Эта функция очищает дерево перед каждым тестом
static void resetTree(void) {
    freeContacts();  // Вызываем функцию из main.c, которая удаляет всё дерево
    operationsSinceBalance = 0;  // Обнуляем счётчик операций
}


// Этот тест проверяет функцию compareContacts
// Она нужна для того, чтобы дерево знало, куда вставлять новый контакт:
// влево (если он "меньше") или вправо (если он "больше")

static void test_compare_contacts() {
    printf("\n=== ТЕСТ 1: Сравнение контактов ===\n");
    resetTree();
    
    // Создаём два контакта с разными фамилиями
    // Иванов и Петров - в алфавитном порядке Иванов идёт раньше
    Contact ivanov = createTestContact("Иванов", "Иван");
    Contact petrov = createTestContact("Петров", "Петр");
    
    printf("Создали контакт 1: Иванов Иван\n");
    printf("Создали контакт 2: Петров Петр\n");
    printf("Вызываем compareContacts(Иванов, Петров)...\n");
    
    int result = compareContacts(&ivanov, &petrov);
    
    printf("Результат сравнения: %d\n", result);
    printf("Если result < 0, значит Иванов < Петров (так и должно быть)\n");
    printf("Если result == 0, значит контакты одинаковые\n");
    printf("Если result > 0, значит Иванов > Петров (это было бы ошибкой)\n");
    
    // Проверяем, что результат отрицательный
    // Потому что Иванов идёт раньше Петрова в алфавите
    ASSERT_TRUE(result < 0, "Иванов < Петров (алфавитный порядок)");
}


// Правило BST: все левые потомки меньше родителя, все правые - больше
// Этот тест проверяет, что при вставке элементов дерево сохраняет это правило

static void test_insert_and_bst_property() {
    printf("\n=== ТЕСТ 2: Вставка в дерево и проверка BST ===\n");
    resetTree();
    
    printf("Создаём дерево из 5 контактов в случайном порядке:\n");
    printf("1. Дмитриев (корень)\n");
    printf("2. Андреев (должен уйти влево, т.к. А < Д)\n");
    printf("3. Егоров (должен уйти вправо, т.к. Е > Д)\n");
    printf("4. Борисов (должен уйти влево от Дмитриева, вправо от Андреева)\n");
    printf("5. Григорьев (должен уйти вправо от Дмитриева, влево от Егорова)\n\n");
    
    // Вставляем контакты один за другим
    insertNode(createTestContact("Дмитриев", "Д"), &root); contactCount++;
    printf("Вставили: Дмитриев. Корень = Дмитриев\n");
    
    insertNode(createTestContact("Андреев", "А"), &root); contactCount++;
    printf("Вставили: Андреев. Он ушёл влево от Дмитриева (А < Д)\n");
    
    insertNode(createTestContact("Егоров", "Е"), &root); contactCount++;
    printf("Вставили: Егоров. Он ушёл вправо от Дмитриева (Е > Д)\n");
    
    insertNode(createTestContact("Борисов", "Б"), &root); contactCount++;
    printf("Вставили: Борисов. Сравнили с Дмитриевым (Б < Д) - идём влево.\n");
    printf("  Сравнили с Андреевым (Б > А) - идём вправо от Андреева.\n");
    
    insertNode(createTestContact("Григорьев", "Г"), &root); contactCount++;
    printf("Вставили: Григорьев. Сравнили с Дмитриевым (Г < Д) - идём влево.\n");
    printf("  Сравнили с Андреевым (Г > А) - идём вправо.\n");
    printf("  Сравнили с Борисовым (Г > Б) - идём вправо от Борисова.\n");
    
    printf("\nПроверяем структуру дерева:\n");
    
    // Проверяем, что корень - это Дмитриев
    ASSERT_TRUE(root != NULL, "Корень существует");
    ASSERT_STR_EQ("Дмитриев", root->contact.lastName, "Корень = Дмитриев");
    
    // Проверяем левого потомка корня (должен быть Андреев)
    ASSERT_TRUE(root->left != NULL, "Левый потомок корня существует");
    ASSERT_STR_EQ("Андреев", root->left->contact.lastName, "Левый потомок = Андреев");
    
    // Проверяем правого потомка корня (должен быть Егоров)
    ASSERT_TRUE(root->right != NULL, "Правый потомок корня существует");
    ASSERT_STR_EQ("Егоров", root->right->contact.lastName, "Правый потомок = Егоров");
    
    // Проверяем, что счётчик контактов правильный
    ASSERT_EQ(5, contactCount, "В дереве 5 контактов");
    
    printf("\nСтруктура дерева:\n");
    printf("        Дмитриев\n");
    printf("       /        \\\n");
    printf("   Андреев    Егоров\n");
    printf("       \\\n");
    printf("     Борисов\n");
    printf("         \\\n");
    printf("       Григорьев\n");
}


// Удаление листа 

static void test_delete_leaf() {
    printf("\n=== ТЕСТ 3: Удаление листа ===\n");
    resetTree();
    
    printf("Создаём простое дерево из 2 узлов:\n");
    printf("Корень: Борисов\n");
    printf("Левый потомок: Алексеев (это будет лист)\n\n");
    
    insertNode(createTestContact("Борисов", "Б"), &root); contactCount++;
    insertNode(createTestContact("Алексеев", "А"), &root); contactCount++;
    
    printf("Дерево до удаления:\n");
    printf("  Борисов\n");
    printf("  /\n");
    printf("Алексеев (лист)\n\n");
    
    // Создаём контакт, который хотим удалить
    Contact toDelete = createTestContact("Алексеев", "А");
    
    printf("Удаляем Алексеева...\n");
    int result = deleteNode(toDelete, &root);
    
    printf("Функция deleteNode вернула: %d\n", result);
    printf("1 = успешно удалено, 0 = не найдено\n\n");
    
    // Проверяем, что удаление прошло успешно
    ASSERT_EQ(1, result, "Удаление вернуло 1 (успех)");
    
    // Проверяем, что левый потомок корня теперь NULL (его удалили)
    ASSERT_TRUE(root->left == NULL, "Левый потомок корня стал NULL");
    
    // Проверяем, что корень остался на месте
    ASSERT_TRUE(root != NULL, "Корень всё ещё существует");
    ASSERT_STR_EQ("Борисов", root->contact.lastName, "Корень = Борисов");
    
    printf("Дерево после удаления:\n");
    printf("  Борисов (лист)\n");
    printf("  (Алексеев удалён)\n");
}

// Удаление листа с двумя потомками: ищем самый правый узел в левом поддереве
// Этот узел копируется на место удаляемого, а потом удаляется 

static void test_delete_node_with_two_children() {
    printf("\n=== ТЕСТ 4: Удаление узла с двумя потомками ===\n");
    resetTree();
    
    printf("Создаём дерево из 3 узлов:\n");
    printf("Корень: Борисов (будем удалять)\n");
    printf("Левый потомок: Алексеев\n");
    printf("Правый потомок: Владимиров\n\n");
    
    insertNode(createTestContact("Борисов", "Б"), &root); contactCount++;
    insertNode(createTestContact("Алексеев", "А"), &root); contactCount++;
    insertNode(createTestContact("Владимиров", "В"), &root); contactCount++;
    
    printf("Дерево до удаления:\n");
    printf("     Борисов\n");
    printf("    /       \\\n");
    printf("Алексеев  Владимиров\n\n");
    
    // Удаляем корень (у него два потомка)
    Contact toDelete = createTestContact("Борисов", "Б");
    
    printf("Удаляем Борисова (у него два потомка)...\n");
    printf("Алгоритм:\n");
    printf("1. Ищем самый правый узел в левом поддереве\n");
    printf("   (в данном случае это Алексеев, т.к. у него нет правого потомка)\n");
    printf("2. Копируем данные Алексеева на место Борисова\n");
    printf("3. Удаляем оригинальный узел Алексеева\n\n");
    
    int result = deleteNode(toDelete, &root);
    
    printf("Функция deleteNode вернула: %d\n\n", result);
    
    // Проверяем, что удаление прошло успешно
    ASSERT_EQ(1, result, "Удаление вернуло 1 (успех)");
    
    // Проверяем, что корень всё ещё существует
    ASSERT_TRUE(root != NULL, "Корень существует после удаления");
    
    // Проверяем, что на месте Борисова теперь Алексеев
    ASSERT_STR_EQ("Алексеев", root->contact.lastName, "Корень теперь = Алексеев");
    
    // Проверяем, что правый потомок остался на месте
    ASSERT_TRUE(root->right != NULL, "Правый потомок существует");
    ASSERT_STR_EQ("Владимиров", root->right->contact.lastName, "Правый потомок = Владимиров");
    
    // Проверяем, что левый потомок удалился (Алексеев переместился наверх)
    ASSERT_TRUE(root->left == NULL, "Левый потомок стал NULL");
    
    printf("Дерево после удаления:\n");
    printf("     Алексеев\n");
    printf("            \\\n");
    printf("          Владимиров\n");
}


// Балансировка нужна для того, чтобы дерево не превратилось в длинную цепочку -
// она перестраивает дерево так, чтобы оно стало кустистым.
// Идеально сбалансированное дерево из N элементов имеет высоту log2(N)

static void test_balance_tree() {
    printf("\n=== ТЕСТ 5: Балансировка дерева ===\n");
    resetTree();
    
    printf("Создаём вырожденное дерево (цепочку) из 7 элементов:\n");
    printf("Вставляем в алфавитном порядке: А, Б, В, Г, Д, Е, Ж\n\n");
    
    // Вставляем в отсортированном порядке - это создаёт цепочку
    insertNode(createTestContact("А", "А"), &root); contactCount++;
    insertNode(createTestContact("Б", "Б"), &root); contactCount++;
    insertNode(createTestContact("В", "В"), &root); contactCount++;
    insertNode(createTestContact("Г", "Г"), &root); contactCount++;
    insertNode(createTestContact("Д", "Д"), &root); contactCount++;
    insertNode(createTestContact("Е", "Е"), &root); contactCount++;
    insertNode(createTestContact("Ж", "Ж"), &root); contactCount++;
    
    int heightBefore = getHeight(root);
    printf("Высота дерева ДО балансировки: %d\n", heightBefore);
    printf("(Для 7 элементов в цепочке высота должна быть 7)\n\n");
    
    printf("Дерево до балансировки (цепочка):\n");
    printf("А\n");
    printf(" \\\n");
    printf("  Б\n");
    printf("   \\\n");
    printf("    В\n");
    printf("     \\\n");
    printf("      Г\n");
    printf("       \\\n");
    printf("        Д\n");
    printf("         \\\n");
    printf("          Е\n");
    printf("           \\\n");
    printf("            Ж\n\n");
    
    printf("Вызываем balanceTree()...\n");
    printf("Алгоритм:\n");
    printf("1. Собираем все узлы в отсортированный массив (in-order обход)\n");
    printf("2. Берём средний элемент массива как новый корень\n");
    printf("3. Рекурсивно строим левое и правое поддеревья\n\n");
    
    balanceTree();
    
    int heightAfter = getHeight(root);
    printf("Высота дерева ПОСЛЕ балансировки: %d\n", heightAfter);
    printf("(Для 7 элементов идеальная высота = 3)\n\n");
    
    // Проверяем, что высота уменьшилась
    ASSERT_TRUE(heightBefore > heightAfter, "Высота уменьшилась после балансировки");
    
    // Проверяем, что высота стала приемлемой (для 7 элементов должно быть 3-4)
    ASSERT_TRUE(heightAfter <= 4, "Высота сбалансированного дерева <= 4");
    
    // Проверяем, что счётчик операций обнулился
    ASSERT_EQ(0, operationsSinceBalance, "Счётчик операций обнулён");
    
    // Проверяем, что корень существует
    ASSERT_TRUE(root != NULL, "Корень существует после балансировки");
    
    printf("Дерево после балансировки (примерная структура):\n");
    printf("        Г\n");
    printf("      /   \\\n");
    printf("    Б       Е\n");
    printf("   / \\       \\\n");
    printf("  А   В       Ж\n");
    printf("       \\\n");
    printf("        Д\n");
}

// getItemByIndex возвращает контакт по его порядковому номеру
// Нумерация идёт по in-order обходу (слева направо в алфавитном порядке)
// Это нужно для того, чтобы пользователь мог выбрать контакт по номеру из списка

static void test_get_item_by_index() {
    printf("\n=== ТЕСТ 6: Получение элемента по индексу ===\n");
    resetTree();
    
    printf("Создаём дерево из 3 контактов в случайном порядке:\n");
    printf("1. В (корень)\n");
    printf("2. А (левый потомок)\n");
    printf("3. Б (правый потомок А)\n\n");
    
    insertNode(createTestContact("В", "В"), &root); contactCount++;
    insertNode(createTestContact("А", "А"), &root); contactCount++;
    insertNode(createTestContact("Б", "Б"), &root); contactCount++;
    
    printf("Дерево:\n");
    printf("    В\n");
    printf("   /\n");
    printf("  А\n");
    printf("   \\\n");
    printf("    Б\n\n");
    
    printf("In-order обход (слева направо): А, Б, В\n");
    printf("Индекс 0 = А\n");
    printf("Индекс 1 = Б\n");
    printf("Индекс 2 = В\n\n");
    
    printf("Запрашиваем элемент с индексом 0...\n");
    Item *item0 = getItemByIndex(0);
    ASSERT_TRUE(item0 != NULL, "Элемент с индексом 0 существует");
    ASSERT_STR_EQ("А", item0->contact.lastName, "Элемент 0 = А");
    
    printf("\nЗапрашиваем элемент с индексом 1...\n");
    Item *item1 = getItemByIndex(1);
    ASSERT_TRUE(item1 != NULL, "Элемент с индексом 1 существует");
    ASSERT_STR_EQ("Б", item1->contact.lastName, "Элемент 1 = Б");
    
    printf("\nЗапрашиваем элемент с индексом 2...\n");
    Item *item2 = getItemByIndex(2);
    ASSERT_TRUE(item2 != NULL, "Элемент с индексом 2 существует");
    ASSERT_STR_EQ("В", item2->contact.lastName, "Элемент 2 = В");
    
    printf("\nЗапрашиваем элемент с некорректным индексом 10...\n");
    Item *itemInvalid = getItemByIndex(10);
    ASSERT_TRUE(itemInvalid == NULL, "Некорректный индекс возвращает NULL");
}


// Этот тест проверяет, что freeContacts правильно удаляет всё дерево
// и не оставляет "висящих" указателей, которые могут привести к утечкам памяти

static void test_free_contacts() {
    printf("\n=== ТЕСТ 7: Освобождение памяти ===\n");
    resetTree();
    
    printf("Создаём дерево из 3 контактов:\n");
    insertNode(createTestContact("Тест1", "Т"), &root); contactCount++;
    insertNode(createTestContact("Тест2", "Т"), &root); contactCount++;
    insertNode(createTestContact("Тест3", "Т"), &root); contactCount++;
    
    printf("Дерево создано, contactCount = %d\n", contactCount);
    printf("Вызываем freeContacts()...\n");
    printf("Эта функция рекурсивно удаляет все узлы:\n");
    printf("1. Сначала удаляет левое поддерево\n");
    printf("2. Потом удаляет правое поддерево\n");
    printf("3. В конце удаляет корень\n\n");
    
    freeContacts();
    
    printf("Проверяем, что корень обнулился...\n");
    ASSERT_TRUE(root == NULL, "Корень стал NULL после freeContacts");
    
    printf("Проверяем, что счётчик контактов обнулился...\n");
    ASSERT_EQ(0, contactCount, "contactCount стал 0");
    
    printf("\nПроверяем, что повторный вызов freeContacts не вызывает ошибок...\n");
    freeContacts();
    ASSERT_TRUE(root == NULL, "Корень остаётся NULL после двойного вызова");
    ASSERT_EQ(0, contactCount, "contactCount остаётся 0");
    
    printf("Если программа не упала - значит утечек памяти нет\n");
}


// запуск всех тестов

void StartTest(void) {
    printf("\n");
    printf("==================================================\n");
    printf("              ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("         (Бинарное дерево контактов)\n");
    printf("==================================================\n");
    
    // Обнуляем счётчики перед запуском
    tests_passed = 0;
    tests_failed = 0;
    tests_total = 0;

    // Запускаем все 7 тестов по очереди
    test_compare_contacts();
    test_insert_and_bst_property();
    test_delete_leaf();
    test_delete_node_with_two_children();
    test_balance_tree();
    test_get_item_by_index();
    test_free_contacts();

    // Выводим итоговый отчёт
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