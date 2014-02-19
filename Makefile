OBJS = graph.o list.o main.o xmalloc.o
HEADERS = graph.h list.h xmalloc.h

make: $(OBJS)
	gcc $(LDFLAGS) -o $@ $(OBJS)

main.o: main.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

xmalloc.o: xmalloc.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

graph.o: graph.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

list.o: list.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<
