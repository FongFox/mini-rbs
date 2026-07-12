mini-rbs: main.c protocol.c
	gcc main.c protocol.c -o mini-rbs

test: test_protocol.c protocol.c
	gcc test_protocol.c protocol.c -o test_protocol -lcheck -lm -lrt -lsubunit -pthread
	./test_protocol