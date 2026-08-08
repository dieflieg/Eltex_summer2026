#include "operation.h"
#include <string.h>

static double execute_div(double a, double b) {
    return a / b;
}

static int validate_div(double a, double b, char *err_msg, size_t err_size) {
    if (b == 0.0) {
        strncpy(err_msg, "Деление на ноль запрещено", err_size - 1);
        err_msg[err_size - 1] = '\0';
        return 0;
    }
    return 1;
}

static Operation op = {
    .name = "Деление",
    .execute = execute_div,
    .validate = validate_div
};

const Operation* get_operation(void) {
    return &op;
}