#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SIZE 1024

// Структура для хранения информации о правах доступа
typedef struct {
    mode_t binary;    // Битовое представление
    int octal;        // Восьмеричное представление
    char symbolic[10];// Символьное представление (rwxrwxrwx + '\0')
} Permissions;

// Прототипы функций
void parse_symbolic(const char *sym, Permissions *perm);
void parse_octal(int oct, Permissions *perm);
void update_symbolic(Permissions *perm);
void print_permissions(const Permissions *perm);
void process_chmod_commands(Permissions *perm, const char *command);
void get_file_permissions(const char *filename, Permissions *perm);

// Функция запуска юнит-тестов
void run_all_tests(void);

#endif // PERMISSIONS_H