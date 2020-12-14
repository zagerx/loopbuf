# TAR = main
# OBJ = main.o zg_heap.o zg_loopbuf.o
# CC = gcc

# $(TAR):$(OBJ)
# 	$(CC) $(OBJ) -o $(TAR)
# %.o:%.c
# 	$(CC) -c $^ -o $@


main: ./debug/main.o ./debug/zg_loopbuf.o ./debug/zg_heap.o
	gcc ./debug/main.o ./debug/zg_loopbuf.o ./debug/zg_heap.o -o ./bin/main
./debug/main.o:main.c
	gcc -c main.c -I .\inc -o ./debug/main.o
./debug/zg_loopbuf.o:.\src\zg_loopbuf.c	
	gcc -c .\src\zg_loopbuf.c -I .\inc -o ./debug/zg_loopbuf.o
./debug/zg_heap.o:.\src\zg_heap.c
	gcc -c .\src\zg_heap.c -I .\inc -o ./debug/zg_heap.o

.PHONY:
clc:	
	rm ./debug/main.o
	rm ./debug/zg_loopbuf.o
	rm ./debug/zg_heap.o
	rm ./bin/main.exe
			




