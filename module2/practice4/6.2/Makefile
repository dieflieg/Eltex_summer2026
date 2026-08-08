# Сборка итоговой программы. 

phonebook: main.o tests.o libphonebook.so
	gcc -o phonebook main.o tests.o -L. -lphonebook -Wl,-rpath,.

# Cоздание динамической библиотеки.
libphonebook.so: list.o
	gcc -shared -o libphonebook.so list.o

# Компиляция исходников в объектные файлы (.o)

list.o: list.c list.h
	gcc -fPIC -c list.c

main.o: main.c list.h app.h tests.h
	gcc -c main.c

tests.o: tests.c list.h app.h tests.h
	gcc -c tests.c

# Очистка проекта
clean:
	rm -f main.o tests.o list.o libphonebook.so phonebook