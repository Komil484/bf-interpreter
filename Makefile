all: bf

bf: bf.c
	gcc bf.c -o bf

clean:
	rm bf
