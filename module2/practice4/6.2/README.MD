## 6.2 Доработать решение задачи 6.1 (список контактов) так, чтобы структуры и функции по работе с двухсвязным упорядоченным списком находились в динамической библиотеке.
В Makefile создается динамическая библиотека ```libphonebook.so```, состоящая из следующих файлов:

```list.h``` - содержит объявление структур Contact и Item;    
```list.c``` - определение функций по работе с двухсвязным упорядоченным списком.

```make
libphonebook.so: list.c list.h
	gcc -fPIC -c list.c
	gcc -shared -o libphonebook.so list.o
```

Создаем исполняемый файл программы ```phonebook```:

```make
phonebook: main.o tests.o libphonebook.so
	gcc -o phonebook main.o tests.o -L. -lphonebook -Wl,-rpath,.
```

Объектные файлы ```main.o``` и ```tests.o``` собираются следующим образом:

```make
main.o: main.c list.h app.h tests.h
	gcc -c main.c

tests.o: tests.c list.h app.h tests.h
	gcc -c tests.c
```

Здесь:   
+ ```main.c``` - точка входа;   
+ ```tests.c``` - определение функций для тестирования;    
+ ```app.h``` - определение функций для пользовательского взаимодействия со списком;  
+ ```tests.h``` - объявление функции запуска тестов.