#CFLAGS = -Wall -Wunused-parameter -Wpadded -Wextra

main:
	cc src/transfer.c src/tree.c src/arena.c -o src/transfer

run:
	./src/transfer

clean:
	rm -f src/transfer
