#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <map>
#include <algorithm>
#include "custom_exceptions.h"
#include "table.h"
#include "virtual_table.h"
#include "sql_parser/SQLParser.h"

using namespace std;

class database{
    public:
        typedef struct condition{
            hsql::OperatorType cur_op;
            bool is_num1;
            bool is_num2;
            bool is_double1;
            bool is_double2;
            int num_int1;
            int num_int2;
            double num_double1;
            double num_double2;
            string attr_name1;
            string attr_name2;
        }condition;
        database();

        ~database();

        void read_in_csv(const string& file_path);

        int execute_query(const string& command);

        void display_all_table_name();

        void display_table(const string& name);

        shared_ptr<table> get_table(const string& table_name);
        
        // function to join two tables
        shared_ptr<table> simple_join(const string& table_name_1, const string& table_name_2, const string& new_table_name);
        
        // function to join two tables
        shared_ptr<table> simple_union(const string& table_name_1, const string& table_name_2, const string& new_table_name);
        
        // function to join two tables
        shared_ptr<table> simple_diff(const string& table_name_1, const string& table_name_2, const string& new_table_name);
    
        // projection function
        shared_ptr<table> projection(const string& tableIn, vector<string> column_names);

        //rename table operator
        void RenameTable (const string& tableIn, const string new_table_name);

        //rename attributes of tables
        void RenameTableAttributes (const string& tableIn, vector<string> newAttrNames);

        //print attribute names of a particular table
        vector<string> display_current_attributeNames_of_a_table(const string& tableIn);

        // function to intersect two tables
        shared_ptr<table> simple_intersection(const string& table_name_1, const string& table_name_2, const string& new_table_name);

        // function to do selection
        vector<int> helper_selection(shared_ptr<table> table, hsql::Expr* expr);

        // function to do selection
        vector<int> helper_selection(shared_ptr<table> table, condition cond);

        // overload version of simple_join
        shared_ptr<table> simple_join(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name);

        // overloaded version of projection function
        shared_ptr<table> projection(shared_ptr<table> table, vector<string> column_names);

        // overloaded version of intersection function
        shared_ptr<table> simple_intersection(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name);

        // overloaded version of union function
        shared_ptr<table> simple_union(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name);
    private:
        // all the tables
        map<string, shared_ptr<table> > _store;

        int _num_of_tables;

        // identify whether two tuples are equal
        bool equal_tuple(vector<void *>& tuple1, vector<void *>& tuple2, vector<char>& types);

        template <typename T>
        bool operator_helper(T data1, T data2, hsql::OperatorType op);
};

#endif