#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tests.h"

#define MAX_PRIORITY 256

typedef struct Node {
    char data[256];
    struct Node* next;
} Node;

typedef struct {
    Node* heads[MAX_PRIORITY];
} PriorityQueue;

void init_queue(PriorityQueue* queue) {
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        queue->heads[i] = NULL;
    }
}

void enqueue(PriorityQueue* queue, const char* data, unsigned char priority) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    strncpy(new_node->data, data, 255);
    new_node->data[255] = '\0';
    new_node->next = NULL;

    Node** head = &queue->heads[priority];
    if (*head == NULL) {
        *head = new_node;
    } else {
        Node* current = *head;
        while (current->next) current = current->next;
        current->next = new_node;
    }
}

char* dequeue_exact(PriorityQueue* queue, unsigned char priority) {
    Node** head = &queue->heads[priority];
    if (*head == NULL) return NULL;

    Node* temp = *head;
    char* result = strdup(temp->data);
    *head = temp->next;
    free(temp);
    return result;
}

char* dequeue_min_priority(PriorityQueue* queue, unsigned char min_priority) {
    for (int i = min_priority; i < MAX_PRIORITY; ++i) {
        char* result = dequeue_exact(queue, i);
        if (result) return result;
    }
    return NULL;
}

char* dequeue_any(PriorityQueue* queue) {
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        char* result = dequeue_exact(queue, i);
        if (result) return result;
    }
    return NULL;
}

void free_queue(PriorityQueue* queue) {
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        Node* current = queue->heads[i];
        while (current) {
            Node* next = current->next;
            free(current);
            current = next;
        }
        queue->heads[i] = NULL;
    }
}

void run_demo() {
    PriorityQueue queue;
    init_queue(&queue);

    enqueue(&queue, "Сообщение A (prio 10)", 10);
    enqueue(&queue, "Сообщение B (prio 5)", 5);
    enqueue(&queue, "Сообщение C (prio 250)", 250);
    enqueue(&queue, "Сообщение D (prio 5)", 5);
    enqueue(&queue, "Сообщение E (prio 200)", 200);

    printf("Извлечение любого элемента:\n");
    char* msg = dequeue_any(&queue);
    if (msg) { printf("-> %s\n", msg); free(msg); }

    printf("Извлечение с приоритетом >= 100:\n");
    while ((msg = dequeue_min_priority(&queue, 100)) != NULL) {
        printf("-> %s\n", msg);
        free(msg);
    }

    printf("Извлечение с приоритетом 5:\n");
    while ((msg = dequeue_exact(&queue, 5)) != NULL) {
        printf("-> %s\n", msg);
        free(msg);
    }

    printf("Очистка оставшихся элементов...\n");
    free_queue(&queue);
}

int main() {
    int choice;
    
    do {
        printf("\n1. Демонстрация работы очереди\n");
        printf("2. Запустить юнит-тесты\n");
        printf("0. Выход\n");
        printf("Выберите действие: ");
        
        if (scanf("%d", &choice) != 1) {
            char temp[100];
            fgets(temp, sizeof(temp), stdin);
            printf("Ошибка: введите число!\n");
            continue;
        }
        
        char temp[100];
        fgets(temp, sizeof(temp), stdin);

        switch(choice) {
            case 1:
                run_demo();
                break;
            case 2:
                run_all_tests();
                break;
            case 0:
                printf("Выход из программы...\n");
                break;
            default:
                printf("Неверный выбор!\n");
        }
    } while (choice != 0);
    
    return 0;
}