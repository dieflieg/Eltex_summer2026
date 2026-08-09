#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permissions.h"

void parse_symbolic(const char *sym, Permissions *perm) {
    mode_t modes = 0;
    const char *p = sym;
    
    modes |= (*p++ == 'r') ? S_IRUSR : 0;
    modes |= (*p++ == 'w') ? S_IWUSR : 0;
    modes |= (*p++ == 'x') ? S_IXUSR : 0;
    modes |= (*p++ == 'r') ? S_IRGRP : 0;
    modes |= (*p++ == 'w') ? S_IWGRP : 0;
    modes |= (*p++ == 'x') ? S_IXGRP : 0;
    modes |= (*p++ == 'r') ? S_IROTH : 0;
    modes |= (*p++ == 'w') ? S_IWOTH : 0;
    modes |= (*p++ == 'x') ? S_IXOTH : 0;
    
    perm->binary = modes;
    perm->octal = modes & 0777;
    update_symbolic(perm);
}

void parse_octal(int oct, Permissions *perm) {
    perm->octal = oct & 0777;
    perm->binary = oct & 0777;
    update_symbolic(perm);
}

void update_symbolic(Permissions *perm) {
    snprintf(perm->symbolic, 10, "%c%c%c%c%c%c%c%c%c",
        (perm->binary & S_IRUSR) ? 'r' : '-',
        (perm->binary & S_IWUSR) ? 'w' : '-',
        (perm->binary & S_IXUSR) ? 'x' : '-',
        (perm->binary & S_IRGRP) ? 'r' : '-',
        (perm->binary & S_IWGRP) ? 'w' : '-',
        (perm->binary & S_IXGRP) ? 'x' : '-',
        (perm->binary & S_IROTH) ? 'r' : '-',
        (perm->binary & S_IWOTH) ? 'w' : '-',
        (perm->binary & S_IXOTH) ? 'x' : '-');
}

void print_permissions(const Permissions *perm) {
    printf("\nТекущие права:\n");
    printf("Символьный формат: %s\n", perm->symbolic);
    printf("Цифровой формат:  %04o\n", perm->octal);
    printf("Битовое представление: ");
    for(int i = 8; i >= 0; i--) {
        printf("%d", (perm->binary >> i) & 1);
        if(i % 3 == 0 && i != 0) printf(" ");
    }
    printf("\n\n");
}

void process_chmod_commands(Permissions *perm, const char *command) {
    char *cmd_copy = strdup(command);
    char *token = strtok(cmd_copy, ",");
    
    while(token != NULL) {
        char *part = token;
        while(*part == ' ') part++;
        
        char *op_ptr = part;
        while(*op_ptr && !strchr("+-=", *op_ptr)) op_ptr++;
        
        if(*op_ptr == '\0') {
            token = strtok(NULL, ",");
            continue;
        }
        
        char op = *op_ptr;
        *op_ptr = '\0';
        char *rights = op_ptr + 1;
        
        mode_t mask = 0;
        char *who = part;
        while(*who) {
            switch(*who) {
                case 'u': mask |= S_IRWXU; break;
                case 'g': mask |= S_IRWXG; break;
                case 'o': mask |= S_IRWXO; break;
                case 'a': mask |= S_IRWXU | S_IRWXG | S_IRWXO; break;
            }
            who++;
        }
        
        mode_t bits = 0;
        while(*rights) {
            switch(*rights) {
                case 'r': bits |= S_IRUSR | S_IRGRP | S_IROTH; break;
                case 'w': bits |= S_IWUSR | S_IWGRP | S_IWOTH; break;
                case 'x': bits |= S_IXUSR | S_IXGRP | S_IXOTH; break;
                case 's': bits |= S_ISUID | S_ISGID; break;
                case 't': bits |= S_ISVTX; break;
            }
            rights++;
        }
        
        switch(op) {
            case '+': perm->binary |= (bits & mask); break;
            case '-': perm->binary &= ~(bits & mask); break;
            case '=': perm->binary = (perm->binary & ~mask) | (bits & mask); break;
        }
        
        token = strtok(NULL, ",");
    }
    
    free(cmd_copy);
    perm->binary &= 07777;  
    perm->octal = perm->binary;
    update_symbolic(perm);
}

// Получение реальных прав доступа указанного файла из файловой системы
void get_file_permissions(const char *filename, Permissions *perm) {
    struct stat st; // Системная структура для хранения метаданных файла
    
    // Вызываем системный вызов stat, который заполняет структуру st информацией о файле.
    // В случае ошибки (например, файл не существует) stat вернет ненулевое значение
    if (stat(filename, &st)) {
        perror("Ошибка получения информации о файле");
        return;
    }
    
    // Поле st_mode содержит тип файла и его права. 
    // Маска 0777 оставляет только базовые 9 битов прав (rwxrwxrwx)
    perm->binary = st.st_mode & 0777;
    perm->octal = perm->binary;
    update_symbolic(perm);
    
    // Сравнение с выводом системной утилиты ls -l (требование ТЗ, пункт 2)
    printf("\nСравнение с ls -l:\n");
    char command[BUF_SIZE];
    // Формируем строку команды для терминала, подставляя имя файла
    snprintf(command, BUF_SIZE, "ls -l %s", filename);
    // Выполняем команду в оболочке для наглядного сравнения
    system(command);
}