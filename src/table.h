#ifndef TABLE_H
#define TABLE_H

#define DIV 9

#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include "custom_exceptions.h"
#include "csv_parser.hpp"
#include "b_plus_tree.cpp"

using namespace std;

enum TYPE{
    INT32 = 1,
    STR,
    DOUBLE64
};

typedef struct tuple_data{
    vector<int> dataIdx;
    vector<int> dataInt;
    vector<double> dataDouble;
    vector<string> dataString;
    vector<char> type;
}tuple_data;

class table {
    public:
        // @input: path of csv file
        table(string file_path, bool write_to_disk_flag = false);

        virtual ~table();

        // print the table
        void print();

        // get the element pointer in a certain row and col
        void* get_element(int y, int x);

        // get the pointer for a row
        const vector<void *>& get_tuple(int y);

        // get the pointer for a column
        const vector<void *>& get_column(int x);

        // get the vector containing the type info
        const vector<char>& get_types();

        const vector<vector<void *>>& get_table_data();
        // identify whether two tables are equal
        bool equal_tableSchema(shared_ptr<table> other);

        int get_height(){
            return _row;
        }

        int get_width(){
            return _col;
        }

        string get_table_name();

        const vector<string>& get_attr_names();

        void set_attr_names(const vector<string>& newNames);

        void set_table_name(string newName);

        void delete_tuple(const vector<int>& tuple_index);

        void insert_into_table (vector<void*> values_vector);

        void write_to_disk();
    protected:
        table(vector<vector<void *>> tuples, vector<char> types, string table_name, vector<string> attr_names)
        :_tuples(tuples), _row(tuples.size()), _types(types), _table_name(table_name), _attr_names(attr_names){
            // used by virtual_table
            if(_row != 0){
                _col = tuples[0].size();
            }
        }
        vector<vector<void *>> _tuples; //tuples that store the data, 2D array of void*
        int _row;
        int _col;
        vector<char> _types;
        string _table_name;
        vector<string> _attr_names;
        vector<void *> column;

        bool written_to_disk;
        BPlusTree<int, long> b_tree_index;
        bool use_first_attr_as_index;
    private:
        //check if the string can be converted to INT
        bool is_int(const string& s);

        //check if the string can be converted to double
        bool is_double(const string& s);

        void delete_data();

        tuple_data decode_line(string line);
};

#endif