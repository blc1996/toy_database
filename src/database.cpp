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


void database::RenameTable (const string& tableIn, const string new_table_name) {
    shared_ptr<table> table = _store[tableIn];
    _store.erase(tableIn);
    table->set_table_name(new_table_name);
    _store.insert({table->get_table_name(), table});
}

vector<string> database::display_current_attributeNames_of_a_table(const string& tableIn) {
    shared_ptr<table> table = _store[tableIn];
    vector<string> attr_names = table->get_attr_names();
    for (int i = 0; i < attr_names.size(); i++) {
        cout<<attr_names[i]<<endl;
    }
    return attr_names;
}

void database::RenameTableAttributes (const string& tableIn, vector<string> newAttrNames) {
    shared_ptr<table> table = _store[tableIn];
    _store.erase(tableIn);
    table->set_attr_names(newAttrNames);
    _store.insert({table->get_table_name(), table});
}

shared_ptr<virtual_table> database::projection(const string& tableIn, vector<string> column_names) {

    shared_ptr<table> table = _store[tableIn];
    int table_height = table->get_height();
    vector<string> attribute_names = table->get_attr_names();
    vector<int> list_of_index_of_cols;
    vector<char> original_table_types = table->get_types();
    vector<char> types_we_will_need;
    for (int i = 0; i < column_names.size(); i++){
        string name = column_names[i];
        for (int j = 0; j < attribute_names.size(); j++) {
            if(name == attribute_names[j]) {
                list_of_index_of_cols.push_back(j);
                cout<<"attr index: "<<j<<endl;
                types_we_will_need.push_back(original_table_types[j]);
            } 
        }
    }

    // Now we have accessed the indexes of those columns which have to be projected
    int table_width = list_of_index_of_cols.size();
    vector<vector<void *> > joined_data(table_height, vector<void*>(table_width, NULL));
    cout<<__LINE__<<endl;
    for (int k = 0; k < table_width; k++) {
        for (int l = 0; l < table_height; l++) {
            cout<<__LINE__<<endl;
            joined_data[l][k] = table->get_element(l, list_of_index_of_cols[k]);
        }
    }
    cout<<__LINE__<<endl;
    shared_ptr<virtual_table> result(new virtual_table(joined_data, types_we_will_need, "", column_names));
    cout<<__LINE__<<endl;
    return result; 
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
    return simple_join(table1, table2, new_table_name);
}

shared_ptr<virtual_table> database::simple_join(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name){
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    vector<vector<void *> > joined_data(height1 * height2, vector<void*>());
    for(int i = 0; i < height1; i++){
        for(int j = 0; j < height2; j++){
            vector<void *>& temp = joined_data[i * height1 + j];
            temp = table1->get_tuple(i);
            temp.insert(temp.end(), table2->get_tuple(j).begin(), table2->get_tuple(j).end());
        }
    }
    vector<char> joined_types = table1->get_types();
    joined_types.insert(joined_types.end(), table2->get_types().begin(), table2->get_types().end());
    shared_ptr<virtual_table> result(new virtual_table(joined_data, joined_types, new_table_name, vector<string>()));
    // modify last parameter when attr_names are considered
    return result; 
}

shared_ptr<virtual_table> database::simple_union(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    int width = table2->get_width();
    vector<char> table_types = table2->get_types();
    vector<vector<void *> >  union_data = table1->get_table_data();
    // cout<<width<<endl;
    for(int i = 0; i < height2; i++){
        int flag = 0;
        vector<void *> temp2 = table2->get_tuple(i);
        for(int j = 0; j < height1; j++){
            int sign = 0;
            vector<void *> temp1 = table1->get_tuple(j);
            for(int k = 0; k < width; k++){
                switch(table_types[k]){
                    case INT64:
                        if(*(int *)temp2[k] == *(int *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case STR:
                        if(*(string *)temp2[k] == *(string *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case FLOAT64:
                        if(*(float *)temp2[k] == *(float *)temp1[k]){
                            sign += 1;
                        }
                    break;
                }
            }
            cout<<flag<<endl;
            if(sign == width){
                flag = 1;
            }
        }
        if(flag == 0){
            union_data.push_back(temp2);
        }
    }
    shared_ptr<virtual_table> result(new virtual_table(union_data, table_types, new_table_name, vector<string>()));
    // modify last parameter when attr_names are considered
    return result; 
}

shared_ptr<virtual_table> database::simple_diff(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    int height1 = table1->get_height();
    int height2 = table2->get_height();
    int width = table2->get_width();
    vector<char> table_types = table1->get_types();
    vector<vector<void *> >  diff_data;
    // cout<<width<<endl;
    for(int i = 0; i < height1; i++){
        int flag = 0;
        vector<void *> temp1 = table1->get_tuple(i);
        for(int j = 0; j < height2; j++){
            int sign = 0;
            vector<void *> temp2 = table2->get_tuple(j);
            for(int k = 0; k < width; k++){
                switch(table_types[k]){
                    case INT64:
                        if(*(int *)temp2[k] == *(int *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case STR:
                        if(*(string *)temp2[k] == *(string *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case FLOAT64:
                        if(*(float *)temp2[k] == *(float *)temp1[k]){
                            sign += 1;
                        }
                    break;
                }
            }
            if(sign == width){
                flag = 1;
            }
            // cout<<flag<<endl;
        }
        // cout<<flag<<endl;
        if(flag == 0){
            // for(auto i : temp1){
            //     // cout<<*(string *)i<<endl;
            // }
            diff_data.push_back(temp1);
        }
    }
    shared_ptr<virtual_table> result(new virtual_table(diff_data, table_types, new_table_name, vector<string>()));
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


// @intput: query string, pointer to result
// @output: 0 if successful, -1 if unsuccessful
int database::execute_query(const string& query){
    // execute a actual SQL query
    /*
        stage 1: support single SELECT-FROM-WHERE clause
    */
   hsql::SQLParserResult result;
   hsql::SQLParser::parse(query, &result);

   if(result.isValid()){
       if(result.size() != 1){
           cout<<"Please don't input multiple queires at one time.";
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
                // print all the source table names
                for(string s : tables){
                    cout<<s<<endl;
                }
                // join all the tables together to get the datastore of this query
            }
       }
   }else{
       cout<<"invalid query!"<<endl;
       return -1;
   }
   return 0;
}