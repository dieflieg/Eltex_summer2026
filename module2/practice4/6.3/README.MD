## 6.3 Доработать решение задачи 2.3 так, чтобы функции загружались из динамических библиотек. В одной библиотеке находится одна функция. При запуске программы считывается каталог с библиотеками и загружаются найденные функции.

В данной работе реализован калькулятор, поддерживающий расширение функциональности через динамические библиотеки, расположенные в директории `plugins/`.

### Описание архитектуры

Архитектура разделена на ядро программы и независимые модули. 

```operation.h``` - содержит определение структуры `Operation` и типов указателей на функции, которые должна реализовать каждая библиотека;    
```main.c``` - ядро программы, осуществляющее сканирование директории `plugins/` и динамическую загрузку библиотек с помощью `dlopen`/`dlsym`.

### Сборка библиотек


```make
plugins/add.so: plugins/add.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/add.so plugins/add.c

plugins/sub.so: plugins/sub.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/sub.so plugins/sub.c

plugins/mul.so: plugins/mul.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/mul.so plugins/mul.c

plugins/div.so: plugins/div.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/div.so plugins/div.c

plugins/mod.so: plugins/mod.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/mod.so plugins/mod.c -lm

plugins/pow.so: plugins/pow.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/pow.so plugins/pow.c -lm
```

### Сборка исполняемого файла

Создаем главный исполняемый файл `calculator`. Для корректной работы функций динамической загрузки (`dlopen`, `dlsym`) добавлен флаг `-ldl`.

```make
calculator: main.c operation.h
	gcc -Wall -Wextra -o calculator main.c -ldl -lm
```

Здесь:   
+ ```main.c``` - точка входа, реализующая менеджер библиотек и интерфейс пользователя;   
+ ```operation.h``` - интерфейс для взаимодействия ядра и библиотек;   
+ ```plugins/*.c``` - реализации конкретных математических операций.