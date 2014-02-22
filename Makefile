OBJS = buffer.o dict.o error.o files.o graph.o list.o \
       macros.o main.o parse.o string.o xmalloc.o
HEADERS = buffer.h dict.h error.h files.h graph.h list.h \
          macros.h parse.h string.h xmalloc.h

COMPILE_CMD = $(CC) -c $(CFLAGS) -o $@ $<

make: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f make $(OBJS)


buffer.o: buffer.c $(HEADERS)
	$(COMPILE_CMD)

dict.o: dict.c $(HEADERS)
	$(COMPILE_CMD)

error.o: error.c $(HEADERS)
	$(COMPILE_CMD)

files.o: files.c $(HEADERS)
	$(COMPILE_CMD)

graph.o: graph.c $(HEADERS)
	$(COMPILE_CMD)

list.o: list.c $(HEADERS)
	$(COMPILE_CMD)

macros.o: macros.c $(HEADERS)
	$(COMPILE_CMD)

main.o: main.c $(HEADERS)
	$(COMPILE_CMD)

parse.o: parse.c $(HEADERS)
	$(COMPILE_CMD)

string.o: string.c $(HEADERS)
	$(COMPILE_CMD)

xmalloc.o: xmalloc.c $(HEADERS)
	$(COMPILE_CMD)
