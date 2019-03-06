#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include "custom_exceptions.h"
#include "csv_parser.hpp"

using namespace std;

enum TYPE{
    INT64 = 1,
    STR,
    FLOAT64
};

class table{
    public:
        // @input: path of csv file
        table(string file_path);

        ~table();

        // print the table
        void print();

        // get the element pointer in a certain row and col
        void* get_element(int y, int x);

        // get the pointer for a row
        vector<void *> get_tuple(int y);

        // get the vector containing the type info
        vector<char> get_types();

        string get_table_name();

        vector<string> get_attr_names();

    private:
        vector<vector<void *>> _tuples; //tuples that store the data, 2D array of void*
        int _row;
        int _col;
        vector<char> _types;
        string _table_name;
        vector<string> _attr_names;

        //check if the string can be converted to INT
        bool is_int(const string& s);

        //check if the string can be converted to FLOAT
        bool is_float(const string& s);
};

#endif