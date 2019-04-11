#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <map>
#include "custom_exceptions.h"
#include "table.h"
#include "virtual_table.h"
#include "sql_parser/SQLParser.h"

using namespace std;

class database{
    public:
        database();

        ~database();

        void read_in_csv(const string& file_path);

        int execute_query(const string& command, shared_ptr<virtual_table>* res);

        void display_all_table_name();

        void display_table(const string& name);

        //overload
        // void display_table(shared_ptr<virtual_table> table);
        
        // function to join two tables
        shared_ptr<virtual_table> simple_join(const string& table_name_1, const string& table_name_2, const string& new_table_name);
        
        // function to join two tables
        shared_ptr<virtual_table> simple_union(const string& table_name_1, const string& table_name_2, const string& new_table_name);
        
        // function to join two tables
        shared_ptr<virtual_table> simple_diff(const string& table_name_1, const string& table_name_2, const string& new_table_name);
    
        // projection function
        shared_ptr<virtual_table> projection(const string& tableIn, vector<string> column_names);

        //rename table operator
        void RenameTable (const string& tableIn, const string new_table_name);

        //rename attributes of tables
        void RenameTableAttributes (const string& tableIn, vector<string> newAttrNames);

        //print attribute names of a particular table
        vector<string> display_current_attributeNames_of_a_table(const string& tableIn);
        
        // function to intersect two tables
        shared_ptr<virtual_table> simple_intersection(const string& table_name_1, const string& table_name_2, const string& new_table_name);
    private:
        // all the tables
        map<string, shared_ptr<table> > _store;

        int _num_of_tables;

        // identify whether two tuples are equal
        bool equal_tuple(vector<void *>& tuple1, vector<void *>& tuple2, vector<char>& types);
};

#endif