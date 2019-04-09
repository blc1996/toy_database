#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <string>
#include <vector>
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

        virtual ~table();

        // print the table
        void print();

        // get the element pointer in a certain row and col
        void* get_element(int y, int x);

        // get the pointer for a row
        vector<void *> get_tuple(int y);

        // get the vector containing the type info
        vector<char> get_types();

        int get_height(){
            return _row;
        }

        int get_width(){
            return _col;
        }

        string get_table_name();

        vector<string> get_attr_names();

        void set_attr_names(vector<string> newNames);

        void set_table_name(string newName);

    protected:
        table(vector<vector<void *>> tuples, vector<char> types, string table_name, vector<string> attr_names)
        :_tuples(tuples), _row(tuples.size()), _col(tuples[0].size()), _types(types), _table_name(table_name), _attr_names(attr_names){
            // used by virtual_table
        }
        vector<vector<void *>> _tuples; //tuples that store the data, 2D array of void*
        int _row;
        int _col;
        vector<char> _types;
        string _table_name;
        vector<string> _attr_names;

    private:
        //check if the string can be converted to INT
        bool is_int(const string& s);

        //check if the string can be converted to FLOAT
        bool is_float(const string& s);
};

#endif