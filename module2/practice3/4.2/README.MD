## 4.2 Реализовать очередь с приоритетом. Приоритет задается целым числом от 0 до 255. Реализовать функции для работы с очередью: 1) добавление элемента и 2) извлечение элемента: находящегося первым в очереди/с указанным приоритетом/с приоритетом не ниже чем заданный. В тестирующей программе имитировать генерацию сообщений с различным приоритетом и выборку данных с различными условиями.


## **Структуры:**

```c

// Узел односвязного списка, хранит сообщение и указатель на следующий элемент
typedef struct Node {
    char data[256];  // Данные сообщения (до 255 символов + '\0')
    struct Node* next;
} Node;

// Очередь с приоритетом - представляет собой массив из 256 односвязных списков
// Каждый список хранит сообщения с определённым приоритетом
typedef struct {
    Node* heads[MAX_PRIORITY];
} PriorityQueue;
```

---

## **Основная программа ```main.c```:**

```c
// Максимальный приоритет (0-255)
// 0 - самый высокий приоритет, 255 - самый низкий
#define MAX_PRIORITY 256

// Узел односвязного списка
// Каждый узел хранит сообщение и указатель на следующий элемент
typedef struct Node {
    char data[256];         // Данные сообщения (до 255 символов + '\0')
    struct Node* next;      // Указатель на следующий элемент в списке
} Node;

// Очередь с приоритетом
// Представляет собой массив из 256 односвязных списков
// Каждый список хранит сообщения с определённым приоритетом
typedef struct {
    Node* heads[MAX_PRIORITY]; // Массив из 256 указателей
    // heads[0] - начало списка сообщений с приоритетом 0 (самый высокий)
    // heads[1] - начало списка сообщений с приоритетом 1
    // ...
    // heads[255] - начало списка сообщений с приоритетом 255 (самый низкий)
} PriorityQueue;


void init_queue(PriorityQueue* queue) {
    // Проходимся по всем 256 приоритетам
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        queue->heads[i] = NULL; // Устанавливаем каждый указатель в NULL
    }
}

```

### **Алгоритм добавления нового узла:**
1. Выделяем память под новый узел
2. Копируем данные сообщения в узел
3. Ищем список для заданного приоритета (heads[priority])
4. Если список пуст - новый узел становится головой
5. Если список не пуст - идём в конец и добавляем новый узел

```c
void enqueue(PriorityQueue* queue, const char* data, unsigned char priority) {
    // Выделяем память под новый узел
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    
    // Копируем данные сообщения в узел
    // strncpy копирует до 255 символов, чтобы не вылезти за границы массива
    strncpy(new_node->data, data, 255);
    new_node->data[255] = '\0'; // Гарантируем завершение строки
    new_node->next = NULL;      // Новый узел пока ни на что не указывает

    // Получаем указатель на голову списка для заданного приоритета
    // Используем двойной указатель, чтобы можно было изменить heads[priority]
    Node** head = &queue->heads[priority];
    
    if (*head == NULL) {
        // Список пуст - новый узел становится головой
        *head = new_node;
    } else {
        // Список не пуст - ищем последний элемент
        Node* current = *head;
        while (current->next) {
            current = current->next; // Идём к следующему узлу
        }
        // Добавляем новый узел в конец списка
        current->next = new_node;
    }
}
```

### **Алгоритм извлечения элемента с точным приоритетом:**
1. Проверяем, существует ли список для заданного приоритета
2. Если нет - возвращаем NULL
3. Если есть - берём первый элемент (голову списка)
4. Копируем данные 
5. Обновляем голову списка на следующий элемент
6. Освобождаем память старого узла
7. Возвращаем копию данных

```c
char* dequeue_exact(PriorityQueue* queue, unsigned char priority) {
    // Получаем указатель на голову списка для заданного приоритета
    Node** head = &queue->heads[priority];
    
    // Если список пуст, возвращаем NULL
    if (*head == NULL) return NULL;

    // Сохраняем указатель на первый узел
    Node* temp = *head;
    
    // Создаём копию данных в динамической памяти
    // strdup выделяет память и копирует строку
    char* result = strdup(temp->data);
    
    // Обновляем голову списка на следующий элемент
    *head = temp->next;
    
    // Освобождаем память старого узла
    free(temp);
    
    // Возвращаем копию данных
    return result;
}
```
### **Алгоритм извлечения элемента с приоритетом >= заданного:**
1. Перебираем приоритеты от min_priority до 255
2. Для каждого приоритета вызываем dequeue_exact
3. Как только нашли элемент - возвращаем его
4. Если ничего не нашли - возвращаем NULL

```c
char* dequeue_min_priority(PriorityQueue* queue, unsigned char min_priority) {
    // Перебираем приоритеты от min_priority до 255
    for (int i = min_priority; i < MAX_PRIORITY; ++i) {
        // Пытаемся извлечь элемент с приоритетом i
        char* result = dequeue_exact(queue, i);
        
        // Если нашли элемент - возвращаем его
        if (result) return result;
    }
    
    // Ничего не нашли - возвращаем NULL
    return NULL;
}
```

### **Алгоритм извлечения самого приоритетного элемента из очереди:**
1. Перебираем приоритеты от 0 до 255
2. Для каждого приоритета вызываем dequeue_exact
3. Как только нашли элемент - возвращаем его
4. Если ничего не нашли - возвращаем NULL

```c
char* dequeue_any(PriorityQueue* queue) {
    // Перебираем приоритеты от 0 до 255
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        // Пытаемся извлечь элемент с приоритетом i
        char* result = dequeue_exact(queue, i);
        
        // Если нашли элемент - возвращаем его
        if (result) return result;
    }
    
    // Ничего не нашли - возвращаем NULL
    return NULL;
}
```
### **Алгоритм очистки очереди (освобождаем память всех узлов во всех списках):**
1. Перебираем все 256 списков
2. Для каждого списка проходимся по всем узлам
3. Освобождаем память каждого узла
4. Устанавливаем heads[i] = NULL

```c
void free_queue(PriorityQueue* queue) {
    // Перебираем все 256 приоритетов
    for (int i = 0; i < MAX_PRIORITY; ++i) {
        // Начинаем с головы списка
        Node* current = queue->heads[i];
        
        // Проходимся по всем узлам списка
        while (current) {
            // Сохраняем указатель на следующий узел
            Node* next = current->next;
            
            // Освобождаем память текущего узла
            free(current);
            
            // Переходим к следующему узлу
            current = next;
        }
        
        // Устанавливаем голову списка в NULL
        queue->heads[i] = NULL;
    }
}
```
### **Демонстрация работы очереди:**

```c
void run_demo() {
    PriorityQueue queue;
    init_queue(&queue);

    // Добавляем сообщения с различными приоритетами
    enqueue(&queue, "Сообщение A (prio 10)", 10);
    enqueue(&queue, "Сообщение B (prio 5)", 5);
    enqueue(&queue, "Сообщение C (prio 250)", 250);
    enqueue(&queue, "Сообщение D (prio 5)", 5);
    enqueue(&queue, "Сообщение E (prio 200)", 200);

    // Извлекаем самый приоритетный элемент
    printf("Извлечение любого элемента:\n");
    char* msg = dequeue_any(&queue);
    if (msg) { printf("-> %s\n", msg); free(msg); }

    // Извлекаем все элементы с приоритетом >= 100
    printf("Извлечение с приоритетом >= 100:\n");
    while ((msg = dequeue_min_priority(&queue, 100)) != NULL) {
        printf("-> %s\n", msg);
        free(msg);
    }


    // Извлекаем все элементы с приоритетом 5
    printf("Извлечение с приоритетом 5:\n");
    while ((msg = dequeue_exact(&queue, 5)) != NULL) {
        printf("-> %s\n", msg);
        free(msg);
    }

    // Очищаем оставшиеся элементы
    printf("Очистка оставшихся элементов...\n");
    free_queue(&queue);
}
```

### **Точка входа в программу:**
```c
int main() {
    int choice;
    
    do {
        printf("\n1. Демонстрация работы очереди\n");
        printf("2. Запустить юнит-тесты\n");
        printf("0. Выход\n");
        printf("Выберите действие: ");
        
        // Читаем выбор пользователя
        if (scanf("%d", &choice) != 1) {
            // Если ввели не число, очищаем буфер ввода
            char temp[100];
            fgets(temp, sizeof(temp), stdin);
            printf("Ошибка: введите число!\n");
            continue;
        }
        
        // Очищаем буфер ввода после scanf
        char temp[100];
        fgets(temp, sizeof(temp), stdin);

        // Обрабатываем выбор
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
```

---

## **Юнит-тесты ```tests.c```:**
### Тестируются: инициализация, добавление элементов, извлечение с различными условиями (любой элемент, точный приоритет, приоритет не ниже заданного), FIFO порядок внутри одного приоритета, очистка памяти.

```c
// Глобальные счётчики для статистики
static int tests_passed = 0;  // Сколько тестов прошло
static int tests_failed = 0;  // Сколько тестов провалилось
static int tests_total = 0;   // Общее количество тестов

// Макрос для проверки строкового равенства
#define ASSERT_STR_EQ(expected, actual, test_name) \
    do { \
        tests_total++; \
        printf("[%02d] %s\n", tests_total, test_name); \
        if (strcmp((expected), (actual)) == 0) { \ // strcmp возвращает 0, если строки одинаковые
            printf("     [PASS] '%s'\n", expected); \
            tests_passed++; \
        } else { \
            printf("     [FAIL] Ожидалось: '%s', Получено: '%s'\n", expected, actual); \
            tests_failed++; \
        } \
    } while(0)

// Макрос для проверки: "Ожидается, что указатель равен NULL"
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

// Макрос для проверки: "Ожидается, что указатель не NULL"
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
```

### **ТЕСТ 1: ИНИЦИАЛИЗАЦИЯ + ОЧИСТКА**

Проверяются функции init_queue и free_queu:
1. После init_queue все списки пусты (все heads[i] = NULL)
2. После добавления элементов списки становятся непустыми
3. После free_queue все списки снова пусты
```c
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
```

### **ТЕСТ 2: ДОБАВЛЕНИЕ И FIFO ПОРЯДОК**

Проверяется функция enqueue: внутри одного приоритета элементы добавляются в конец списка 
(FIFO - First In, First Out)   
Добавляем 3 элемента с приоритетом 5:
1. "Первое"
2. "Второе"
3. "Третье"

Ожидаемая структура:   
heads[5]->[Первое]->[Второе]->[Третье]->NULL

```c
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
    
    // Проверяем первый элемент
    ASSERT_STR_EQ("Первое", queue.heads[5]->data, "1-й элемент");
    
    // Проверяем второй элемент
    ASSERT_STR_EQ("Второе", queue.heads[5]->next->data, "2-й элемент");
    
    // Проверяем третий элемент
    ASSERT_STR_EQ("Третье", queue.heads[5]->next->next->data, "3-й элемент");
    
    // Проверяем, что четвёртого элемента нет
    ASSERT_NULL(queue.heads[5]->next->next->next, "4-й элемент == NULL");
    
    free_queue(&queue);
}
```

### **ТЕСТ 3: DEQUEUE_ANY (ИЗВЛЕЧЕНИЕ ПО ПРИОРИТЕТУ)**

Проверяем функцию dequeue_any: элементы извлекаются в порядке возрастания приоритета   
(0 - самый высокий приоритет, 255 - самый низкий)
Добавляем элементы с приоритетами: 100, 10, 50, 1, 200
Ожидаемый порядок извлечения: 1, 10, 50, 100, 200

Алгоритм dequeue_any:
+ Перебираем heads[0], heads[1], heads[2], ...
+ Как только нашли непустой список - берём первый элемент
+ Это гарантирует, что элемент с приоритетом 1 извлечётся первым
```c
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
    
    // Извлекаем 1-й элемент (должен быть Prio 1)
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 1", msg, "1-й извлечён"); free(msg); }
    
    // Извлекаем 2-й элемент (должен быть Prio 10)
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 10", msg, "2-й извлечён"); free(msg); }
    
    // Извлекаем 3-й элемент (должен быть Prio 50)
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 50", msg, "3-й извлечён"); free(msg); }
    
    // Извлекаем 4-й элемент (должен быть Prio 100)
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 100", msg, "4-й извлечён"); free(msg); }
    
    // Извлекаем 5-й элемент (должен быть Prio 200)
    msg = dequeue_any(&queue);
    if (msg) { ASSERT_STR_EQ("Prio 200", msg, "5-й извлечён"); free(msg); }
    
    // Пытаемся извлечь 6-й элемент (очередь должна быть пуста)
    msg = dequeue_any(&queue);
    ASSERT_NULL(msg, "6-й извлечён == NULL (очередь пуста)");
    
    free_queue(&queue);
}
```

### **ТЕСТ 4: СПЕЦИАЛЬНЫЕ ИЗВЛЕЧЕНИЯ (EXACT И MIN_PRIORITY)**


Проверяем функции dequeue_exact и dequeue_min_priority

Часть A: dequeue_exact (точный приоритет)   
Добавляем: "Prio 5-A", "Prio 5-B" (оба с приоритетом 5)   
dequeue_exact(5) должен вернуть "A", потом "B", потом NULL   

Часть B: dequeue_min_priority (приоритет >= заданного)   
Добавляем: "Prio 20", "Prio 50"   
dequeue_min_priority(15) должен найти первый элемент с prio >= 15   
Перебираем: 15, 16, ..., 20 (нашли!) -> возвращаем "Prio 20"   
dequeue_min_priority(100) должен вернуть NULL (нет элементов с prio >= 100)   

```c
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
    
    // Извлекаем первый элемент с приоритетом 5
    char* msg1 = dequeue_exact(&queue, 5);
    if (msg1) { ASSERT_STR_EQ("Prio 5-A", msg1, "exact(5) -> A"); free(msg1); }
    
    // Извлекаем второй элемент с приоритетом 5
    char* msg2 = dequeue_exact(&queue, 5);
    if (msg2) { ASSERT_STR_EQ("Prio 5-B", msg2, "exact(5) -> B"); free(msg2); }
    
    // Пытаемся извлечь третий элемент с приоритетом 5 (его нет)
    char* msg3 = dequeue_exact(&queue, 5);
    ASSERT_NULL(msg3, "exact(5) -> NULL (больше нет)");
    
    printf("\n=== Часть B: dequeue_min_priority (>= заданного) ===\n");
    printf("Извлекаем с приоритетом >= 15:\n");
    printf("  Ищем: 15, 16, ..., 20 (нашли!)\n\n");
    
    // Извлекаем первый элемент с приоритетом >= 15
    char* msg4 = dequeue_min_priority(&queue, 15);
    if (msg4) { ASSERT_STR_EQ("Prio 20", msg4, "min_priority(15) -> 20"); free(msg4); }
    
    printf("\nИзвлекаем с приоритетом >= 100:\n");
    printf("  Ищем: 100, 101, ..., 255 (все пустые)\n\n");
    
    // Пытаемся извлечь элемент с приоритетом >= 100 (его нет)
    char* msg5 = dequeue_min_priority(&queue, 100);
    ASSERT_NULL(msg5, "min_priority(100) -> NULL");
    
    free_queue(&queue);
}
```

### **ТЕСТ 5: ИЗВЛЕЧЕНИЕ ИЗ ПУСТОЙ ОЧЕРЕДИ**

Проверяем, что все функции извлечения корректно обрабатывают   
пустую очередь и возвращают NULL, а не падают с ошибкой   

Проверяем:   
dequeue_any(&queue) должен вернуть NULL   
dequeue_exact(&queue, 5) должен вернуть NULL   
dequeue_min_priority(&queue, 0) должен вернуть NULL   

```c
static void test_empty_queue() {
    printf("\n=== ТЕСТ 5: Извлечение из пустой очереди ===\n");
    
    PriorityQueue queue;
    init_queue(&queue);
    
    printf("Очередь пуста (все heads[i] = NULL)\n\n");
    
    // Пытаемся извлечь любой элемент из пустой очереди
    char* msg1 = dequeue_any(&queue);
    ASSERT_NULL(msg1, "dequeue_any -> NULL");
    
    // Пытаемся извлечь элемент с приоритетом 5 из пустой очереди
    char* msg2 = dequeue_exact(&queue, 5);
    ASSERT_NULL(msg2, "dequeue_exact(5) -> NULL");
    
    // Пытаемся извлечь элемент с приоритетом >= 0 из пустой очереди
    char* msg3 = dequeue_min_priority(&queue, 0);
    ASSERT_NULL(msg3, "dequeue_min_priority(0) -> NULL");
    
    printf("\nОчередь всё ещё пуста (ничего не сломалось):\n");
    ASSERT_NULL(queue.heads[0], "heads[0] == NULL");
    ASSERT_NULL(queue.heads[255], "heads[255] == NULL");
    
    free_queue(&queue);
}
```
### **ЗАПУСК ТЕСТОВ**

```c
void run_all_tests(void) {
    printf("\n");
    printf("==================================================\n");
    printf("              ЗАПУСК ЮНИТ-ТЕСТОВ\n");
    printf("           (Очередь с приоритетом)\n");
    printf("==================================================\n");
    
    // Обнуляем счётчики перед запуском
    tests_passed = 0;
    tests_failed = 0;
    tests_total = 0;

    // Запускаем все 5 тестов по очереди
    test_lifecycle();
    test_enqueue_fifo();
    test_dequeue_any_priority();
    test_special_dequeue();
    test_empty_queue();

    // Выводим итоговый отчёт
    printf("\n==================================================\n");
    printf("                    РЕЗУЛЬТАТЫ\n");
    printf("==================================================\n");
    printf("Всего: %d | Пройдено: %d | Провалено: %d\n", 
           tests_total, tests_passed, tests_failed);
    printf("==================================================\n");
    
    if (tests_failed > 0) {
        printf("СТАТУС: [FAIL] Обнаружены ошибки в реализации\n");
    } else {
        printf("СТАТУС: [PASS] Все тесты пройдены успешно\n");
    }
    printf("==================================================\n\n");
}
```

---
