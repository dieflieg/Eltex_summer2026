#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tests.h"

#define MAX_LENGTH 100

// Глобальные переменные
Item *head = NULL;
int contactCount = 0;

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

// Функция для вывода главного меню
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

// Создание паузы для ожидания нажатия Enter
void waitEnter() {
    printf("\nНажмите Enter, чтобы продолжить...");
    char temp[10];
    fgets(temp, sizeof(temp), stdin);
}

// Безопасное чтение строки с клавиатуры
void getInput(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// Защищенный ввод числа с проверкой диапазона
int validateNumberInput(int min, int max) {
    int input;
    char buffer[100]; // Временный текстовый буфер для считывания ввода
    while (1) {
        // Читаем весь ввод пользователя как обычный текст
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Ошибка чтения ввода!\n");
            continue;
        }

        // Пытаемся вытащить число из считанного текста.
        // Если sscanf не смог найти число, возвращаем ошибку
        if (sscanf(buffer, "%d", &input) != 1) {
            printf("Ошибка: Введите число от %d до %d: ", min, max);
            continue;
        }
        // Проверяем, попадает ли введенное число в заданный диапазон
        if (input < min || input > max) {
            printf("Ошибка: Число должно быть от %d до %d: ", min, max);
            continue;
        }
        return input;
    }
}

// Поиск элемента по индексу
Item* getItemByIndex(int index) {
    Item *tmp = head;
    int i = 0;
    // Шагаем по списку до тех пор, пока узел существует 
    // и мы не дошли до нужного индекса
    while (tmp != NULL && i < index) {
        tmp = tmp->next;
        i++;
    }
    return tmp;
}

// Полное освобождение памяти списка
void freeContacts() {
    Item *tmp = head; 
    while (tmp != NULL) {
        Item *next = tmp->next; // Сохраняем адрес следующего узла, 
                                // иначе после free мы его потеряем
        free(tmp); // Освобождаем динамическую память текущего узла
        tmp = next; // Переход к следующему узлу, который мы заранее сохранили
    }
    head = NULL;
}


// Быстрая проверка на то, не пуста ли телефонная книга
// Используется перед выводом, удалением и редактированием и списка
bool checkContactsExist() {
    if (contactCount == 0) {
        printf("\nСписок контактов пуст.\n");
        return false;
    }
    return true;
}

// Удаление элемента из списка
void removeItemFromList(Item *item) {
    if (item->prev) {       // Если левый сосед есть, связываем его next 
        item->prev->next = item->next; // напрямую с правым соседом item
    } else {
        head = item->next; // Если левый сосед отсутствует, значит item был головой 
                           // списка, поэтому обновляем head на следующий элемент
    }
    if (item->next) {      // Если правый сосед есть, связываем его prev
        item->next->prev = item->prev; // напрямую с левым соседом item
    }
    // Обнуляем указатели удаляемого элемента
    item->prev = NULL;
    item->next = NULL;
}

// Вставка с сохранением алфавитного порядка
void insertItemSorted(Item *item) {
    Item *tmp = head; // Вспомогательный указатель, смотрит на узел, перед которым,
                      // возможно, придется вставить item.
    Item *prev = NULL;
    
    // Перебираем список, пока не найдем элемент, который больше нового (item), 
    // или не дойдем до конца списка
    while (tmp != NULL) {
        int cmpLast = strcmp(tmp->contact.lastName, item->contact.lastName);
        if (cmpLast > 0) break; // Перешагнули нужное место
        if (cmpLast == 0) { // Фамилии одинаковые, сравниваем имена
            int cmpFirst = strcmp(tmp->contact.firstName, item->contact.firstName);
            if (cmpFirst > 0) break; 
        }
        // Перемещение указателей для следующей итерации
        prev = tmp; // Запоминаем текущий узел
        tmp = tmp->next; // Переходим к следующему узлу
    }

    // По завершении цикла, prev указывает на узел, после которого нужно вставить item,
    // а tmp указывает на узел, перед которым нужно вставить item


    // Вставка item в начало списка
    if (prev == NULL) { 
        item->next = head; // 1. Новый элемент указывает своим next на старую голову
        item->prev = NULL; // 2. У нового элемента нет никого слева (он первый)
       
        if (head != NULL) {
            head->prev = item; // 3. Если старая голова существовала, ее prev теперь указывает на item
        } 

        head = item; // 4. Голова теперь указывает на новый элемент

    // Вставка в середину или в конец списка    
    } else {
        item->prev = prev; // 1. Указываем, что слева от item стоит prev
        item->next = prev->next; // 2. Указываем, что справа от item стоит тот элемент, 
                                 //    который раньше стоял справа от prev (т.е. tmp)

        if (prev->next != NULL) {
            prev->next->prev = item; // 3. Если справа был элемент (вставка в середину),
                                     //    заставляем его смотреть левой стрелочкой (prev) на item
        }
        prev->next = item; // 4. Заставляем левый узел (prev) смотреть правой стрелочкой (next) на item
    }
}


// Функция добавления нового контакта. Запрашивает данные, 
// создает новый узел в памяти и передает его в функцию сортировки
void addContact() { // Создаем пустой контакт (заполнен нулями)
    Contact newContact = {0};
    int validLast = 0, validFirst = 0;
    
    printf("\n--- ДОБАВЛЕНИЕ КОНТАКТА ---\n");

    // Обязательные поля запрашиваем в цикле, пока пользователь их не введет 
    do {
        printf("Введите фамилию: ");
        getInput(newContact.lastName, MAX_LENGTH);
        validLast = strlen(newContact.lastName) > 0; // Проверка на пустоту
        if (!validLast) printf("Ошибка: Фамилия обязательна для заполнения!\n");
        
        printf("Введите имя: ");
        getInput(newContact.firstName, MAX_LENGTH);
        validFirst = strlen(newContact.firstName) > 0; // Проверка на пустоту
        if (!validFirst) printf("Ошибка: Имя обязательно для заполнения!\n");
    } while (!validLast || !validFirst);

    // Дополнительные поля (опционально)
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

    // Выделяем динамическую память под новый узел
    Item *item = (Item*)malloc(sizeof(Item));
    if (!item) {
        printf("Ошибка выделения памяти!\n");
        return;
    }

    // Копируем собранные данные в структуру узла и обнуляем связи
    item->contact = newContact;
    item->prev = NULL;
    item->next = NULL;

    // Вставка нового узла, отсортированного лексикографически
    insertItemSorted(item);
    contactCount++; // Увеличиваем счетчик контактов

    printf("\nКонтакт успешно добавлен!\n");
    waitEnter();
}


// Функция вывода списка контактов (только имена и фамилии с нумерацией)
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

// Функция для расширенного просмотра деталей контакта по его индексу,
// запрашивает у пользователя номер контакта и выводит все его поля
void displayContacts() {
    if (!checkContactsExist()) {
        waitEnter();
        return;
    }

    printListOnly();
    
    printf("\nВведите номер контакта для деталей (0 - назад): ");
    int choice = validateNumberInput(0, contactCount); // Пользователь выбирает номер
    
    if (choice > 0) { // Если не 0, показываем подробную информацию
        printContactDetailsByIndex(choice);
        waitEnter();
    }
}


// Расширенный вывод деталей контакта по его индексу
void printContactDetailsByIndex(int index) {

    // Получаем узел (нумерация index начинается с 1, а в списке с 0)
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

// Позволяет изменять поля существующего контакта
void editContact() { 
    if (!checkContactsExist()) {
        waitEnter();
        return;
    }
    
    printListOnly();
    
    printf("\nВведите номер контакта для редактирования (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) {
        printf("Редактирование отменено.\n");
        return;
    }
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    // Делаем резервную копию данных контакта
    // Если пользователь захочет отменить изменения, вернем эту копию назад
    Contact oldContact = current_item->contact;
    Contact *c = &current_item->contact;
    
    int valid;
    char input[MAX_LENGTH];
    do {
        printContactDetailsByIndex(index);
        printf("\nРедактирование поля:\n");
        printf("1. Фамилия\n");
        printf("2. Имя\n");
        printf("3. Место работы\n");
        printf("4. Должность\n");
        printf("5. Телефон\n");
        printf("6. Email\n");
        printf("7. Соцсети\n");
        printf("8. Мессенджер\n");
        printf("9. Сохранить изменения\n");
        printf("0. Отменить изменения\n");
        printf("\nВыберите поле для редактирования: ");
        int field = validateNumberInput(0, 9);
        
        switch(field) {
            case 1:
                do {
                    printf("Новая фамилия: ");
                    getInput(input, MAX_LENGTH);
                    valid = strlen(input) > 0;
                    if (!valid) printf("Ошибка: Фамилия не может быть пустой!\n");
                } while (!valid);
                strcpy(c->lastName, input);
                break;
            case 2:
                do {
                    printf("Новое имя: ");
                    getInput(input, MAX_LENGTH);
                    valid = strlen(input) > 0;
                    if (!valid) printf("Ошибка: Имя не может быть пустым!\n");
                } while (!valid);
                strcpy(c->firstName, input);
                break;
            case 3:
                printf("Новое место работы: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->workplace, input);
                break;
            case 4:
                printf("Новая должность: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->job, input);
                break;
            case 5:
                printf("Новый телефон: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->phone, input);
                break;
            case 6:
                printf("Новый email: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->email, input);
                break;
            case 7:
                printf("Новые соцсети: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->social, input);
                break;
            case 8:
                printf("Новый мессенджер: ");
                getInput(input, MAX_LENGTH);
                strcpy(c->messenger, input);
                break;
            case 9:
                // Если изменили имя или фамилию - обновляем позицию в отсортированном списке
                removeItemFromList(current_item); // отвязываем элемент от списка
                insertItemSorted(current_item); // и вставляем его заново
                printf("\nИзменения сохранены!\n");
                waitEnter();
                return;
            case 0:
                current_item->contact = oldContact; // Возвращаем старые сохраненные данные и выходим
                printf("\nИзменения отменены.\n");
                waitEnter();
                return;
        }
    } while(1);
}

// Удаление контакта по индексу
void deleteContact() {
    if (!checkContactsExist()) {
        waitEnter();
        return;
    }
    
    printListOnly();
    
    printf("\nВведите номер контакта для удаления (0 - отмена): ");
    int index = validateNumberInput(0, contactCount);
    if (index == 0) {
        printf("Удаление отменено.\n");
        return;
    }
    
    Item *current_item = getItemByIndex(index - 1);
    if (!current_item) return;
    
    printf("\nВы уверены, что хотите удалить контакт #%d (%s %s)?\n", 
           index, current_item->contact.lastName, current_item->contact.firstName);
    printf("1. Да, удалить\n");
    printf("2. Нет, отменить\n");
    printf("Выберите действие: ");
    int confirm = validateNumberInput(1, 2);
    
    if (confirm == 2) {
        printf("Удаление отменено.\n");
        waitEnter();
        return;
    }
    
    removeItemFromList(current_item);
    free(current_item);
    contactCount--;
    
    printf("\nКонтакт успешно удален!\n");
    waitEnter();
}