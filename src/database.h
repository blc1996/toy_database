#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <map>
#include "custom_exceptions.h"
#include "table.h"
#include "sql_parser/SQLParser.h"

using namespace std;

class database{
    public:
        database();

        ~database();

        void read_in_csv(string file_path);

        int execute_command(string command, void** res);

        void display_all_table_name();

        void display_table(string name);
    private:
        // all the tables
        // vector<table *> _store;
        map<string, table*> _store;
        int _num_of_tables;
};

#endif