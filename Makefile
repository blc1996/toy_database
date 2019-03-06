Compiler = clang++
cflags = -g -Wall -std=c++11
objects = main.o csv.o
dir = ./src/

main: main.o csv.o
	$(Compiler) -o exec main.o csv.o
	rm -rf $(objects)

main.o: $(dir)main.cpp
	$(Compiler) $(cflags) -c $(dir)main.cpp -o main.o

csv.o: $(dir)csv.cpp
	$(Compiler) $(cflags) -c $(dir)csv.cpp -o csv.o

clean:
	rm -rf $(objects) exec