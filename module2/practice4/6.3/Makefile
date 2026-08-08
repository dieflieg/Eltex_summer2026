all: calculator plugins/add.so plugins/sub.so plugins/mul.so plugins/div.so plugins/mod.so plugins/pow.so

# Сборка ядра калькулятора
calculator: main.c operation.h
	gcc -Wall -Wextra -fPIC -o calculator main.c -ldl -lm

plugins/add.so: plugins/add.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/add.so plugins/add.c

plugins/sub.so: plugins/sub.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/sub.so plugins/sub.c

plugins/mul.so: plugins/mul.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/mul.so plugins/mul.c

plugins/div.so: plugins/div.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/div.so plugins/div.c

plugins/mod.so: plugins/mod.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/mod.so plugins/mod.c

plugins/pow.so: plugins/pow.c operation.h
	gcc -Wall -Wextra -fPIC -I. -shared -o plugins/pow.so plugins/pow.c

clean:
	rm -f calculator
	rm -f plugins/*.so

.PHONY: all clean
