#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "app.h"
#include "tests.h"

int main() {
    int choice;
    do {
        printMenu();
        choice = validateNumberInput(1, 6);
        switch (choice) {
            case 1: addContact(); break;
            case 2: editContact(); break;
            case 3: deleteContact(); break;
            case 4: displayContacts(); break;
            case 5: StartTest(); break;
            case 6:
                printf("\nВыход из программы...\n");
                freeContacts();
                break;
        }
    } while (choice != 6);
    return 0;
}

void printMenu() {
    printf("\n=== ТЕЛЕФОННАЯ КНИГА ===\n");
    printf("1. Добавить контакт\n");
    printf("2. Редактировать контакт\n");
    printf("3. Удалить контакт\n");
    printf("4. Показать все контакты\n");
    printf("5. Запустить тесты\n");
    printf("6. Выход\n");
    printf("\nВыберите действие: ");
}

void waitEnter() {
    printf("\nНажмите Enter, чтобы продолжить...");
    char temp[10];
    fgets(temp, sizeof(temp), stdin);
}

void getInput(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

int validateNumberInput(int min, int max) {
    int input;
    char buffer[100];
    while (1) {
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Ошибка чтения ввода!\n");
            continue;
        }
        if (sscanf(buffer, "%d", &input) != 1) {
            printf("Ошибка: Введите число от %d до %d: ", min, max);
            continue;
        }
        if (input < min || input > max) {
            printf("Ошибка: Число должно быть от %d до %d: ", min, max);
            continue;
        }
        return input;
    }
}

bool checkContactsExist() {
    if (contactCount == 0) {
        printf("\nСписок контактов пуст.\n");
        return false;
    }
    return true;
}

void addContact() {
    Contact newContact = {0};
    int validLast = 0, validFirst = 0;
    
    printf("\n--- ДОБАВЛЕНИЕ КОНТАКТА ---\n");
    do {
        printf("Введите фамилию: ");
        getInput(newContact.lastName, MAX_LENGTH);
        validLast = strlen(newContact.lastName) > 0;
        if (!validLast) printf("Ошибка: Фамилия обязательна для заполнения!\n");
        
        printf("Введите имя: ");
        getInput(newContact.firstName, MAX_LENGTH);
        validFirst = strlen(newContact.firstName) > 0;
        if (!validFirst) printf("Ошибка: Имя обязательно для заполнения!\n");
    } while (!validLast || !validFirst);

    printf("Место работы (Enter - пропустить): ");
    getInput(newContact.workplace, MAX_LENGTH);
    printf("Должность (Enter - пропустить): ");
    getInput(newContact.job, MAX_LENGTH);
    printf("Телефон (Enter - пропустить): ");
    getInput(newContact.phone, MAX_LENGTH);
    printf("Email (Enter - пропустить): ");
    getInput(newContact.email, MAX_LENGTH);
    printf("Соцсети (Enter - пропустить): ");
    getInput(newContact.social, MAX_LENGTH);
    printf("Мессенджер (Enter - пропустить): ");
    getInput(newContact.messenger, MAX_LENGTH);

    Item *item = (Item*)malloc(sizeof(Item));
    if (!item) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    item->contact = newContact;
    item->prev = NULL;
    item->next = NULL;

    insertItemSorted(item);
    printf("\nКонтакт успешно добавлен!\n");
    waitEnter();
}

void printListOnly() {
    printf("\n--- ВСЕ КОНТАКТЫ (%d) ---\n", contactCount);
    printf("--------------------------------------------------\n");
    printf("| %-3s | %-19s | %-19s |\n", "№", "Фамилия", "Имя");
    printf("--------------------------------------------------\n");
    
    int idx = 1;
    Item *tmp = head;
    while (tmp != NULL) {
        printf("| %-3d | %-19s | %-19s |\n", idx, tmp->contact.lastName, tmp->contact.firstName);
        tmp = tmp->next;
        idx++;
    }
    printf("--------------------------------------------------\n");
}

void displayContacts() {
    if (!checkContactsExist()) {
        waitEnter();
        return;
    }
    printListOnly();
    printf("\nВведите номер контакта для деталей (0 - назад): ");
    int choice = validateNumberInput(0, contactCount);
    if (choice > 0) {
        printContactDetailsByIndex(choice);
        waitEnter();
    }
}

void printContactDetailsByIndex(int index) {
    Item *item = getItemByIndex(index - 1);
    if (!item) return;

    Contact c = item->contact;
    printf("\n=== Контакт #%d =========================\n", index);
    printf("ФИО         : %s %s\n", c.lastName, c.firstName);
    if(strlen(c.workplace) > 0) printf("Место работы: %s\n", c.workplace);
    if(strlen(c.job) > 0)       printf("Должность   : %s\n", c.job);
    if(strlen(c.phone) > 0)     printf("Телефон     : %s\n", c.phone);
    if(strlen(c.email) > 0)     printf("Email       : %s\n", c.email);
    if(strlen(c.social) > 0)    printf("Соцсети     : %s\n", c.social);
    if(strlen(c.messenger) > 0) printf("Мессенджер  : %s\n", c.messenger);
    printf("==========================================\n");
}

void editContact() {
    if (!checkContactsExist()) { waitEnter(); return; }
    printListOnly();
    printf("\nВведите номер контакта для редактирования (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) { printf("Редактирование отменено.\n"); return; }
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    Contact oldContact = current_item->contact;
    Contact *c = &current_item->contact;
    int valid;
    char input[MAX_LENGTH];
    
    do {
        printContactDetailsByIndex(index);
        printf("\nРедактирование поля:\n1. Фамилия\n2. Имя\n3. Место работы\n4. Должность\n5. Телефон\n6. Email\n7. Соцсети\n8. Мессенджер\n9. Сохранить изменения\n0. Отменить изменения\n\nВыберите поле: ");
        int field = validateNumberInput(0, 9);
        
        switch(field) {
            case 1:
                do { printf("Новая фамилия: "); getInput(input, MAX_LENGTH); valid = strlen(input) > 0; if (!valid) printf("Ошибка!\n"); } while (!valid);
                strcpy(c->lastName, input); break;
            case 2:
                do { printf("Новое имя: "); getInput(input, MAX_LENGTH); valid = strlen(input) > 0; if (!valid) printf("Ошибка!\n"); } while (!valid);
                strcpy(c->firstName, input); break;
            case 3: printf("Новое место работы: "); getInput(input, MAX_LENGTH); strcpy(c->workplace, input); break;
            case 4: printf("Новая должность: "); getInput(input, MAX_LENGTH); strcpy(c->job, input); break;
            case 5: printf("Новый телефон: "); getInput(input, MAX_LENGTH); strcpy(c->phone, input); break;
            case 6: printf("Новый email: "); getInput(input, MAX_LENGTH); strcpy(c->email, input); break;
            case 7: printf("Новые соцсети: "); getInput(input, MAX_LENGTH); strcpy(c->social, input); break;
            case 8: printf("Новый мессенджер: "); getInput(input, MAX_LENGTH); strcpy(c->messenger, input); break;
            case 9:
                removeItemFromList(current_item);
                insertItemSorted(current_item);
                printf("\nИзменения сохранены!\n"); waitEnter(); return;
            case 0:
                current_item->contact = oldContact;
                printf("\nИзменения отменены.\n"); waitEnter(); return;
        }
    } while(1);
}

void deleteContact() {
    if (!checkContactsExist()) { waitEnter(); return; }
    printListOnly();
    printf("\nВведите номер контакта для удаления (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) { printf("Удаление отменено.\n"); return; }
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    printf("\nВы уверены, что хотите удалить контакт #%d (%s %s)?\n1. Да\n2. Нет\nВыберите: ", 
           index, current_item->contact.lastName, current_item->contact.firstName);
    int confirm = validateNumberInput(1, 2);
    
    if (confirm == 2) { printf("Удаление отменено.\n"); waitEnter(); return; }
    
    removeItemFromList(current_item);
    free(current_item);
    contactCount--;
    printf("\nКонтакт успешно удален!\n");
    waitEnter();
}