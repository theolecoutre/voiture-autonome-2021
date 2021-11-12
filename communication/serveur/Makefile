all: receiveLocation sendCommands

receiveLocation:
	gcc -g -pthread -c receiveLocation.c -o receiveLocation.o
	gcc -g -pthread -c ../auxiliarFunctions.c -o auxiliarFunctions.o
	gcc -o receiveLocation receiveLocation.o auxiliarFunctions.o -pthread

sendCommands:
	gcc -c sendCommands.c -o sendCommands -lpthread

clean:
	rm -rf *.o receiveLocation sendCommands