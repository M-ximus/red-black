CFLAGS = -g -O0 -MD
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
DOT_FLAGS = -Tpng

test: unittest.out

unittest.out: unittests.o rb-tree.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

cov_tree: rb-tree.gcno rb-tree.gcda
	gcov rb-tree.c

rb-tree.gcno: unittests.c rb-tree.c
	$(CC) $(GCOV_FLAGS) -o cov.out $^

rb-tree.gcda: cov.out
	./cov.out

cov.out: unittests.c rb-tree.c
	$(CC) $(GCOV_FLAGS) -o cov.out $^

dot: tree.png

tree.png: debug.dot
	dot $(DOT_FLAGS) -o $@ $<

debug.dot: unittest.out
	./unittest.out

-include *.d

.PHONY: clean

clean:
	rm *.o *.d *.out *.gcov *.gcda *.gcno *.dot *.png
