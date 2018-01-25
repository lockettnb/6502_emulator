linenoise_example: linenoise.h linenoise.c

linenoise_example: linenoise.c example.c
	$(CC) -Wall -W -Os -g -o linenoise_example linenoise.c example.c

autoc: autoc.c linenoise.c
	$(CC) -Wall -W -Os -g -o autoc linenoise.c autoc.c

clean:
	rm -f linenoise_example
