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

        int execute_command(const string& command, void** res);

        void display_all_table_name();

        void display_table(const string& name);

        //overload
        // void display_table(shared_ptr<virtual_table> table);
        
        // function to join two tables
        shared_ptr<virtual_table> simple_join(const string& table_name_1, const string& table_name_2, const string& new_table_name);
        private:
        // all the tables
        // vector<table *> _store;
        map<string, shared_ptr<table> > _store;
        int _num_of_tables;
};

#endif