#include "database.h"
#include "sql_parser/sql/Expr.h"

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

shared_ptr<table> database::get_table(const string& table_name){
    if(_store.count(table_name)){
        return _store[table_name];
    }else{
        return shared_ptr<table>(NULL);
    }
}


void database::RenameTable (const string& tableIn, const string new_table_name) {
    // need to check if there are duplicate names!
    // TO BE IMPLEMENTED!!

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

shared_ptr<table> database::projection(const string& tableIn, vector<string> column_names) {
    if(_store.count(tableIn)){
        return projection(_store[tableIn], column_names);
    }else{
        cout<<"Projection: No such table name"<<endl;
        return NULL;
    }
}

shared_ptr<table> database::projection(shared_ptr<table> table, vector<string> column_names){
    // Now we have accessed the indexes of those columns which have to be projected
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
                // cout<<"attr index: "<<j<<endl;
                types_we_will_need.push_back(original_table_types[j]);
            } 
        }
    }

    // Now we have accessed the indexes of those columns which have to be projected
    int table_width = list_of_index_of_cols.size();
    vector<vector<void *> > joined_data(table_height, vector<void*>(table_width, NULL));
    for (int k = 0; k < table_width; k++) {
        for (int l = 0; l < table_height; l++) {
            joined_data[l][k] = table->get_element(l, list_of_index_of_cols[k]);
        }
    }
    shared_ptr<virtual_table> result(new virtual_table(joined_data, types_we_will_need, "", column_names));
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
            case INT32:
                if(*(int *)tuple1[i] != *(int *)tuple2[i]){
                    return false;
                }
            break;
            case STR:
                if(*(string *)tuple1[i] != *(string *)tuple2[i]){
                    return false;
                }
            break;
            case DOUBLE64:
                if(*(double *)tuple1[i] != *(double *)tuple2[i]){
                    return false;
                }
            break;
        }
       
    }
    return true;
}

shared_ptr<table> database::simple_join(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    return simple_join(table1, table2, new_table_name);
}

shared_ptr<table> database::simple_join(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name){
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

shared_ptr<table> database::simple_union(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    return simple_union(table1, table2, new_table_name);
}

shared_ptr<table> database::simple_union(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name){
    if(!table1->equal_tableSchema(table2)){
        // different table schema
        return shared_ptr<virtual_table>(NULL);
    }
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
                    case INT32:
                        if(*(int *)temp2[k] == *(int *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case STR:
                        if(*(string *)temp2[k] == *(string *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case DOUBLE64:
                        if(*(double *)temp2[k] == *(double *)temp1[k]){
                            sign += 1;
                        }
                    break;
                }
            }
            if(sign == width){
                flag = 1;
            }
        }
        if(flag == 0){
            union_data.push_back(temp2);
        }
    }
    shared_ptr<virtual_table> result(new virtual_table(union_data, table_types, new_table_name, table1->get_attr_names()));
    // modify last parameter when attr_names are considered
    return result; 
}

shared_ptr<table> database::simple_diff(const string& table_name_1, const string& table_name_2, const string& new_table_name){
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
    for(int i = 0; i < height1; i++){
        int flag = 0;
        vector<void *> temp1 = table1->get_tuple(i);
        for(int j = 0; j < height2; j++){
            int sign = 0;
            vector<void *> temp2 = table2->get_tuple(j);
            for(int k = 0; k < width; k++){
                switch(table_types[k]){
                    case INT32:
                        if(*(int *)temp2[k] == *(int *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case STR:
                        if(*(string *)temp2[k] == *(string *)temp1[k]){
                            sign += 1;
                        }
                    break;
                    case DOUBLE64:
                        if(*(double *)temp2[k] == *(double *)temp1[k]){
                            sign += 1;
                        }
                    break;
                }
            }
            if(sign == width){
                flag = 1;
            }
        }
        if(flag == 0){
            diff_data.push_back(temp1);
        }
    }
    shared_ptr<virtual_table> result(new virtual_table(diff_data, table_types, new_table_name, table1->get_attr_names()));
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

shared_ptr<table> database::simple_intersection(const string& table_name_1, const string& table_name_2, const string& new_table_name){
    if(_store.count(table_name_1) == 0 || _store.count(table_name_1) == 0){
        // invalid input, return NULL
        return shared_ptr<virtual_table>(NULL);
    }
    shared_ptr<table> table1 = _store[table_name_1];
    shared_ptr<table> table2 = _store[table_name_2];
    return simple_intersection(table1, table2, new_table_name);
}

shared_ptr<table> database::simple_intersection(shared_ptr<table> table1, shared_ptr<table> table2, const string& new_table_name){
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

    shared_ptr<virtual_table> result(new virtual_table(inter_data, inter_types, new_table_name, table1->get_attr_names()));
    // modify last parameter when attr_names are considered
    return result; 
}


vector<int> database::helper_selection(shared_ptr<table> table, hsql::Expr* expr){
    /*
        expr can be string literal!!!! Need to modify!!!!
    */

    if(expr != NULL && expr->type == hsql::ExprType::kExprOperator){
        switch(expr->opType){
            case hsql::OperatorType::kOpNone:

            break;
            // binary operator
            /*
            // to be implemented later
            */
            case hsql::OperatorType::kOpPlus:

            break;
            case hsql::OperatorType::kOpMinus:

            break;
            // arithmatic operator
            case hsql::OperatorType::kOpNotEquals:
            case hsql::OperatorType::kOpLess:
            case hsql::OperatorType::kOpLessEq:
            case hsql::OperatorType::kOpGreater:
            case hsql::OperatorType::kOpGreaterEq:
            case hsql::OperatorType::kOpEquals:{
                condition cond;
                cond.cur_op = expr->opType;
                hsql::Expr* expr1 = expr->expr;
                hsql::Expr* expr2 = expr->expr2;
                // deal with the first expr
                if(expr1->type == hsql::ExprType::kExprColumnRef){
                    cond.is_num1 = false;
                    cond.attr_name1 = string(expr1->table) + "." + string(expr1->name);
                }else{
                    cond.is_num1 = true;
                    if(expr1->type == hsql::ExprType::kExprLiteralFloat){
                        cond.is_double1 = true;
                        cond.num_double1 = expr1->fval;
                    }else{
                        cond.is_double1 = false;
                        cond.num_int1 = expr1->ival;
                    }
                }
                // deal with the second expr
                if(expr2->type == hsql::ExprType::kExprColumnRef){
                    cond.is_num2 = false;
                    cond.attr_name2 = string(expr2->table) + "." + string(expr2->name);
                }else{
                    cond.is_num2 = true;
                    if(expr2->type == hsql::ExprType::kExprLiteralFloat){
                        cond.is_double2 = true;
                        cond.num_double2 = expr2->fval;
                    }else{
                        cond.is_double2 = false;
                        cond.num_int2 = expr2->ival;
                    }
                }
                return helper_selection(table, cond);
                break;
            }
            // logic operator
            case hsql::OperatorType::kOpAnd:{
                vector<int> res1 = helper_selection(table, expr->expr);
                vector<int> res2 = helper_selection(table, expr->expr2);
                vector<int> v(res1.size());
                auto it = set_intersection(res1.begin(), res1.end(), res2.begin(), res2.end(), v.begin());
                v.resize(it - v.begin());
                return v;
                break;
            }
            case hsql::OperatorType::kOpOr:{
                vector<int> res1 = helper_selection(table, expr->expr);
                vector<int> res2 = helper_selection(table, expr->expr2);
                vector<int> v(res1.size());
                auto it = set_union(res1.begin(), res1.end(), res2.begin(), res2.end(), v.begin());
                v.resize(it - v.begin());
                return v;
                break;
            }
            default:{
                cout<<"Selection: Operator not supported yet"<<endl;
                break;
            }
        }
    }
    return vector<int>();
}

vector<int> database::helper_selection(shared_ptr<table> table, condition cond){
    int height = table->get_height();
    vector<string> attributes = table->get_attr_names();
    vector<char> types = table->get_types();
    // arrange the data we need in advance
    int index1, index2;
    double num1, num2;
    //check if the attribute1 exist in the table
    if(!cond.is_num1){
        auto ret = find(attributes.begin(), attributes.end(), cond.attr_name1);
        if(ret == attributes.end()){
            cout<<"Selection: invalid condition!"<<endl;
            return vector<int>();
        }
        index1 = ret - attributes.begin();
    }else if(!cond.is_double1){
        num1 = cond.num_int1;
    }else{
        num1 = cond.num_double1;
    }
    //check if the attribute2 exist in the table
    if(!cond.is_num2){
        auto ret = find(attributes.begin(), attributes.end(), cond.attr_name2);
        if(ret == attributes.end()){
            cout<<"Selection: invalid condition!"<<endl;
            return vector<int>();
        }
        index2 = ret - attributes.begin();
    }else if(!cond.is_double2){
        num2 = cond.num_int2;
    }else{
        num2 = cond.num_double2;
    }

    // type check
    if(!cond.is_num1 && !cond.is_num2 && types[index1] != types[index2]){
        cout<<"Selection: invalid condition!"<<endl;
        return vector<int>();
    }

    vector<int> selected_index;
    for(int i = 0; i < height; i++){
        bool flag = false;
        auto cur_tuple = table->get_tuple(i);
        if(!cond.is_num1 && !cond.is_num2){
            switch(types[index1]){
                case INT32:
                    flag = operator_helper(*(int *)cur_tuple[index1], *(int *)cur_tuple[index2], cond.cur_op);
                    break;
                case STR:
                    flag = operator_helper(*(string *)cur_tuple[index1], *(string *)cur_tuple[index2], cond.cur_op);
                    break;
                case DOUBLE64:
                    flag = operator_helper(*(double *)cur_tuple[index1], *(double *)cur_tuple[index2], cond.cur_op);
                    break;
            }
        }else if(!cond.is_num1 && cond.is_num2){
            switch(types[index1]){
                case INT32:
                    flag = operator_helper(*(int *)cur_tuple[index1], (int)num2, cond.cur_op);
                    break;
                case DOUBLE64:
                    flag = operator_helper(*(double *)cur_tuple[index1], num2, cond.cur_op);
                    break;
            }
        }else if(cond.is_num1 && !cond.is_num2){
            switch(types[index2]){
                case INT32:
                    flag = operator_helper((int)num1, *(int *)cur_tuple[index2], cond.cur_op);
                    break;
                case DOUBLE64:
                    flag = operator_helper(num1, *(double *)cur_tuple[index2], cond.cur_op);
                    break;
            }
        }else{
            flag = operator_helper(num1, num2, cond.cur_op);
        }
        if(flag){
            selected_index.push_back(i);
        }
    }
    
    return selected_index;
}

template <typename T>
bool database::operator_helper(T data1, T data2, hsql::OperatorType op){
    switch(op){
        case hsql::OperatorType::kOpNotEquals:{
            return data1 != data2;
        }
        case hsql::OperatorType::kOpLess:{
            return data1 < data2;
        }
        case hsql::OperatorType::kOpLessEq:{
            return data1 <= data2;
        }
        case hsql::OperatorType::kOpGreater:{
            return data1 > data2;
        }
        case hsql::OperatorType::kOpGreaterEq:{
            return data2 >= data2;
        }
        case hsql::OperatorType::kOpEquals:{
            return data1 == data2;
        }
        default:{
            return false;
        }
    }
}

shared_ptr<table> database::simple_selection(shared_ptr<table> table, hsql::Expr* expr){
    auto index = helper_selection(table, expr);
    vector<vector<void *>> selected_data;
    for(int i : index){
        selected_data.push_back(table->get_tuple(i));
    }
    shared_ptr<virtual_table> result(new virtual_table(selected_data, table->get_types(), table->get_table_name(), table->get_attr_names()));
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
                // cout<<((const hsql::SelectStatement*)query)->fromTable->type<<endl;
                vector<string> tables = get_sources(((const hsql::SelectStatement*)query)->fromTable);
                // join all the tables together to get the datastore of this query
                shared_ptr<table> query_store;
                if(_store.count(tables[0])){
                    query_store = _store[tables[0]];
                }else{
                    cout<<"invalid input source!"<<endl;
                    return -1;
                }

                // need to record how many attributes each table have
                vector<int> num_attr(tables.size(), tables[0].size());
                // also need to record the name of each attribute
                vector<string> name_attr;
                auto temp_attr_names = _store[tables[0]]->get_attr_names();
                for(auto s : temp_attr_names){
                    // will look like "table.attr_name"
                    name_attr.push_back(tables[0]+"."+s);
                }
                for(int i = 1; i < tables.size(); i++){
                    if(_store.count(tables[i])){
                        query_store = simple_join(query_store, _store[tables[i]], "");
                        num_attr[i] = tables[i].size();
                        auto temp_attr_names = _store[tables[i]]->get_attr_names();
                        for(auto s : temp_attr_names){
                            // will look like "table.attr_name"
                            name_attr.push_back(tables[i]+"."+s);
                        }
                    }else{
                        cout<<"invalid input source!"<<endl;
                        return -1;
                    }
                }
                query_store->set_attr_names(name_attr);
                query_store->print();
                cout<<endl;

                // /*
                //     WHERE clause to be implemented
                // */
                // hsql::Expr* whereClause = ((const hsql::SelectStatement*)query)->whereClause;
                // query_store = simple_selection(query_store, whereClause);
                // query_store->print();
                // cout<<endl;

                // move to SELECT clause
                vector<hsql::Expr*>* selectList =((const hsql::SelectStatement*)query)->selectList;
                vector<string> attrs_needed;
                for(auto s : *selectList){
                    // s->name is the seleted attribute name
                    attrs_needed.push_back(string(s->table)+'.'+string(s->name));
                }
                auto result = projection(query_store, attrs_needed);
                result->print();
            }
       }
   }else{
       cout<<"invalid query!"<<endl;
       return -1;
   }
   return 0;
}