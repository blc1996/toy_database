#include "csv.h"

// Constructor function
// @input: path of csv file
table::table(string file_path){
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
    if(data.size() == 0){
        cout<<"empty table!"<<endl;
        throw IOException();
    }

    _col = data[0].size();
    _row = data.size();

    //get name of table and attributes
    cout<<"Please name the table:";
    _table_name = string();
    cin>>_table_name;
    cout<<"The assigned table name is:"<<_table_name<<endl;

    _attr_names = vector<string>(_col, "");
    for(int i = 0; i < _col; i++){
        cout<<"Please input the name of col "<<i<<":";
        cin>>_attr_names[i];
        cout<<"The assigned attribute name is:"<<_attr_names[i]<<endl;
    }

    _types = vector<char>(_col, 0);
    for(int i = 0; i < _col; i++){
        while(_types[i] == 0){
            cout<<"Please input the representing number for datatypes( 1=INT64 2=STR 3=FLOAT64 ) of column "<<i<<":";
            char temp = 0;
            cin>>temp;
            if(temp > '0' && temp < '3'){
                _types[i] = temp - '0';
            }else{
                cout<<"Please input a valid number. Try again!"<<endl;
            }
        }
    }

    //check if input datatype match
    cout<<"Check if all data types match..."<<endl;
    bool valid = true;
    int x = 0, y = 0;
    auto iter = parser.begin();
    for(y = 0; y < _row; y++){
        for(x = 0; x < _col; x++){
            switch(_types[x]){
                case INT64:
                    if(!is_int(data[y][x])){
                        valid = false;
                    }
                break;
                case FLOAT64:
                    if(!is_float(data[y][x])){
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
                case INT64:
                    _tuples[y][x] = new int;
                    *((int *)_tuples[y][x]) = stoi(data[y][x]);
                break;
                case STR:
                    _tuples[y][x] = new string(data[y][x]);
                break;
                case FLOAT64:
                    _tuples[y][x] = new float;
                    *((float *)_tuples[y][x]) = stof(data[y][x]);
                break;
            }
        }
    }
}

// destructor
table::~table(){
    for(int y = 0; y < _row; y++){
        for(int x = 0; x < _col; x++){
            switch(_types[x]){
                case INT64:
                    delete (int *)_tuples[y][x];
                break;
                case STR:
                    delete (string *)_tuples[y][x];
                break;
                case FLOAT64:
                    delete (float *)_tuples[y][x];
                break;
            }
        }
    }
}

// print the table
void table::print(){
    for(int y = 0; y < _row; y++){
        cout<<"Row "<<y<<": ";
        for(int x = 0; x < _col; x++){
            switch(_types[x]){
                case INT64:
                    cout<<*((int *)_tuples[y][x])<<" | ";
                break;
                case STR:
                    cout<<*((string *)_tuples[y][x])<<" | ";
                break;
                case FLOAT64:
                    cout<<*((float *)_tuples[y][x])<<" | ";
                break;
            }
        }
        cout<<endl;
    }
}

void* table::get_element(int y, int x){
    return _tuples[y][x];
}

vector<void *> table::get_tuple(int y){
    return _tuples[y];
}

vector<char> table::get_types(){
    return _types;
}

//check if the string can be converted to INT
bool table::is_int(const string& s){
    for(char c : s){
        if(!isnumber(c)){
            return false;
        }
    }
    return true;
}

//check if the string can be converted to FLOAT
bool table::is_float(const string& s){
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