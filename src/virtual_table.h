// virtual_table is derived from table, but it doesn't really store a copy of data.
// it only stores the pointer to datas, used to represent the join product of two tables
#ifndef VIRTUAL_TABLE_H
#define VIRTUAL_TABLE_H

#include <vector>
#include "table.h"

class virtual_table: public table{
    public:
        // constructor of virtual table, no default constructor
        virtual_table(vector<vector<void *> > tuples, vector<char> types, string table_name, vector<string> attr_names)
        :table(tuples, types, table_name, attr_names){
            
        }

        virtual_table(const table& t): table(t){
        }

        ~virtual_table(){
            // all virtual, nothing to release
            cout<<"virtual destrctor"<<endl;
            _row = 0;
            _col = 0;
        }
};

#endif