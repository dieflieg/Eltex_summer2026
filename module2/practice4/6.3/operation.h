#ifndef OPERATION_H
#define OPERATION_H

#include <stddef.h>

// Типы указателей на функции
typedef double (*operation_fn)(double a, double b);
typedef int (*validate_fn)(double a, double b, char *err_msg, size_t err_size);

// Структура, описывающую операцию
typedef struct {
    const char *name;
    operation_fn execute;
    validate_fn validate;
} Operation;

const Operation* get_operation(void);

#endif // OPERATION_H