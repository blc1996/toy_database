#include "database.h"

// constructor
database::database(){
    _store = map<string, table*> ();
    _num_of_tables = 0;
}

// destructor
database::~database(){
    for(auto iter = _store.begin(); iter != _store.end(); iter++){
        delete iter->second;
    }
}

void database::read_in_csv(string file_path){
    table* new_table = NULL;
    try{
        new_table = new table(file_path);
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

void database::display_table(string name){
    if(_store.count(name)){
        _store[name]->print();
    }else{
        cout<<"No such table!"<<endl;
    }
}