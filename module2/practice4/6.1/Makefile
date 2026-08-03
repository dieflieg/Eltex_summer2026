# Сборка итоговой программы. 
# Чтобы создать phonebook, нужны main.o, tests.o и libphonebook.a
phonebook: main.o tests.o libphonebook.a
	gcc -o phonebook main.o tests.o libphonebook.a

# Создание статической библиотеки.
# Чтобы создать libphonebook.a, нужен list.o
libphonebook.a: list.c list.h
	gcc -c list.c
	ar rcs libphonebook.a list.o

# Сборка остальных объектных файлов.
main.o: main.c list.h app.h tests.h
	gcc -c main.c

tests.o: tests.c list.h app.h tests.h
	gcc -c tests.c

# Очистка проекта от сгенерированных файлов
clean:
	rm -f main.o tests.o list.o libphonebook.a phonebook