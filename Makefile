OBJS = buffer.o dict.o graph.o list.o main.o string.o xmalloc.o
HEADERS = buffer.h dict.h graph.h list.h string.h xmalloc.h

make: $(OBJS)
	gcc $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm make $(OBJS)


buffer.o: buffer.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

dict.o: dict.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

graph.o: graph.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

list.o: list.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

main.o: main.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

string.o: string.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<

xmalloc.o: xmalloc.c $(HEADERS)
	gcc -c -Wall -Wextra -Wconversion $(CFLAGS) -o $@ $<
