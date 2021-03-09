build:
	gcc -Wall -g Treap.c -o treap
memory:
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./treap
run:
	./treap
clean:
	rm -f treap
