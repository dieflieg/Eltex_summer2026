#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tests.h"

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

// Структура узла бинарного дерева
typedef struct Item {
    Contact contact;
    struct Item *left, *right;
} Item;

// Глобальные переменные
Item *root = NULL; // Корень дерева (вместо head)
int contactCount = 0;
int operationsSinceBalance = 0; // Счетчик операций для периодической балансировки

// Прототипы функций
void printMenu();
void waitEnter();
void getInput(char *buffer, int size);
int validateNumberInput(int min, int max);
void addContact();
void editContact();
void deleteContact();
void displayContacts();
void freeContacts();
bool checkContactsExist();
Item* getItemByIndex(int index);

// Новые функции для дерева
int compareContacts(Contact *a, Contact *b);
void insertNode(Contact val, Item **q);
int deleteNode(Contact key, Item **node);
void printInOrder(Item *p, int *idx);
void flattenToArray(Item *p, Item **arr, int *currentIndex);
Item* buildBalanced(Item **arr, int start, int end);
void balanceTree();
void freeTree(Item *p);

int main() {
    int choice;
    do {
        printMenu();
        choice = validateNumberInput(1, 7);
        switch (choice) {
            case 1: addContact(); break;
            case 2: editContact(); break;
            case 3: deleteContact(); break;
            case 4: displayContacts(); break;
            case 5: StartTest(); waitEnter(); break;
            case 6: balanceTree(); waitEnter(); break; // Ручная балансировка
            case 7:
                printf("\nВыход из программы...\n");
                freeContacts();
                break;
        }
    } while (choice != 7);
    return 0;
}

void printMenu() {
    printf("\n=== ТЕЛЕФОННАЯ КНИГА (БИНАРНОЕ ДЕРЕВО) ===\n");
    printf("1. Добавить контакт\n");
    printf("2. Редактировать контакт\n");
    printf("3. Удалить контакт\n");
    printf("4. Показать все контакты\n");
    printf("5. Запустить тесты\n");
    printf("6. Выполнить балансировку дерева\n");
    printf("7. Выход\n");
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

// Сравнение двух контактов для определения направления в дереве (лексикографически)
int compareContacts(Contact *a, Contact *b) {
    int cmpLast = strcmp(a->lastName, b->lastName);
    if (cmpLast != 0) return cmpLast;
    return strcmp(a->firstName, b->firstName);
}

// -----------------------------------------------------------------------------
// АЛГОРИТМЫ БИНАРНОГО ДЕРЕВА ПОИСКА (на основе ваших фрагментов)
// -----------------------------------------------------------------------------

// Вставка элемента в дерево поиска
void insertNode(Contact val, Item **q) {
    if(*q == NULL) {
        *q = malloc(sizeof(Item));
        (*q)->left = (*q)->right = NULL;
        (*q)->contact = val;
        return;
    }
    // Если текущий узел лексикографически больше нового, идем влево
    if(compareContacts(&(*q)->contact, &val) > 0)
        insertNode(val, &(*q)->left);
    else
        insertNode(val, &(*q)->right);
}

// Удаление узла из дерева
int deleteNode(Contact key, Item **node) {
    Item *t, *up;
    if(*node == NULL) return 0; // Нет такого значения в дереве

    int cmp = compareContacts(&(*node)->contact, &key);
    
    if(cmp == 0) { // Нашли нужный узел
        // 1. Если значение находится в листе, то удаляем лист
        if(((*node)->left == NULL) && ((*node)->right == NULL)) {
            free(*node);
            *node = NULL;
            return 1;
        }
        // 2. Если у вершины только правый потомок
        if((*node)->left == NULL) {
            t = *node;
            *node = (*node)->right;
            free(t);
            return 1;
        }
        // 3. Если у вершины только левый потомок
        if((*node)->right == NULL) {
            t = *node;
            *node = (*node)->left;
            free(t);
            return 1;
        }
        // 4. Если у вершины два потомка
        up = *node;
        t = (*node)->left; // Идем в левое поддерево
        
        // Спускаемся до крайнего правого узла
        while (t->right != NULL) {
            up = t;
            t = t->right;
        }
        
        // Копируем данные из правого узла вместо удаляемого значения
        (*node)->contact = t->contact;
        
        // Если ниже удаляемого больше, чем одна вершина
        if (up != (*node)) {
            if(t->left != NULL) up->right = t->left;
            else up->right = NULL; // Удаляем лист
        }
        // Если ниже удаляемого одна вершина
        else {
            (*node)->left = t->left;
        }
        
        free(t);
        return 1;
    }
    
    // Поиск ключа в левом или правом поддереве
    if(cmp > 0)
        return deleteNode(key, &(*node)->left);
    return deleteNode(key, &(*node)->right);
}

// Обход дерева для заполнения массива (нужно для балансировки и индексации)
void flattenToArray(Item *p, Item **arr, int *currentIndex) {
    if (p == NULL) return;
    flattenToArray(p->left, arr, currentIndex);
    arr[*currentIndex] = p;
    (*currentIndex)++;
    flattenToArray(p->right, arr, currentIndex);
}

// Рекурсивное построение идеально сбалансированного дерева из массива
Item* buildBalanced(Item **arr, int start, int end) {
    if (start > end) return NULL;
    int mid = (start + end) / 2;
    Item *node = arr[mid];
    node->left = buildBalanced(arr, start, mid - 1);
    node->right = buildBalanced(arr, mid + 1, end);
    return node;
}

// Периодическая балансировка
void balanceTree() {
    if (contactCount == 0) return;
    
    Item **arr = malloc(contactCount * sizeof(Item*));
    int current = 0;
    flattenToArray(root, arr, &current);
    
    // Перестраиваем связи для создания сбалансированного дерева
    root = buildBalanced(arr, 0, contactCount - 1);
    free(arr);
    
    operationsSinceBalance = 0;
    printf("\n[Система]: Дерево успешно сбалансировано.\n");
}

// Освобождение памяти всего дерева
void freeTree(Item *p) {
    if (p == NULL) return;
    freeTree(p->left);
    freeTree(p->right);
    free(p);
}

void freeContacts() {
    freeTree(root);
    root = NULL;
    contactCount = 0;
}

// -----------------------------------------------------------------------------
// ИНТЕРФЕЙС И ЛОГИКА ТЕЛЕФОННОЙ КНИГИ
// -----------------------------------------------------------------------------

bool checkContactsExist() {
    if (contactCount == 0) {
        printf("\nСписок контактов пуст.\n");
        return false;
    }
    return true;
}

// Получение контакта по визуальному индексу (1..N) с помощью In-Order обхода
Item* getItemByIndex(int index) {
    if (index < 0 || index >= contactCount) return NULL;
    
    Item **arr = malloc(contactCount * sizeof(Item*));
    int current = 0;
    flattenToArray(root, arr, &current);
    
    Item *res = arr[index];
    free(arr);
    return res;
}

void addContact() {
    Contact newContact = {0};
    int validLast = 0, validFirst = 0;
    
    printf("\n--- ДОБАВЛЕНИЕ КОНТАКТА ---\n");
    do {
        printf("Введите фамилию: ");
        getInput(newContact.lastName, MAX_LENGTH);
        validLast = strlen(newContact.lastName) > 0;
        if (!validLast) printf("Ошибка: Фамилия обязательна!\n");
        
        printf("Введите имя: ");
        getInput(newContact.firstName, MAX_LENGTH);
        validFirst = strlen(newContact.firstName) > 0;
        if (!validFirst) printf("Ошибка: Имя обязательно!\n");
    } while (!validLast || !validFirst);

    printf("Место работы (Enter - пропустить): "); getInput(newContact.workplace, MAX_LENGTH);
    printf("Должность (Enter - пропустить): "); getInput(newContact.job, MAX_LENGTH);
    printf("Телефон (Enter - пропустить): "); getInput(newContact.phone, MAX_LENGTH);
    printf("Email (Enter - пропустить): "); getInput(newContact.email, MAX_LENGTH);
    printf("Соцсети (Enter - пропустить): "); getInput(newContact.social, MAX_LENGTH);
    printf("Мессенджер (Enter - пропустить): "); getInput(newContact.messenger, MAX_LENGTH);

    insertNode(newContact, &root);
    contactCount++;
    operationsSinceBalance++;

    printf("\nКонтакт успешно добавлен!\n");
    
    // Периодическая балансировка после 5 операций
    if (operationsSinceBalance >= 5) balanceTree();
    
    waitEnter();
}

// Симметричный обход для вывода таблицы
void printInOrder(Item *p, int *idx) {
    if (p == NULL) return;
    printInOrder(p->left, idx);
    printf("| %-3d | %-19s | %-19s |\n", *idx, p->contact.lastName, p->contact.firstName);
    (*idx)++;
    printInOrder(p->right, idx);
}

void printListOnly() {
    printf("\n--- ВСЕ КОНТАКТЫ (%d) ---\n", contactCount);
    printf("--------------------------------------------------\n");
    printf("| %-3s | %-19s | %-19s |\n", "№", "Фамилия", "Имя");
    printf("--------------------------------------------------\n");
    int idx = 1;
    printInOrder(root, &idx);
    printf("--------------------------------------------------\n");
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

void displayContacts() {
    if (!checkContactsExist()) return;
    printListOnly();
    
    printf("\nВведите номер контакта для деталей (0 - назад): ");
    int choice = validateNumberInput(0, contactCount);
    if (choice > 0) {
        printContactDetailsByIndex(choice);
        waitEnter();
    }
}

void editContact() { 
    if (!checkContactsExist()) return;
    
    printListOnly();
    printf("\nВведите номер контакта для редактирования (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) return;
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    Contact oldContact = current_item->contact;
    Contact tempContact = current_item->contact; // Работаем с копией, чтобы не сломать BST
    
    char input[MAX_LENGTH];
    do {
        printContactDetailsByIndex(index);
        printf("\nРедактирование поля:\n");
        printf("1. Фамилия\n2. Имя\n3. Место работы\n4. Должность\n");
        printf("5. Телефон\n6. Email\n7. Соцсети\n8. Мессенджер\n");
        printf("9. Сохранить изменения\n0. Отменить изменения\n");
        printf("\nВыберите поле для редактирования: ");
        
        int field = validateNumberInput(0, 9);
        switch(field) {
            case 1:
                do {
                    printf("Новая фамилия: "); getInput(input, MAX_LENGTH);
                } while (strlen(input) == 0);
                strcpy(tempContact.lastName, input);
                break;
            case 2:
                do {
                    printf("Новое имя: "); getInput(input, MAX_LENGTH);
                } while (strlen(input) == 0);
                strcpy(tempContact.firstName, input);
                break;
            case 3: printf("Новое место: "); getInput(tempContact.workplace, MAX_LENGTH); break;
            case 4: printf("Новая должн: "); getInput(tempContact.job, MAX_LENGTH); break;
            case 5: printf("Новый телефон: "); getInput(tempContact.phone, MAX_LENGTH); break;
            case 6: printf("Новый email: "); getInput(tempContact.email, MAX_LENGTH); break;
            case 7: printf("Новые соцсети: "); getInput(tempContact.social, MAX_LENGTH); break;
            case 8: printf("Новый мессенджер: "); getInput(tempContact.messenger, MAX_LENGTH); break;
            case 9:
                // Важно! В дереве поиска нельзя менять ключ "на месте".
                // Нужно удалить старый узел и вставить новый с обновленными данными.
                deleteNode(oldContact, &root);
                insertNode(tempContact, &root);
                printf("\nИзменения сохранены!\n");
                waitEnter();
                return;
            case 0:
                printf("\nИзменения отменены.\n");
                waitEnter();
                return;
        }
    } while(1);
}

void deleteContact() {
    if (!checkContactsExist()) return;
    
    printListOnly();
    printf("\nВведите номер контакта для удаления (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) return;
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    printf("\nВы уверены, что хотите удалить контакт #%d (%s %s)?\n", 
           index, current_item->contact.lastName, current_item->contact.firstName);
    printf("1. Да, удалить\n2. Нет, отменить\nВыберите действие: ");
    
    if (validateNumberInput(1, 2) == 2) {
        printf("Удаление отменено.\n");
        waitEnter();
        return;
    }
    
    // Удаляем узел по значению ключа (Фамилия + Имя)
    deleteNode(current_item->contact, &root);
    contactCount--;
    operationsSinceBalance++;
    
    printf("\nКонтакт успешно удален!\n");
    
    // Периодическая балансировка
    if (operationsSinceBalance >= 5) balanceTree();
    
    waitEnter();
}