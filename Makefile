assh : assh.o util.o
	gcc util.o assh.o -o assh

assh.o : assh.c util.h wordList.h parser.h
	gcc -Wall -Wextra -Werror -std=c99 -c assh.c

util.o : util.c util.h
	gcc -Wall -Wextra -Werror -std=c99 -c util.c
