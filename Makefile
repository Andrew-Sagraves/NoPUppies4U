nopuppies4u: main.o asagrave.o bmulli21.o jdong11.o kbissonn.o
	g++ -o nopuppies4u main.o asagrave.o bmulli21.o jdong11.o kbissonn.o

main.o: main.cpp asagrave.h bmulli21.h jdong11.h
	g++ -c main.cpp

asagrave.o: asagrave.cpp asagrave.h
	g++ -c asagrave.cpp

bmulli21.o: bmulli21.cpp bmulli21.h
	g++ -c bmulli21.cpp

jdong11.o: jdong11.cpp jdong11.h
	g++ -c jdong11.cpp

kbissonn.o: kbissonn.cpp kbissonn.h
	g++ -c kbissonn.cpp

clean:
	rm -f *.o program
