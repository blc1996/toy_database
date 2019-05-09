INSTALL  = /usr/local

RED  =  "\e[31;1m"

Compiler = clang++
cflags = -g -Wall -std=c++11
objects = main.o table.o database.o query_executor.o
dir = ./src/

LIB_SQL = libsqlparser.so

main: $(objects)
	$(Compiler) -o exec $(objects) -lsqlparser
	# rm -rf $(objects)

main.o: $(dir)main.cpp $(dir)b_plus_tree.h $(dir)b_plus_tree.cpp
	$(Compiler) $(cflags) -c $(dir)main.cpp -o main.o

table.o: $(dir)table.cpp $(dir)table.h $(dir)b_plus_tree.h $(dir)b_plus_tree.cpp
	$(Compiler) $(cflags) -c $(dir)table.cpp -o table.o

database.o: $(dir)database.cpp $(dir)database.h $(dir)virtual_table.h
	$(Compiler) $(cflags) -c $(dir)database.cpp -o database.o

# b_plus_tree.o: $(dir)b_plus_tree.cpp $(dir)b_plus_tree.h
# 	$(Compiler) $(cflags) -c $(dir)b_plus_tree.h -o b_plus_tree.o

query_executor.o: $(dir)query_executor.cpp $(dir)query_executor.h
	$(Compiler) $(cflags) -c $(dir)query_executor.cpp -o query_executor.o

install:
	cp $(LIB_SQL) $(INSTALL)/lib/$(LIB_SQL)

sqltest:
	$(Compiler) -std=c++1z -lstdc++ -Wall -Werror -I../src/ -L../ $(dir)example.cpp -o example -lsqlparser

merge:
	clang++ -o test2 $(dir)TestMergeCustomStruct.cpp

clean:
	rm -rf $(objects) exec example