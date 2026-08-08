#include "operation.h"

static double execute_mul(double a, double b) {
    return a * b;
}

static Operation op = {
    .name = "Умножение",
    .execute = execute_mul,
    .validate = NULL
};

const Operation* get_operation(void) {
    return &op;
}