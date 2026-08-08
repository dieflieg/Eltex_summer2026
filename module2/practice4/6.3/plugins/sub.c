#include "operation.h"

static double execute_sub(double a, double b) {
    return a - b;
}

static Operation op = {
    .name = "Вычитание",
    .execute = execute_sub,
    .validate = NULL
};

const Operation* get_operation(void) {
    return &op;
}