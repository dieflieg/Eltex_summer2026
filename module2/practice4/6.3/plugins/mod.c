#include "operation.h"
#include <math.h>
#include <string.h>

static double execute_mod(double a, double b) {
    return fmod(a, b);
}

static int validate_mod(double a, double b, char *err_msg, size_t err_size) {
    if (b == 0.0) {
        strncpy(err_msg, "Делитель не может быть равен нулю", err_size - 1);
        err_msg[err_size - 1] = '\0';
        return 0;
    }
    return 1;
}

static Operation op = {
    .name = "Остаток от деления",
    .execute = execute_mod,
    .validate = validate_mod
};

const Operation* get_operation(void) {
    return &op;
}