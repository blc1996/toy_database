#include "table.h"

// Constructor function
// @input: path of csv file
table::table(string file_path, bool write_to_disk_flag){
    
    ifstream f(file_path.c_str());
    aria::csv::CsvParser parser(f);

    // store everything in data
    vector<vector<string>> data;
    for(auto& row : parser){
        data.push_back(vector<string>());
        for(auto& field : row){
            data.back().push_back(field);
        }
    }
    if((int)data[0][0][0] == -17 && (int)data[0][0][1] == -69 && (int)data[0][0][2] == -65)
        data[0][0] = data[0][0].substr(3, data[0][0].size() - 1);
    // validate data
    if(data.size() < 2){
        cout<<__LINE__<<endl;
        cout<<"empty table!"<<endl;
        throw IOException();
    }else{
        _col = data[0].size();
        _row = data.size();
        for(int i = 1; i < _row; i++){
            if(_col != data[i].size()){
                cout<<"Some attributes in row "<<i<<" are missing!";
                throw IOException();
            }
        }
        _row -= 2;
    }

    //get name of table and attributes
    cout<<"Please name the table:";
    _table_name = string();
    cin>>_table_name;
    cout<<"The assigned table name is:"<<_table_name<<endl;


    // the first line is the name of each attribute
    // the second line is the type of each attribute

    _attr_names = vector<string>(_col, "");
    _types = vector<char>(_col, 0);
    for(int i = 0; i < _col; i++){
        _attr_names[i] = data[0][i];
        _types[i] = stoi(data[1][i]);
        if(_types[i] < 1 || _types[i] > 3){
            cout<<"No such type. Please check col "<<i<<" of row 2 of input file."<<endl;
            throw IOException();
        }
    }

    //check if input datatype match
    cout<<"Check if all data types match..."<<endl;
    bool valid = true;
    int x = 0, y = 0;
    for(y = 0; y < _row; y++){
        for(x = 0; x < _col; x++){
            switch(_types[x]){
                case INT32:
                    if(!is_int(data[y + 2][x])){
                        valid = false;
                    }
                break;
                case DOUBLE64:
                    if(!is_double(data[y + 2][x])){
                        valid = false;
                    }
                break;
            }
            if(!valid)
                break;
        }
        if(!valid)
            break;
    }
    if(valid){
        cout<<"All data types match!"<<endl;
    }else{
        cout<<"Invalid match at row "<<y<<" col "<<x<<". Abandon..."<<endl;
        throw IOException();
    }


    // store all the data into the tuple
    _tuples = vector<vector<void *>>(_row, vector<void *>(_col, NULL));
    for(y = 0; y < _row; y++){
        for(x = 0; x < _col; x++){
            switch(_types[x]){
                case INT32:
                    _tuples[y][x] = new int;
                    *((int *)_tuples[y][x]) = stoi(data[y + 2][x]);
                break;
                case STR:
                    _tuples[y][x] = new string(data[y + 2][x]);
                break;
                case DOUBLE64:
                    _tuples[y][x] = new double;
                    *((double *)_tuples[y][x]) = stof(data[y + 2][x]);
                break;
            }
        }
    }

    b_tree_index = new BPlusTree<int, long>();
    // write the table to disk if neccessary
    written_to_disk = false;
    if(true){
        write_to_disk();
    }
    
}

void table::delete_data(){
    for(int y = 0; y < _row; y++){
        for(int x = 0; x < _col; x++){
            switch(_types[x]){
                case INT32:
                    delete (int *)_tuples[y][x];
                break;
                case STR:
                    delete (string *)_tuples[y][x];
                break;
                case DOUBLE64:
                    delete (double *)_tuples[y][x];
                break;
            }
        }
    }
}

// destructor
table::~table(){
    // cout<<"inside base destructor "<<_row<<" "<<_col<<endl;
    if(!written_to_disk){
        delete_data();
    }
}

// print the table
void table::print(){
    auto attr_names = get_attr_names();
    for(auto s : attr_names){
        cout<<" "<<s<<" |";
    }
    cout<<endl;
    for(int y = 0; y < _row; y++){
        cout<<"Row "<<y<<": ";
        for(int x = 0; x < _col; x++){
            switch(_types[x]){
                case INT32:
                    cout<<*((int *)_tuples[y][x])<<" | ";
                break;
                case STR:
                    cout<<*((string *)_tuples[y][x])<<" | ";
                break;
                case DOUBLE64:
                    cout<<*((double *)_tuples[y][x])<<" | ";
                break;
            }
        }
        cout<<endl;
    }
}

void* table::get_element(int y, int x){
    return _tuples[y][x];
}

const vector<void *>& table::get_tuple(int y){
    return _tuples[y];
}

const vector<void *>& table::get_column(int x){
    for(int y = 0; y < _row; y++ ){
        column.push_back(_tuples[y][x]);
    }
    return column;
}

const vector<vector<void *>>& table::get_table_data(){
    return _tuples;
}

const vector<char>& table::get_types(){
    return _types;
}

//check if the string can be converted to INT
bool table::is_int(const string& s){
    for(char c : s){
        if(!isnumber(c)){
            cout<<c<<endl;
            return false;
        }
    }
    return true;
}

//check if the string can be converted to double
bool table::is_double(const string& s){
    int count = 0;
    for(char c : s){
        if(!isnumber(c)){
            if(count == 0 && c == '.'){
                count = 1;
                continue;
            }
            return false;
        }
    }
    return true;
}

string table::get_table_name(){
    return _table_name;
}

void table::set_table_name(string newName) {
    _table_name = newName;
}

const vector<string>& table::get_attr_names(){
    return _attr_names;
}

void table::set_attr_names(const vector<string>& newNames) {
    // for (int i = 0; i < _col; i++) {
    //     _attr_names[i] = newNames[i];
    // }
    _attr_names = newNames;
}

// identify whether two tables are equal
bool table::equal_tableSchema(shared_ptr<table> other){

    vector<char> type1 = this->get_types();
    vector<char> type2 = other->get_types();

    // if two tuples don't have same number of attribute   
    if(type1.size() != type2.size()){
        return false;
    }
    for(size_t i = 0; i < type1.size(); i++){
        if(type1[i] != type2[i]){
            return false;
        }        
    }
    return true;
}


void table::delete_tuple(const vector<int>& tuple_index){
    if(_tuples.size() > _row){
        cout<<"WRONG DELETE"<<endl;
    }else{
        for(int i = tuple_index.size() - 1; i >= 0; i--){
            vector<char> type = get_types();
            for(int j = 0; j < _col; j++){
                switch(type[i]){
                    case INT32:
                        delete (int *)_tuples[tuple_index[i]][j];
                    break;
                    case STR:
                        delete (string *)_tuples[tuple_index[i]][j];
                    break;
                    case DOUBLE64:
                        delete (double *)_tuples[tuple_index[i]][j];
                    break;
                }
            }
            _tuples.erase(_tuples.begin() + tuple_index[i]);
        }
        _row = _row - tuple_index.size();
    }
}


void table::insert_into_table (vector<void*> values_vector) {
    _tuples.push_back(values_vector);
    _row++;
}

void table::write_to_disk(){
    if(written_to_disk){
        cout<<"write_to_disk: table already written to disk!"<<endl;
        return;
    }
    if(_col == 0 || _row == 0){
        cout<<"write_to_disk: Table is empty"<<endl;
        return;
    }
    const char divider = 9;
    char path[100] = "./tables/";
    strcpy(path + 9, _table_name.c_str());
    FILE* out_file = fopen(path, "w");
    char buffer[10000];
    long counter = 0; // counter to the offset on each table file
    // check if we can use the first attribute as index
    if(_types[0] == INT32){
        use_first_attr_as_index = true;
    }else{
        use_first_attr_as_index = false;
    }
    for(int i = 0; i < _row; i++){
        if(use_first_attr_as_index){
            b_tree_index->insert(*(int *)_tuples[i][0], counter);
        }else{
            b_tree_index->insert(i, counter);
        }
        int size = 0;
        for(int j = 0; j < _col; j++){
            memcpy(buffer + size, &divider, 1);
            size++;
            memcpy(buffer + size, &_types[j], 1);
            size++;
            memcpy(buffer + size, &divider, 1);
            size++;
            switch(_types[j]){
                case INT32:
                    memcpy(buffer + size, _tuples[i][j], 4);
                    size += 4;
                    break;
                case STR:
                    memcpy(buffer + size, (*(string *)_tuples[i][j]).c_str(), (*(string *)_tuples[i][j]).size());
                    size += (*(string *)_tuples[i][j]).size();
                    break;
                case DOUBLE64:
                    memcpy(buffer + size, _tuples[i][j], 8);
                    size += 8;
                    break;
            }
        }
        buffer[size] = DIV;
        size++;
        buffer[size] = '\n';
        size++;
        fwrite(buffer, 1, size, out_file);
        counter += size;
    }
    fclose(out_file);

    written_to_disk = false;
    // delete_data();
    b_tree_index->print();
}

// dynamically allocate memory
// don't forget to delete
tuple_data table::decode_line(string line){
    string buffer;
    tuple_data cur_tuple;
    for(int i = 0; i < line.size(); i++){
        if(line[i] == DIV){
            // encounter the divider
            cur_tuple.type.push_back(line[i + 1]);
            switch(cur_tuple.type.back()){
                case INT32:
                    cur_tuple.dataInt.push_back(1);
                    memcpy(&cur_tuple.dataInt.back(), line.c_str() + i + 3, 4);
                    cur_tuple.dataIdx.push_back(cur_tuple.dataInt.size() - 1);
                    // cout<<*(int *)b.data.back()<<endl;
                    i += 6;
                    break;
                case DOUBLE64:
                    cur_tuple.dataDouble.push_back(1);
                    memcpy(&cur_tuple.dataDouble.back(), line.c_str() + i + 3, 8);
                    cur_tuple.dataIdx.push_back(cur_tuple.dataDouble.size() - 1);
                    // cout<<*(double *)b.data.back()<<endl;
                    i += 10;
                    break;
                case STR:
                    int count = 3;
                    buffer.clear();
                    while(line[i + count] != DIV){
                        buffer.push_back(line[i + count]);
                        count++;
                    }
                    // cout<<buffer<<endl;
                    cur_tuple.dataString.push_back(buffer);
                    cur_tuple.dataIdx.push_back(cur_tuple.dataString.size() - 1);
                    i += count + 2;
                    break;
            }
        }
    }
    return cur_tuple;
}