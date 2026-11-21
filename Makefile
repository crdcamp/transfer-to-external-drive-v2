#CFLAGS = -Wall -Wunused-parameter -Wpadded -Wextra

main:
	cc src/main.c src/sync.c src/arena.c -o src/main

debug:
	cc -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer src/main.c src/sync.c src/arena.c -o src/main

run:
	./src/main

clean:
	rm -f src/main
