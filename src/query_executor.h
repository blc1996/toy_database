#include <vector>
#include <string>
#include "sql_parser/SQLParser.h"
#include "table.h"
#include "database.h"
#include "virtual_table.h"

class query_executor {
    public:
        query_executor(database* target_db, const string& query): db(target_db), sql_query(query){
            executed = false;
        }

        void execute();

        shared_ptr<table> get_result();
    
    private:
        void select_query();

        void insert_query();

        void update_query();

        void delete_query();

        vector<string> get_sources(hsql::TableRef* table);
        vector<string> get_name(char* tableName);
        bool executed;
        database* db;
        string sql_query;
        hsql::SQLParserResult parse_result;
        shared_ptr<table> result_table;
};