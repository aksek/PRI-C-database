database: list.o assist.o fileSupport.o
	gcc list.o assist.o fileSupport.o -o database

list.o: list.c assist.h fileSupport.h
	gcc -c -Wall -g -std=c99 list.c
	
fileSupport.o: fileSupport.c fileSupport.h assist.h list.h
	gcc -c -Wall -g -std=c99 fileSupport.c
	
assist.o: assist.c assist.h list.h
	gcc -c -Wall -g -std=c99 assist.c
