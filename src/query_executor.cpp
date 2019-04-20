#include "query_executor.h"

shared_ptr<table> query_executor::get_result(){
    if(executed){
        return result_table;
    }else{
        cout<<"Execute the query befor getting the result!"<<endl;
        return shared_ptr<table>(NULL);
    }
}

void query_executor::execute(){
    hsql::SQLParser::parse(sql_query, &parse_result);

    if(!executed && parse_result.isValid()){
       if(parse_result.size() != 1){
           // for now, only support one quey at a time. no subqueries.
           cout<<"Please don't input multiple queires at one time.";
       }else{
            const hsql::SQLStatement* query = parse_result.getStatement(0);
            switch(query->type()){
                case hsql::StatementType::kStmtSelect:
                    select_query();
                    break;
                case hsql::StatementType::kStmtDelete:
                    delete_query();
                    break;
                case hsql::StatementType::kStmtInsert:
                    insert_query();
                    break;
                case hsql::StatementType::kStmtUpdate:
                    update_query();
                    break;
                default:
                    break;
            }    
       }
   }else{
       cout<<"invalid query!"<<endl;
   }
}

vector<string> query_executor::get_sources(hsql::TableRef* table){
    vector<string> result;
    switch(table->type){
      case hsql::TableRefType::kTableName:
        //wait to be implemented
        result.push_back(string(table->name));
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

vector<string> query_executor::get_name(char* tableName){
    vector<string> result;
    result.push_back(string(tableName));
    return result;
}

void query_executor::select_query(){
    const hsql::SQLStatement* query = parse_result.getStatement(0);
    if(((const hsql::SelectStatement*)query)->fromTable == NULL){
        cout<<"NULL!!"<<endl;
        return;
    }
    // cout<<((const hsql::SelectStatement*)query)->fromTable->type<<endl;
    vector<string> tables = get_sources(((const hsql::SelectStatement*)query)->fromTable);
    // join all the tables together to get the datastore of this query
    shared_ptr<table> query_store;
    if(db->get_table(tables[0]) != NULL){
        query_store = shared_ptr<virtual_table>(new virtual_table(*db->get_table(tables[0])));
    }else{
        cout<<"FROM: invalid source!"<<endl;
        return;
    }
    // need to record how many attributes each table have
    vector<int> num_attr(tables.size(), tables[0].size());
    // also need to record the name of each attribute
    vector<string> name_attr;
    auto temp_attr_names = query_store->get_attr_names();
    for(auto s : temp_attr_names){
        // will look like "table.attr_name"
        name_attr.push_back(tables[0]+"."+s);
    }
    for(int i = 1; i < tables.size(); i++){
        if(db->get_table(tables[i]) != NULL){
            query_store = db->simple_join(query_store, db->get_table(tables[i]), "");
            num_attr[i] = tables[i].size();
            auto temp_attr_names = db->get_table(tables[i])->get_attr_names();
            for(auto s : temp_attr_names){
                // will look like "table.attr_name"
                name_attr.push_back(tables[i]+"."+s);
            }
        }else{
            cout<<"FROM: invalid source!"<<endl;
            return;
        }
    }
    query_store->set_attr_names(name_attr);
    // query_store->print();
    // cout<<endl;

    /*
        deal with WHERE clause in the most stupid way
    */
    hsql::Expr* whereClause = ((const hsql::SelectStatement*)query)->whereClause;
    if(whereClause != NULL){
        query_store = db->simple_selection(query_store, whereClause);
    }
    
    // query_store->print();
    // cout<<endl;

    // move to SELECT clause
    vector<hsql::Expr*>* selectList =((const hsql::SelectStatement*)query)->selectList;
    vector<string> attrs_needed;
    for(auto s : *selectList){
        if(s->type == hsql::ExprType::kExprStar){
            // deal with * sign
            attrs_needed.clear();
            for(string table : tables){
                auto cur_attributs = db->get_table(table)->get_attr_names();
                for(string attr : cur_attributs){
                    attrs_needed.push_back(table+'.'+attr);
                }
            }
            break;
        }
        // s->name is the seleted attribute name
        attrs_needed.push_back(string(s->table)+'.'+string(s->name));
    }
    auto result = db->projection(query_store, attrs_needed);
    // result->print();

    result_table = result;
    // indicate the execution has finished
    executed = true;
}


void query_executor::delete_query(){
    const hsql::SQLStatement* query = parse_result.getStatement(0);
    if(((const hsql::DeleteStatement*)query)->tableName == NULL){
        cout<<"NULL!!"<<endl;
        return;
    }
    // cout<<((const hsql::DeleteStatement*)query)->fromTable->type<<endl;
    vector<string> tables = get_name(((const hsql::DeleteStatement*)query)->tableName);
    // join all the tables together to get the datastore of this query
    
    shared_ptr<table> query_store;
    if(db->get_table(tables[0]) != NULL){
        query_store = shared_ptr<virtual_table>(new virtual_table(*db->get_table(tables[0])));
    }else{
        cout<<"FROM: invalid source!"<<endl;
        return;
    }
    // need to record how many attributes each table have
    vector<int> num_attr(tables.size(), tables[0].size());
    // also need to record the name of each attribute
    vector<string> name_attr;
    auto temp_attr_names = query_store->get_attr_names();
    for(auto s : temp_attr_names){
        // will look like "table.attr_name"
        name_attr.push_back(tables[0]+"."+s);
    }
    query_store->set_attr_names(name_attr);
    // query_store->print();
    // cout<<endl;

    /*
        deal with WHERE clause in the most stupid way
    */
    hsql::Expr* expr = ((const hsql::DeleteStatement*)query)->expr;
    // query_store = db->helper_selection(query_store, expr);
    vector<int> tuple_index = db->helper_selection(query_store, expr);

    db->get_table(tables[0])->delete_tuple(tuple_index);
    executed = true;
}

//INSERT INTO t1 (id, gpa, name) VALUES (4, 3.2, 'DD')
void query_executor::insert_query(){
    const hsql::SQLStatement* query = parse_result.getStatement(0);
    if(((hsql::InsertStatement*)query)->tableName == NULL) {
        cout<<"NULL table name"<<endl;
        return;
    }
    // cout<<"The name of the table to be inserted into is "<<((hsql::InsertStatement*)query)->tableName<<endl;
    string table_name = string(((hsql::InsertStatement*)query)->tableName);
    shared_ptr<table> table_to_be_inserted_into = db->get_table(table_name);
    if(table_to_be_inserted_into == NULL){
        cout<<"No such table found!"<<endl;
        return;
    }
    vector<hsql::Expr*>* values_to_be_inserted = (((hsql::InsertStatement*)query)->values);
    vector<void*> vector_to_be_passed_as_argument_to_table_class;
    vector<char> table_attr_types = table_to_be_inserted_into->get_types();

    for (int i = 0; i < values_to_be_inserted->size(); i++) {
        hsql::Expr* expr = values_to_be_inserted->at(i);
        // first we're checking for int
        if (expr->type == hsql::ExprType::kExprLiteralInt) {
            if (table_attr_types[i] != INT32) {
                cout<<"invalid type! 1"<<endl;
                return;
            }
            int* temp = new int;
            *temp = expr->ival;
            vector_to_be_passed_as_argument_to_table_class.push_back(temp);
        }
        else if (expr->type == hsql::ExprType::kExprLiteralFloat) {
            if (table_attr_types[i] != DOUBLE64) {
                cout<<"invalid type! 3"<<endl;
                return;
            }
            double* temp = new double;
            *temp = expr->fval;
            vector_to_be_passed_as_argument_to_table_class.push_back(temp);
        }
        else if (expr->type == hsql::ExprType::kExprLiteralString) {
            if (table_attr_types[i] != STR) {
                cout<<"invalid type! 2"<<endl;
                return;
            }
            string* temp = new string(expr->name);
            vector_to_be_passed_as_argument_to_table_class.push_back(temp);
        }
    } // end for
    table_to_be_inserted_into->insert_into_table(vector_to_be_passed_as_argument_to_table_class);

    executed = true;
}

void query_executor::update_query(){
    const hsql::SQLStatement* query = parse_result.getStatement(0);
    if(((const hsql::UpdateStatement*)query)->table == NULL){
        cout<<"NULL!!"<<endl;
        return;
    }
    // cout<<((const hsql::SelectStatement*)query)->fromTable->type<<endl;
    vector<string> tables = get_sources(((const hsql::UpdateStatement*)query)->table);
    // join all the tables together to get the datastore of this query

    shared_ptr<table> query_store;
    if(db->get_table(tables[0]) != NULL){
        query_store = shared_ptr<virtual_table>(new virtual_table(*db->get_table(tables[0])));
    }else{
        cout<<"FROM: invalid source!"<<endl;
        return;
    }
    // need to record how many attributes each table have
    vector<int> num_attr(tables.size(), tables[0].size());
    // also need to record the name of each attribute
    vector<string> name_attr;
    auto temp_attr_names = query_store->get_attr_names();
    for(auto s : temp_attr_names){
        // will look like "table.attr_name"
        name_attr.push_back(tables[0]+"."+s);
    }
    query_store->set_attr_names(name_attr);

    /*
        deal with WHERE clause in the most stupid way
    */
    hsql::Expr* whereClause = ((const hsql::UpdateStatement*)query)->where;
    if(whereClause != NULL){
        query_store = db->simple_selection(query_store, whereClause);
    }
    // query_store->print();
    // cout<<endl;

    // move to UPDATE clause
    vector<hsql::UpdateClause*>* updates =((const hsql::UpdateStatement*)query)->updates;
    vector<string> attrs_needed;

    for(auto u : *updates){
        // u->column is the seleted attribute name
        attrs_needed.push_back(string(tables[0]+"."+u->column));
    }

    query_store = db->projection(query_store, attrs_needed);
    //query_store->print();

    vector<void *> column = query_store->get_column(0);

    for (int i = 0; i < column.size(); ++i)
    {
        if ((*updates)[0]->value->type == hsql::ExprType::kExprLiteralInt) {
            // cout<< *(int*)column[i] << endl;
            *(int*)column[i] = (*updates)[0]->value->ival;
        
        }
        else if ((*updates)[0]->value->type == hsql::ExprType::kExprLiteralFloat) {
            *(double*)column[i] = (*updates)[0]->value->fval;
            
        }
        else if ((*updates)[0]->value->type == hsql::ExprType::kExprColumnRef) {
            *(string*)column[i] = (*updates)[0]->value->name;
        }
    }
    
    // indicate the execution has finished
    executed = true;
}