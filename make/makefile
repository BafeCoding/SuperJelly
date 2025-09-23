OBJS	= superjelly.o
SOURCE	= superjelly.c
HEADER	= 
OUT	= superjelly.exe
FLAGS	= -g -c -Wall
LFLAGS	= 
CC	= gcc

all:	superjelly.exe

superjelly.exe: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

superjelly.o: superjelly.c
	$(CC) $(FLAGS) superjelly.c 

clean:
	rm -f $(OBJS) $(OUT)

run: superjelly.exe
	./superjelly.exe


