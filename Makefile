CFLAGS ?= -std=gnu99 -O4 -Wall -fPIC
EXAMPLE_BINARIES = examples/callback_parser examples/pause_parser

default: multipart_parser.o

multipart_parser.o: multipart_parser.c multipart_parser.h

solib: multipart_parser.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,libmultipart.so -o libmultipart.so multipart_parser.o

examples: $(EXAMPLE_BINARIES)

examples/% : examples/%.c multipart_parser.o
	$(CC) $(CFLAGS) -I. $< multipart_parser.o -o examples/$*

tests: $(EXAMPLE_BINARIES)
	@cat test/hello_world.data | examples/callback_parser >&-
	@cat test/hello_world.data | examples/pause_parser >&-
	@echo done

clean:
	-rm -f *.o *.so
	-rm $(EXAMPLE_BINARIES)
