program: main.o
        g++ -o program main.o
main.o: main.cpp
        g++ -c main.cpp
clean:
        rm -f ./*.o
