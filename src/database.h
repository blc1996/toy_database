#ifndef DATABASE_H
#define DATABASE_H

#include "csv.h"
#include "SQLParser.h"

class database{
    public:
        database();

        ~database();

        void read_in_csv(string file_path);

        table command_parser()

    private:
        // all the tables
        vector<table *> _store;
        int _num_of_tables;
};

#endif