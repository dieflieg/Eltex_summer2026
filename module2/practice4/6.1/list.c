#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Определение глобальных переменных
Item *head = NULL;
int contactCount = 0;

Item* getItemByIndex(int index) {
    Item *tmp = head;
    int i = 0;
    while (tmp != NULL && i < index) {
        tmp = tmp->next;
        i++;
    }
    return tmp;
}

void freeContacts(void) {
    Item *tmp = head; 
    while (tmp != NULL) {
        Item *next = tmp->next;
        free(tmp);
        tmp = next;
    }
    head = NULL;
    contactCount = 0;
}

void removeItemFromList(Item *item) {
    if (item->prev) {
        item->prev->next = item->next;
    } else {
        head = item->next;
    }
    if (item->next) {
        item->next->prev = item->prev;
    }
    item->prev = NULL;
    item->next = NULL;
}

void insertItemSorted(Item *item) {
    Item *tmp = head;
    Item *prev = NULL;
    
    while (tmp != NULL) {
        int cmpLast = strcmp(tmp->contact.lastName, item->contact.lastName);
        if (cmpLast > 0) break;
        if (cmpLast == 0) {
            int cmpFirst = strcmp(tmp->contact.firstName, item->contact.firstName);
            if (cmpFirst > 0) break; 
        }
        prev = tmp;
        tmp = tmp->next;
    }

    if (prev == NULL) { 
        item->next = head;
        item->prev = NULL;
        if (head != NULL) {
            head->prev = item;
        } 
        head = item;
    } else {
        item->prev = prev;
        item->next = prev->next;
        if (prev->next != NULL) {
            prev->next->prev = item;
        }
        prev->next = item;
    }
    contactCount++;
}