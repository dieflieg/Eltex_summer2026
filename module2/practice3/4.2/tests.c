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

extern void init_queue(PriorityQueue* queue);
extern void enqueue(PriorityQueue* queue, const char* data, unsigned char priority);
extern char* dequeue_exact(PriorityQueue* queue, unsigned char priority);
extern char* dequeue_min_priority(PriorityQueue* queue, unsigned char min_priority);
extern char* dequeue_any(PriorityQueue* queue);
extern void free_queue(PriorityQueue* queue);

static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0;

#define ASSERT_STR_EQ(expected, actual, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] %s\n", tests_total, test_name); \
        if (strcmp((expected), (actual)) == 0) { \
            printf("     [PASS] '%s'\n", expected); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Ожидалось: '%s', Получено: '%s'\n", expected, actual); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_NULL(ptr, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] %s\n", tests_total, test_name); \
        if ((ptr) == NULL) { \
            printf("     [PASS] NULL\n"); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Не NULL\n"); \
            tests_failed++; \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] %s\n", tests_total, test_name); \
        if ((ptr) != NULL) { \
            printf("     [PASS] Не NULL\n"); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] NULL\n"); \
            tests_failed++; \
        } \
    } while(0)

// ==========================================================================
// ТЕСТ 1: ИНИЦИАЛИЗАЦИЯ И ОЧИСТКА
// ==========================================================================

// Проверяет, что init_queue создаёт пустую очередь,
// а free_queue корректно освобождает память

static void test_lifecycle() {
    printf("\n=== ТЕСТ 1: Жизненный цикл (init + free) ===\n");
    
    PriorityQueue queue;
    
    printf("1. Инициализируем очередь...\n");
    init_queue(&queue);
    
    printf("   Проверяем, что все списки пусты:\n");
    ASSERT_NULL(queue.heads[0], "heads[0] == NULL");
    ASSERT_NULL(queue.heads[128], "heads[128] == NULL");
    ASSERT_NULL(queue.heads[255], "heads[255] == NULL");
    
    printf("\n2. Добавляем несколько элементов...\n");
    enqueue(&queue, "A", 5);
    enqueue(&queue, "B", 10);
    
    printf("   heads[5]  -> [A] -> NULL\n");
    printf("   heads[10] -> [B] -> NULL\n\n");
    
    ASSERT_NOT_NULL(queue.heads[5], "heads[5] != NULL (добавили A)");
    ASSERT_NOT_NULL(queue.heads[10], "heads[10] != NULL (добавили B)");
    
    printf("3. Очищаем очередь...\n");
    free_queue(&queue);
    
    printf("   Проверяем, что все списки снова пусты:\n");
    ASSERT_NULL(queue.heads[5], "heads[5] == NULL (после free)");
    ASSERT_NULL(queue.heads[10], "heads[10] == NULL (после free)");
}

// ==========================================================================
// ТЕСТ 2: ДОБАВЛЕНИЕ И FIFO ПОРЯДОК
// ==========================================================================

// Проверяет, что внутри одного приоритета элементы
// добавляются в конец списка (FIFO)

static void test_enqueue_fifo() {
    printf("\n=== ТЕСТ 2: Добавление и FIFO порядок ===\n");
    
    PriorityQueue queue;
    init_queue(&queue);
    
    printf("Добавляем 3 элемента с приоритетом 5:\n");
    printf("  1. 'Первое'\n");
    printf("  2. 'Второе'\n");
    printf("  3. 'Третье'\n\n");
    
    enqueue(&queue, "Первое", 5);
    enqueue(&queue, "Второе", 5);
    enqueue(&queue, "Третье", 5);
    
    printf("Ожидаемая структура:\n");
    printf("  heads[5] -> [Первое] -> [Второе] -> [Третье] -> NULL\n\n");
    
    ASSERT_STR_EQ("Первое", queue.heads[5]->data, "1-й элемент");
    ASSERT_STR_EQ("Второе", queue.heads[5]->next->data, "2-й элемент");
    ASSERT_STR_EQ("Третье", queue.heads[5]->next->next->data, "3-й элемент");
    ASSERT_NULL(queue.heads[5]->next->next->next, "4-й элемент == NULL");
    
    free_queue(&queue);
}

// ==========================================================================
// ТЕСТ 3: DEQUEUE_ANY (ИЗВЛЕЧЕНИЕ ПО ПРИОРИТЕТУ)
// ==========================================================================

// Проверяет, что dequeue_any возвращает элементы
// в порядке возрастания приоритета (0 - самый высокий)

static void test_dequeue_any_priority() {
    printf("\n=== ТЕСТ 3: dequeue_any (приоритетное извлечение) ===\n");
    
    PriorityQueue queue;
    init_queue(&queue);
    
    printf("Добавляем элементы с разными приоритетами:\n");
    printf("  enqueue('Prio 100', 100)\n");
    printf("  enqueue('Prio 10', 10)\n");
    printf("  enqueue('Prio 50', 50)\n");
    printf("  enqueue('Prio 1', 1)\n");
    printf("  enqueue('Prio 200', 200)\n\n");
    
    enqueue(&queue, "Prio 100", 100);
    enqueue(&queue, "Prio 10", 10);
    enqueue(&queue, "Prio 50", 50);
    enqueue(&queue, "Prio 1", 1);
    enqueue(&queue, "Prio 200", 200);
    
    printf("Ожидаемый порядок извлечения (по приоритету):\n");
    printf("  1. Prio 1\n");
    printf("  2. Prio 10\n");
    printf("  3. Prio 50\n");
    printf("  4. Prio 100\n");
    printf("  5. Prio 200\n\n");
    
    char* msg;
    
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 1", msg, "1-й извлечён"); free(msg); }
    
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 10", msg, "2-й извлечён"); free(msg); }
    
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 50", msg, "3-й извлечён"); free(msg); }
    
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 100", msg, "4-й извлечён"); free(msg); }
    
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 200", msg, "5-й извлечён"); free(msg); }
    
    msg = dequeue_any(&queue);
    ASSERT_NULL(msg, "6-й извлечён == NULL (очередь пуста)");
    
    free_queue(&queue);
}

// ==========================================================================
// ТЕСТ 4: DEQUEUE_EXACT И DEQUEUE_MIN_PRIORITY
// ==========================================================================

// Проверяет извлечение с точным приоритетом
// и извлечение с приоритетом >= заданного

static void test_special_dequeue() {
    printf("\n=== ТЕСТ 4: Специальные извлечения ===\n");
    
    PriorityQueue queue;
    init_queue(&queue);
    
    printf("Добавляем элементы:\n");
    printf("  enqueue('Prio 5-A', 5)\n");
    printf("  enqueue('Prio 5-B', 5)\n");
    printf("  enqueue('Prio 20', 20)\n");
    printf("  enqueue('Prio 50', 50)\n\n");
    
    enqueue(&queue, "Prio 5-A", 5);
    enqueue(&queue, "Prio 5-B", 5);
    enqueue(&queue, "Prio 20", 20);
    enqueue(&queue, "Prio 50", 50);
    
    printf("=== Часть A: dequeue_exact (точный приоритет) ===\n");
    printf("Извлекаем с приоритетом 5 (должны получить A, потом B):\n\n");
    
    char* msg1 = dequeue_exact(&queue, 5);
    if (msg1) { ASSERT_STR_EQ("Prio 5-A", msg1, "exact(5) -> A"); free(msg1); }
    
    char* msg2 = dequeue_exact(&queue, 5);
    if (msg2) { ASSERT_STR_EQ("Prio 5-B", msg2, "exact(5) -> B"); free(msg2); }
    
    char* msg3 = dequeue_exact(&queue, 5);
    ASSERT_NULL(msg3, "exact(5) -> NULL (больше нет)");
    
    printf("\n=== Часть B: dequeue_min_priority (>= заданного) ===\n");
    printf("Извлекаем с приоритетом >= 15:\n");
    printf("  Ищем: 15, 16, ..., 20 (нашли!)\n\n");
    
    char* msg4 = dequeue_min_priority(&queue, 15);
    if (msg4) { ASSERT_STR_EQ("Prio 20", msg4, "min_priority(15) -> 20"); free(msg4); }
    
    printf("\nИзвлекаем с приоритетом >= 100:\n");
    printf("  Ищем: 100, 101, ..., 255 (все пустые)\n\n");
    
    char* msg5 = dequeue_min_priority(&queue, 100);
    ASSERT_NULL(msg5, "min_priority(100) -> NULL");
    
    free_queue(&queue);
}

// ==========================================================================
// ТЕСТ 5: ИЗВЛЕЧЕНИЕ ИЗ ПУСТОЙ ОЧЕРЕДИ
// ==========================================================================

// Проверяет, что все функции извлечения корректно
// обрабатывают пустую очередь и возвращают NULL

static void test_empty_queue() {
    printf("\n=== ТЕСТ 5: Извлечение из пустой очереди ===\n");
    
    PriorityQueue queue;
    init_queue(&queue);
    
    printf("Очередь пуста (все heads[i] = NULL)\n\n");
    
    char* msg1 = dequeue_any(&queue);
    ASSERT_NULL(msg1, "dequeue_any -> NULL");
    
    char* msg2 = dequeue_exact(&queue, 5);
    ASSERT_NULL(msg2, "dequeue_exact(5) -> NULL");
    
    char* msg3 = dequeue_min_priority(&queue, 0);
    ASSERT_NULL(msg3, "dequeue_min_priority(0) -> NULL");
    
    printf("\nОчередь всё ещё пуста (ничего не сломалось):\n");
    ASSERT_NULL(queue.heads[0], "heads[0] == NULL");
    ASSERT_NULL(queue.heads[255], "heads[255] == NULL");
    
    free_queue(&queue);
}

// ==========================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// ==========================================================================

void run_all_tests(void) {
    printf("\n");
    printf("==================================================\n");
    printf("              ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("==================================================\n");
    
    tests_passed = 0;
    tests_failed = 0;
    tests_total = 0;

    test_lifecycle();
    test_enqueue_fifo();
    test_dequeue_any_priority();
    test_special_dequeue();
    test_empty_queue();

    printf("\n==================================================\n");
    printf("                    РЕЗУЛЬТАТЫ\n");
    printf("==================================================\n");
    printf("Всего: %d | Пройдено: %d | Провалено: %d\n", 
           tests_total, tests_passed, tests_failed);
    printf("==================================================\n");
    
    if (tests_failed > 0) {
        printf("СТАТУС: [FAIL]\n");
    } else {
        printf("СТАТУС: [PASS]\n");
    }
    printf("==================================================\n\n");
}