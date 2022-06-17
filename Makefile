all: run

#CFLAGS = -Wall -Wextra -O2
CFLAGS = -O2
CC = gcc

Main: Main.o support_sorts.o qsort.o heapsort.o merge_tim_sorts.o cubesort.o
run: Main
	./Main
clean:
	del *.o *.exe
#	$(RM) *.o *.exe
