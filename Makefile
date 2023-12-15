project3: project3.o node0.o node1.o node2.o node3.o common.o
	gcc project3.o node0.o node1.o node2.o node3.o common.o -o project3

project3.o: project3.c
	gcc -c project3.c -o project3.o

node0.o: node0.c
	gcc -c node0.c -o node0.o

node1.o: node1.c
	gcc -c node1.c -o node1.o

node2.o: node2.c
	gcc -c node2.c -o node2.o

node3.o: node3.c
	gcc -c node3.c -o node3.o

common.o: common.c
	gcc -c common.c -o common.o

clean: project3 project3.o node0.o node1.o node2.o node3.o common.o
	rm project3
	rm project3.o
	rm node0.o
	rm node1.o
	rm node2.o
	rm node3.o
	rm common.o