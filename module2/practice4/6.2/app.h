#ifndef APP_H
#define APP_H

#include "list.h" 

void printMenu(void);
void waitEnter(void);
void getInput(char *buffer, int size);
int validateNumberInput(int min, int max);
bool checkContactsExist(void);
void addContact(void);
void displayContacts(void);
void printListOnly(void);
void printContactDetailsByIndex(int index);
void editContact(void);
void deleteContact(void);


#endif // APP_H