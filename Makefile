all: lets-talk.c list.c client.c server.c
	gcc -g -Wall -o lets-talk  lets-talk.c list.c client.c server.c -pthread

valgrind:
	gcc -g -Wall -o lets-talk  lets-talk.c list.c client.c server.c -pthread
	valgrind --leak-check=full ./lets-talk 25000 localhost 25000

clean:
	$(RM) lets-talk