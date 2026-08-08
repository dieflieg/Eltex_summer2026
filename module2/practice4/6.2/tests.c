#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "list.h"
#include "app.h"
#include "tests.h"

#define RUN_TEST(test) do { \
    printf("Запуск %-30s...", #test); \
    tests_run++; \
    int passed_before = tests_passed; \
    test(); \
    if (tests_passed > passed_before) { printf(" [OK]\n"); } \
} while (0)

#define ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        printf("\n[ОШИБКА] Утверждение не выполнено: %s (строка %d)\n", #condition, __LINE__); \
        freeContacts(); \
        return; \
    } \
} while (0)

#define ASSERT_STR_EQ(str1, str2) do { \
    if (strcmp((str1), (str2)) != 0) { \
        printf("\n[ОШИБКА] Строки не совпадают: '%s' != '%s' (строка %d)\n", (str1), (str2), __LINE__); \
        freeContacts(); \
        return; \
    } \
} while (0)

static Item* createTestItem(const char *lastName, const char *firstName) {
    Item *item = (Item*)malloc(sizeof(Item));
    if (!item) return NULL;
    memset(item, 0, sizeof(Item));
    strncpy(item->contact.lastName, lastName, MAX_LENGTH - 1);
    strncpy(item->contact.firstName, firstName, MAX_LENGTH - 1);
    return item;
}

void test_insert_into_empty_list() {
    freeContacts();
    Item *item = createTestItem("Удзумаки", "Наруто");
    insertItemSorted(item);
    ASSERT_TRUE(head != NULL);
    ASSERT_STR_EQ(head->contact.lastName, "Удзумаки");
    ASSERT_TRUE(head->prev == NULL);
    ASSERT_TRUE(head->next == NULL);
    freeContacts();
    tests_passed++;
}

void test_sorted_insertion() {
    freeContacts();
    insertItemSorted(createTestItem("Харуно", "Сакура"));
    insertItemSorted(createTestItem("Удзумаки", "Наруто"));
    insertItemSorted(createTestItem("Учиха", "Саске"));
    ASSERT_TRUE(head != NULL);
    ASSERT_STR_EQ(head->contact.lastName, "Удзумаки");
    ASSERT_TRUE(head->next != NULL);
    ASSERT_STR_EQ(head->next->contact.lastName, "Учиха");
    ASSERT_TRUE(head->next->next != NULL);
    ASSERT_STR_EQ(head->next->next->contact.lastName, "Харуно");
    Item *haruno = head->next->next;
    ASSERT_TRUE(haruno->prev != NULL);
    ASSERT_STR_EQ(haruno->prev->contact.lastName, "Учиха");
    ASSERT_STR_EQ(haruno->prev->prev->contact.lastName, "Удзумаки");
    freeContacts(); 
    tests_passed++;
}

void test_get_item_by_index() {
    freeContacts();
    insertItemSorted(createTestItem("Удзумаки", "Наруто"));
    insertItemSorted(createTestItem("Учиха", "Саске"));
    insertItemSorted(createTestItem("Харуно", "Сакура"));
    
    Item *item0 = getItemByIndex(0);
    Item *item1 = getItemByIndex(1);
    Item *item2 = getItemByIndex(2);
    Item *itemOut = getItemByIndex(99);

    ASSERT_TRUE(item0 != NULL); ASSERT_STR_EQ(item0->contact.lastName, "Удзумаки");
    ASSERT_TRUE(item1 != NULL); ASSERT_STR_EQ(item1->contact.lastName, "Учиха");
    ASSERT_TRUE(item2 != NULL); ASSERT_STR_EQ(item2->contact.lastName, "Харуно");
    ASSERT_TRUE(itemOut == NULL);
    freeContacts();
    tests_passed++;
}

void test_remove_head() {
    freeContacts();
    Item *i1 = createTestItem("Удзумаки", "Наруто");
    Item *i2 = createTestItem("Учиха", "Саске");
    insertItemSorted(i1);
    insertItemSorted(i2);
    removeItemFromList(i1);
    free(i1);
    ASSERT_TRUE(head != NULL);
    ASSERT_STR_EQ(head->contact.lastName, "Учиха");
    ASSERT_TRUE(head->prev == NULL);
    freeContacts();
    tests_passed++;
}

void test_remove_middle() {
    freeContacts();
    Item *i1 = createTestItem("Удзумаки", "Наруто");
    Item *i2 = createTestItem("Учиха", "Саске");
    Item *i3 = createTestItem("Харуно", "Сакура");
    insertItemSorted(i1); insertItemSorted(i2); insertItemSorted(i3);
    removeItemFromList(i2);
    free(i2);
    ASSERT_TRUE(i1->next == i3);
    ASSERT_TRUE(i3->prev == i1);
    freeContacts();
    tests_passed++;
}

void test_remove_tail() {
    freeContacts();
    Item *i1 = createTestItem("Удзумаки", "Наруто");
    Item *i2 = createTestItem("Учиха", "Саске");
    insertItemSorted(i1); insertItemSorted(i2);
    removeItemFromList(i2);
    free(i2);
    ASSERT_TRUE(i1->next == NULL);
    freeContacts();
    tests_passed++;
}

void test_free_contacts() {
    freeContacts();
    insertItemSorted(createTestItem("Тест1", "Т"));
    insertItemSorted(createTestItem("Тест2", "Т"));
    freeContacts();
    ASSERT_TRUE(head == NULL);
    tests_passed++;
}

void StartTest() {
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
    
    head = saved_head;
    contactCount = saved_count;
    waitEnter();
}