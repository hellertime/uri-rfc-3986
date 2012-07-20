CC?=gcc
AR?=ar

CPPFLAGS += -I.
CPPFLAGS_DEBUG = $(CPPFLAGS)
CPPFLAGS_OPTIMIZE = $(CPPFLAGS)

CFLAGS += -std=c99 -Wall -Wextra -Werror -pedantic -Wstrict-aliasing=2 -Wno-missing-field-initializers
CFLAGS_DEBUG = $(CFLAGS) -g -ggdb -O0
CFLAGS_OPTIMIZE = $(CFLAGS) -Ofast

test: t/test_debug t/test_optimize
	./t/test_debug
	./t/test_optimize

t/test_debug: uri_debug.o t/test_debug.o
	$(CC) $(CFLAGS_DEBUG) $(LDFLAGS) uri_debug.o t/test_debug.o -o $@

t/test_debug.o: t/test.c uri.h Makefile
	$(CC) $(CPPFLAGS_DEBUG) $(CFLAGS_DEBUG) -c t/test.c -o $@

uri_debug.o: uri.c uri.h Makefile
	$(CC) $(CPPFLAGS_DEBUG) $(CFLAGS_DEBUG) -c uri.c -o $@

t/test_optimize: uri_optimize.o t/test_optimize.o
	$(CC) $(CFLAGS_OPTIMIZE) $(LDFLAGS) uri_optimize.o t/test_optimize.o -o $@

t/test_optimize.o: t/test.c uri.h Makefile
	$(CC) $(CPPFLAGS_OPTIMIZE) $(CFLAGS_OPTIMIZE) -c t/test.c -o $@

uri_optimize.o: uri.c uri.h Makefile
	$(CC) $(CPPFLAGS_OPTIMIZE) $(CFLAGS_OPTIMIZE) -c uri.c -o $@

clean:
	rm -f *.o t/test_debug t/test_optimize

.PHONY: clean

