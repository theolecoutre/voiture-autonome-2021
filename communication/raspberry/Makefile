all: sendLocation

sendLocation:
	gcc -g -pthread -c marvelmind/marvelmind.c -o marvelmind.o
	gcc -g -pthread -c sendLocation.c -o sendLocation.o
	gcc -g -pthread -c ../auxiliarFunctions.c -o auxiliarFunctions.o
	gcc -o sendLocation sendLocation.o marvelmind.o auxiliarFunctions.o -pthread

receiveCommands:
	gcc -g -pthread -c receiveCommands.c -o receiveCommands.o
	gcc -g -pthread -c ../auxiliarFunctions.c -o auxiliarFunctions.o
	gcc -o receiveCommands receiveCommands.o auxiliarFunctions.o -lpthread

clean:
	rm -rf *.o sendLocation receiveCommands
