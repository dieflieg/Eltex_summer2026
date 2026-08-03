#ifndef TESTS_H
#define TESTS_H
#define MAX_LENGTH 100

// Структура для хранения информации о контакте
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

// Узел двусвязного списка, содержащий контакт
typedef struct Item {
    Contact contact;
    struct Item *prev;
    struct Item *next;
} Item;


// Прототипы функций из main
void printMenu();
void printListOnly();
void printContactDetailsByIndex(int index);
void getInput(char *buffer, int size);
int validateNumberInput(int min, int max);
void addContact();
void displayContacts();
void editContact();
void deleteContact();
Item* getItemByIndex(int index);
void freeContacts();
bool checkContactsExist();
void removeItemFromList(Item *item);
void insertItemSorted(Item *item);
void waitEnter();


// Прототипы функций тестов
void StartTest();
void test_free_contacts();
void test_remove_tail();
void test_remove_middle();
void test_remove_head();
void test_get_item_by_index();
void test_sorted_insertion();
void test_insert_into_empty_list();
void waitEnter();


#endif