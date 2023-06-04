reforward: 
	cat head.h>reforward.c
	cat initfd.c>>reforward.c
	cat accept.c>>reforward.c
	cat rns.c>>reforward.c
	cat main.c>>reforward.c
	$(CC) reforward.c -o reforward -static -pthread
	
reforward2:
	$(CC) reforward2.c -o reforward -static

clean:
	rm reforward.*
