#include "operation.h"
#include <math.h>
#include <string.h>

static double execute_pow(double a, double b) {
    return pow(a, b);
}

static int validate_pow(double a, double b, char *err_msg, size_t err_size) {
    if (a == 0.0 && b <= 0.0) {
        strncpy(err_msg, "Математическая неопределенность (ноль в неположительной степени)", err_size - 1);
        err_msg[err_size - 1] = '\0';
        return 0;
    }
    return 1;
}

static Operation op = {
    .name = "Возведение в степень",
    .execute = execute_pow,
    .validate = validate_pow
};

const Operation* get_operation(void) {
    return &op;
}