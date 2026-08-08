#include "operation.h"

static double execute_add(double a, double b) {
    return a + b;
}

static Operation op = {
    .name = "Сложение",
    .execute = execute_add,
    .validate = NULL
};

const Operation* get_operation(void) {
    return &op;
}