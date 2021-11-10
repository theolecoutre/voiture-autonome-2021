all: sendLocation

sendLocation:
	gcc -g -pthread -c marvelmind/marvelmind.c -o marvelmind.o
	gcc -g -pthread -c sendLocation.c -o sendLocation.o
	gcc -g -pthread -c ../auxiliarFunctions.c -o auxiliarFunctions.o
	gcc -o sendLocation sendLocation.o marvelmind.o auxiliarFunctions.o -pthread

receiveCommands:
	gcc -c receiveCommands.c -o receiveCommands -lpthread

clean:
	rm -rf *.o sendLocation receiveCommands