#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

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
    struct Item *prev;
    struct Item *next;
} Item;

// Глобальные переменные состояния списка (объявлены как extern, определены в list.c)
extern Item *head;
extern int contactCount;

// Прототипы функций управления списком (реализованы в list.c)
void insertItemSorted(Item *item);
void removeItemFromList(Item *item);
Item* getItemByIndex(int index);
void freeContacts(void);

#endif // LIST_H