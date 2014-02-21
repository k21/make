OBJS = buffer.o dict.o graph.o list.o main.o parse.o string.o xmalloc.o
HEADERS = buffer.h dict.h graph.h list.h parse.h string.h xmalloc.h

COMPILE_CMD = $(CC) -c $(CFLAGS) -o $@ $<

make: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f make $(OBJS)


buffer.o: buffer.c $(HEADERS)
	$(COMPILE_CMD)

dict.o: dict.c $(HEADERS)
	$(COMPILE_CMD)

graph.o: graph.c $(HEADERS)
	$(COMPILE_CMD)

list.o: list.c $(HEADERS)
	$(COMPILE_CMD)

main.o: main.c $(HEADERS)
	$(COMPILE_CMD)

parse.o: parse.c $(HEADERS)
	$(COMPILE_CMD)

string.o: string.c $(HEADERS)
	$(COMPILE_CMD)

xmalloc.o: xmalloc.c $(HEADERS)
	$(COMPILE_CMD)
