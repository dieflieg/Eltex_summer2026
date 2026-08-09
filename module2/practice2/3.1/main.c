#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permissions.h"

int main() {
    char input[BUF_SIZE];
    Permissions perm = {0};
    int running = 1;

    while(running) {
        printf("\n=== Меню управления правами ===\n");
        printf("1. Ввод прав в символьном/цифровом формате\n");
        printf("2. Показать права файла\n");
        printf("3. Изменить права (chmod-подобный синтаксис)\n");
        printf("4. Запустить юнит-тесты\n");
        printf("5. Выход\n");
        printf("Выберите действие: ");
        
        fgets(input, BUF_SIZE, stdin);
        int choice = atoi(input);

        switch(choice) {
            case 1: {
                printf("Введите права (например, 755 или rwxr-xr-x): ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = '\0';
                if (input[0] >= '0' && input[0] <= '9') {
                    parse_octal((int)strtol(input, NULL, 8), &perm);
                } else {
                    parse_symbolic(input, &perm);
                }
                print_permissions(&perm);
                break;
            }
            case 2: {
                printf("Введите имя файла: ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = '\0';
                get_file_permissions(input, &perm);
                print_permissions(&perm);
                break;
            }
            case 3: {
                printf("Введите команду изменения прав (например, u+rwx,g-x): ");
                fgets(input, BUF_SIZE, stdin);
                input[strcspn(input, "\n")] = '\0';
                process_chmod_commands(&perm, input);
                print_permissions(&perm);
                break;
            }
            case 4:
                run_all_tests();
                break;
            case 5:
                running = 0;
                break;
            default:
                printf("Неверный выбор!\n");
        }
    }
    return 0;
}