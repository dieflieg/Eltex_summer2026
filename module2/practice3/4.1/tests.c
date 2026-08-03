#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "tests.h"

#define MAX_LENGTH 100

extern Item *head;
extern int contactCount;

extern void insertItemSorted(Item *item);
extern void removeItemFromList(Item *item);
extern Item* getItemByIndex(int index);
extern void freeContacts();

static int tests_run = 0;
static int tests_passed = 0;

// Запускает один тестовый случай и выводит статус в консоль.
#define RUN_TEST(test) do { \
    printf("Запуск %-30s...", #test); \
    tests_run++; \
    int passed_before = tests_passed; \
    test(); \
    if (tests_passed > passed_before) { \
        printf(" [OK]\n"); \
    } \
} while (0)

// Проверяет истинность логического выражения (условия).
// Если условие ложно, выводятся подробности ошибки и тест прерывается. 
#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        printf("\n[ОШИБКА] Утверждение не выполнено: %s (строка %d)\n", #condition, __LINE__); \
        freeContacts(); \
        return; \
    } \
} while (0)


// Сравнивает две строки по содержимому. Если строки не совпадают, 
// выводит их значения и прерывает тест.
#define ASSERT_STR_EQ(str1, str2) do { \
    if (strcmp((str1), (str2)) != 0) { \
        printf("\n[ОШИБКА] Строки не совпадают: '%s' != '%s' (строка %d)\n", (str1), (str2), __LINE__); \
        freeContacts(); \
        return; \
    } \
} while (0)

// Вспомогательная функция для создания тестового узла
static Item* createTestItem(const char *lastName, const char *firstName) {
    Item *item = (Item*)malloc(sizeof(Item)); // Выделяем память под один узел списка
    if (!item) return NULL;
    memset(item, 0, sizeof(Item)); // Заполнение всего выделенного блока памяти нулями,
                                   // чтобы избежать случайных значений в указателях prev и next,
                                   // а также в строках структуры Contact.
    strncpy(item->contact.lastName, lastName, MAX_LENGTH - 1);  // Безопасно копирует строки фамилии 
    strncpy(item->contact.firstName, firstName, MAX_LENGTH - 1);// и имени в поля lastName и firstName
    return item; // Возвращаем указатель на созданный узел
}

// ============================================================================
// ТЕСТЫ
// ============================================================================

// Тест 1: Проверка вставки элемента в пустой список
void test_insert_into_empty_list() {
    freeContacts(); // Очищаем список на случай, если в нем остались данные от прошлых запусков
    
    Item *item = createTestItem("Удзумаки", "Наруто"); // Создаем тестовый узел
    insertItemSorted(item); // Вставляем узел в список с помощью тестируемой функции

    ASSERT_TRUE(head != NULL); // Проверяем: голова списка больше не пустая (элемент действительно добавлен)
    ASSERT_STR_EQ(head->contact.lastName, "Удзумаки"); // Проверяем: фамилия первого элемента совпадает с "Удзумаки"
    ASSERT_TRUE(head->prev == NULL); // Проверяем: у первого и единственного элемента нет левого соседа (prev == NULL)
    ASSERT_TRUE(head->next == NULL); // Проверяем: у первого и единственного элемента нет правого соседа (next == NULL)

    freeContacts(); // Очищаем память списка после завершения теста
    tests_passed++; // Увеличиваем счетчик успешно пройденных тестов
}

// Тест 2: Проверка автоматической сортировки при вставке нескольких элементов
void test_sorted_insertion() {
    freeContacts();
    
    // Добавляем контакты в заведомо неверном алфавитном порядке: Харуно, Удзумаки, Учиха
    insertItemSorted(createTestItem("Харуно", "Сакура"));
    insertItemSorted(createTestItem("Удзумаки", "Наруто"));
    insertItemSorted(createTestItem("Учиха", "Саске"));

    // Проверка прямой отсортированной цепочки (next): Удзумаки -> Учиха -> Харуно
    ASSERT_TRUE(head != NULL); // Голова списка существует, contact.lastName = "Удзумаки"
    ASSERT_STR_EQ(head->contact.lastName, "Удзумаки");
    
    ASSERT_TRUE(head->next != NULL); // Второй узел существует, contact.lastName = "Учиха"
    ASSERT_STR_EQ(head->next->contact.lastName, "Учиха");

    ASSERT_TRUE(head->next->next != NULL); // Третий узел существует, contact.lastName = "Харуно"
    ASSERT_STR_EQ(head->next->next->contact.lastName, "Харуно");
    
    // Проверка прямой отсортированной цепочки (prev): Удзумаки <- Учиха <- Харуно
    Item *haruno = head->next->next; // Запоминаем узел Харуно (последний)
    ASSERT_TRUE(haruno->prev != NULL);                       // Проверяем, что у Харуно
    ASSERT_STR_EQ(haruno->prev->contact.lastName, "Учиха");  // есть левый сосед (Учиха)
    ASSERT_STR_EQ(haruno->prev->prev->contact.lastName, "Удзумаки"); // Слева от Учихи должен 
                                                                     // стоять "Удзумаки"

    freeContacts(); 
    tests_passed++;
}

// Тест 3: Проверка функции поиска элемента по индексу
void test_get_item_by_index() {
    freeContacts();
    // Наполняем список отсортированными данными
    insertItemSorted(createTestItem("Удзумаки", "Наруто"));   // index 0
    insertItemSorted(createTestItem("Учиха", "Саске"));  // index 1
    insertItemSorted(createTestItem("Харуно", "Сакура"));// index 2
    
    // Запрашиваем элементы по индексам
    Item *item0 = getItemByIndex(0);
    Item *item1 = getItemByIndex(1);
    Item *item2 = getItemByIndex(2);
    Item *itemOut = getItemByIndex(99); // Запрашиваем заведомо несуществующий index 99

    // Проверяем правильность найденных узлов
    ASSERT_TRUE(item0 != NULL);
    ASSERT_STR_EQ(item0->contact.lastName, "Удзумаки");

    ASSERT_TRUE(item1 != NULL);
    ASSERT_STR_EQ(item1->contact.lastName, "Учиха");

    ASSERT_TRUE(item2 != NULL);
    ASSERT_STR_EQ(item2->contact.lastName, "Харуно");

    ASSERT_TRUE(itemOut == NULL); // Выход за границы диапазона должен возвращать NULL

    freeContacts();
    tests_passed++;
}

// Тест 4: Удаление первого элемента списка (головы)
void test_remove_head() {
    freeContacts();

    // Создаем и вставляем два узла
    Item *i1 = createTestItem("Удзумаки", "Наруто"); // index 0
    Item *i2 = createTestItem("Учиха", "Саске"); // index 1

    insertItemSorted(i1); // Вставляем первый элемент (голову списка)
    insertItemSorted(i2); // Вставляем второй элемент (будет стоять после головы)

    removeItemFromList(i1); // Удаляем первый элемент (голову списка)
    free(i1);               // и освобождаем память удаленного узла

                                        
    ASSERT_TRUE(head != NULL);                 // Проверяем, что голова списка существует,
    ASSERT_STR_EQ(head->contact.lastName, "Учиха"); // и что ее contact.lastName = "Учиха"
    ASSERT_TRUE(head->prev == NULL); // Левый сосед головы должен быть NULL

    freeContacts();
    tests_passed++;
}

// Тест 5: Удаление элемента из середины списка
void test_remove_middle() {
    freeContacts();

    // Создаем и вставляем три узла
    Item *i1 = createTestItem("Удзумаки", "Наруто");
    Item *i2 = createTestItem("Учиха", "Саске");
    Item *i3 = createTestItem("Харуно", "Сакура");
    
    // Вставляем их в список
    insertItemSorted(i1);
    insertItemSorted(i2);
    insertItemSorted(i3);

    removeItemFromList(i2); // Вырезаем элемент из середины списка
    free(i2);               // и освобождаем память удаленного узла

    // Проверяем, что i1 теперь указывает на i3, а i3 указывает на i1
    ASSERT_TRUE(i1->next == i3);
    ASSERT_TRUE(i3->prev == i1);

    freeContacts();
    tests_passed++;
}

// Тест 6: Удаление последнего элемента списка
void test_remove_tail() {
    freeContacts();

    // Создаем и вставляем два узла
    Item *i1 = createTestItem("Удзумаки", "Наруто");
    Item *i2 = createTestItem("Учиха", "Саске");
    insertItemSorted(i1);
    insertItemSorted(i2);

    removeItemFromList(i2);               // Удаляем Учиху (хвост)
    free(i2);               // и свобождаем память удаленного узла

                             // Убеждаемся, что указатель next у i1 теперь NULL, 
    ASSERT_TRUE(i1->next == NULL); // так как он стал последним элементом списка

    freeContacts();
    tests_passed++;
}

// Тест 7: Проверка полной очистки памяти списка
void test_free_contacts() {
    freeContacts();

    // Добавляем пару тестовых узлов
    insertItemSorted(createTestItem("Тест1", "Т"));
    insertItemSorted(createTestItem("Тест2", "Т"));
    
    freeContacts(); // Вызываем очистку всей памяти списка

    ASSERT_TRUE(head == NULL); // Проверяем: глобальный указатель head обнулился
    tests_passed++;
}

// ============================================================================
// ЗАПУСК ТЕСТОВ
// ============================================================================

void StartTest() {
    // Сохраняем пользовательские данные перед запуском тестов
    Item *saved_head = head;
    int saved_count = contactCount;
    
    head = NULL;
    contactCount = 0;

    printf("\n========================================\n");
    printf("   ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("========================================\n");

    tests_run = 0;
    tests_passed = 0;

    RUN_TEST(test_insert_into_empty_list);
    RUN_TEST(test_sorted_insertion);
    RUN_TEST(test_get_item_by_index);
    RUN_TEST(test_remove_head);
    RUN_TEST(test_remove_middle);
    RUN_TEST(test_remove_tail);
    RUN_TEST(test_free_contacts);

    printf("----------------------------------------\n");
    printf("РЕЗУЛЬТАТ: Успешно пройдены %d из %d тестов.\n", tests_passed, tests_run);
    printf("========================================\n\n");
    // Восстанавливаем данные пользователя
    head = saved_head;
    contactCount = saved_count;
    waitEnter();
}