## 6.1 Доработать решение задачи 4.1 (список контактов) так, чтобы структуры и функции по работе с двухсвязным упорядоченным списком находились в статической библиотеке.

В Makefile создается статическая библиотека ```libphonebook.a```, состоящая изследующих файлов:

```list.h``` - содержит объявление структур Contact и Item;    
```list.c``` - определение функций по работе с двухсвязным упорядоченным списком.

```make
libphonebook.a: list.c list.h
	gcc -c list.c
	ar rcs libphonebook.a list.o
```

Создаем исполняемый файла программы ```phonebook```:

```Make
phonebook: main.o tests.o libphonebook.a
	gcc -o phonebook main.o tests.o libphonebook.a
```
Объектные файлы ```main.o``` и ```tests.o``` собираются следующим образом
```Make
main.o: main.c list.h app.h tests.h
	gcc -c main.c

tests.o: tests.c list.h app.h tests.h
	gcc -c tests.c
```
Здесь:   
+ ```main.c``` - точка входа;   
+ ```tests.c``` - определение функций для тестирования;    
+ ```app.h``` - определение функций для пользовательского взаимодействия со списком;  
+ ```tests.h```.  
