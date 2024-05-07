CC=gcc
CFLAGS=-Wall -Wextra -g

LOAD=load_balancer
SERVER=server
CACHE=lru_cache
UTILS=utils

# Add new source file names here:
AL=arraylist
LL=linked_list
Q=queue
HM=hashmap
DLL=doubly_linked_list

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(CACHE).o $(UTILS).o $(AL).o $(LL).o $(Q).o $(HM).o $(DLL).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(CACHE).o: $(CACHE).c $(CACHE).h
	$(CC) $(CFLAGS) $^ -c

$(UTILS).o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $^ -c

$(AL).o: $(AL).c $(AL).h
	$(CC) $(CFLAGS) $^ -c

$(LL).o: $(LL).c $(LL).h
	$(CC) $(CFLAGS) $^ -c

$(Q).o: $(Q).c $(Q).h
	$(CC) $(CFLAGS) $^ -c

$(HM).o: $(HM).c $(HM).h
	$(CC) $(FLAGS) $^ -c

$(DLL).o: $(DLL).c $(DLL).h
	$(CC) $(FLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch
