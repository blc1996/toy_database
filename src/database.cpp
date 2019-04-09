#include "database.h"

// constructor
database::database(){
    _store = map<string, shared_ptr<table> > ();
    _num_of_tables = 0;
}

// destructor
database::~database(){
    // shared_ptr used here to store tables, no need to delete manually
}

void database::read_in_csv(const string& file_path){
    shared_ptr<table> new_table(NULL);
    try{
        new_table = shared_ptr<table>(new table(file_path));
    }catch (IOException e){
        //failed
        return;
    }
    //store the new table
    _store.insert({new_table->get_table_name(), new_table});
    _num_of_tables++;
}

void database::display_all_table_name(){
    cout<<"-----------table names------------"<<endl;
    for(auto iter = _store.begin(); iter != _store.end(); iter++){
        cout<<iter->first<<endl;
    }
}

void database::display_table(const string& name){
    if(_store.count(name)){
        _store[name]->print();
    }else{
        cout<<"No such table!"<<endl;
    }
}

// identify whether two tuples are equal
bool database::equal_tuple(vector<void *>& tuple1, vector<void *>& tuple2, vector<char>& types){
    // if two tuples don't have same number of attribute
    if(tuple1.size() != tuple2.size()){
        return false;
    }
    for(size_t i = 0; i < tuple1.size(); i++){
        switch(types[i]){
            case INT64:
                if(*(int *)tuple1[i] != *(int *)tuple2[i]){
                    return false;
                }
            break;
            case STR:
                if(*(string *)tuple1[i] != *(string *)tuple2[i]){
                    return false;
                }
            break;
            case FLOAT64:
                if(*(float *)tuple1[i] != *(float *)tuple2[i]){
                    return false;
                }
            break;
        }
       
    }
    return true;
}

shared_ptr<virtual_table> database::simple_join(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    vector<vector<void *> > joined_data(height1 * height2, vector<void*>());
    for(int i = 0; i < height1; i++){
        for(int j = 0; j < height2; j++){
            vector<void *>& temp = joined_data[i * height1 + j];
            temp = table1->get_tuple(i);
            temp.insert(temp.end(), table2->get_tuple(j).begin(), table2->get_tuple(j).end());
            cout<<temp.size()<<endl;
        }
    }
    cout<<joined_data.size()<<" "<<joined_data[0].size()<<endl;
    vector<char> joined_types = table1->get_types();
    joined_types.insert(joined_types.end(), table2->get_types().begin(), table2->get_types().end());
    shared_ptr<virtual_table> result(new virtual_table(joined_data, joined_types, new_table_name, vector<string>()));
    // modify last parameter when attr_names are considered
    return result; 
}

shared_ptr<virtual_table> database::simple_intersection(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];

    if(!table1->equal_tableSchema(table2)){
        // different table schema
        return shared_ptr<virtual_table>(NULL);
    }
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    vector<vector<void *> > inter_data;
    vector<char> inter_types = table1->get_types();
    vector<void *> temp1;
    vector<void *> temp2;
    for(int i = 0; i < height1; i++){

        temp1 = table1->get_tuple(i);
        for(int j = 0; j < height2; j++){
            
            temp2 = table2->get_tuple(j);
            if(equal_tuple(temp1, temp2, inter_types)){
                inter_data.push_back(temp1);
            }
        }
    }
    cout<<inter_data.size()<<" "<<inter_data[0].size()<<endl;
    
    shared_ptr<virtual_table> result(new virtual_table(inter_data, inter_types, new_table_name, vector<string>()));
    // modify last parameter when attr_names are considered
    return result; 
}

shared_ptr<virtual_table> database::selection(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];

    if(!table1->equal_tableSchema(table2)){
        // different table schema
        return shared_ptr<virtual_table>(NULL);
    }
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    vector<vector<void *> > inter_data;
    vector<char> inter_types = table1->get_types();
    vector<void *> temp1;
    vector<void *> temp2;
    for(int i = 0; i < height1; i++){

        temp1 = table1->get_tuple(i);
        for(int j = 0; j < height2; j++){
            
            temp2 = table2->get_tuple(j);
            if(equal_tuple(temp1, temp2, inter_types)){
                inter_data.push_back(temp1);
            }
        }
    }
    cout<<inter_data.size()<<" "<<inter_data[0].size()<<endl;
    
    shared_ptr<virtual_table> result(new virtual_table(inter_data, inter_types, new_table_name, vector<string>()));
    // modify last parameter when attr_names are considered
    return result; 
}

vector<string> get_sources(hsql::TableRef* table){
    vector<string> result;
    switch(table->type){
      case hsql::TableRefType::kTableName:
        //wait to be implemented
        break;
      case hsql::TableRefType::kTableSelect:
        result.push_back(string(table->name));
        break;
      case hsql::TableRefType::kTableJoin:
        //wait to be implemented
        break;
      case hsql::TableRefType::kTableCrossProduct:
        for (hsql::TableRef* tbl : *table->list) result.push_back(string(tbl->name));
        break;
    }
    return result;
  }


// @intput: query string, pointer to result
// @output: 0 if successful, -1 if unsuccessful
int database::execute_query(const string& query, shared_ptr<virtual_table>* res){
    // execute a actual SQL query
    /*
        stage 1: support single SELECT-FROM-WHERE clause
    */
   hsql::SQLParserResult result;
   hsql::SQLParser::parse(query, &result);

   if(result.isValid()){
       cout<<result.size()<<endl;
       if(result.size() != 1){
           cout<<"Please don't input multiple queires.";
           return -1;
       }else{
            const hsql::SQLStatement* query = result.getStatement(0);
            if(!query->isType(hsql::StatementType::kStmtSelect)){
                cout<<"Only Support SELECT-FROM-WHERE query. Try again.";
                return -1;
            }else{
                if(((const hsql::SelectStatement*)query)->fromTable == NULL){
                    cout<<"NULL!!"<<endl;
                    return -1;
                }
                cout<<((const hsql::SelectStatement*)query)->fromTable->type<<endl;
                vector<string> tables = get_sources(((const hsql::SelectStatement*)query)->fromTable);
                cout<<"out"<<endl;
                // print all the source table names
                for(string s : tables){
                    cout<<s<<endl;
                }
            }
       }
   }else{
       cout<<"invalid query!"<<endl;
       return -1;
   }
}