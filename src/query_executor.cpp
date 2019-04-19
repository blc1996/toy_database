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
                case hsql::StatementType::kStmtUpdate:
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
    query_store->print();
    cout<<endl;

    /*
        deal with WHERE clause in the most stupid way
    */
    hsql::Expr* whereClause = ((const hsql::SelectStatement*)query)->whereClause;
    query_store = db->simple_selection(query_store, whereClause);
    // query_store->print();
    // cout<<endl;

    // move to SELECT clause
    vector<hsql::Expr*>* selectList =((const hsql::SelectStatement*)query)->selectList;
    vector<string> attrs_needed;
    for(auto s : *selectList){
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
    query_store->print();
    // cout<<endl;

    /*
        deal with WHERE clause in the most stupid way
    */
    hsql::Expr* expr = ((const hsql::DeleteStatement*)query)->expr;
    query_store = db->simple_selection(query_store, expr);
    query_store->print();
    cout<<endl;

    // move to SELECT clause
    // vector<hsql::Expr*>* selectList =((const hsql::SelectStatement*)query)->selectList;
    // vector<string> attrs_needed;
    // for(auto s : *selectList){
    //     // s->name is the seleted attribute name
    //     attrs_needed.push_back(string(s->table)+'.'+string(s->name));
    // }
    // auto result = db->projection(query_store, attrs_needed);
    // // result->print();

    // result_table = result;
    // indicate the execution has finished
    executed = true;
}