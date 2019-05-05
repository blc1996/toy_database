#include "table.h"
#include "database.h"
#include "sql_parser/SQLParser.h"
#include "b_plus_tree.cpp"
#include "query_executor.h"
#include <iostream>
#include <string>
#include <utility>

using namespace std;

void simple_interact(){
    database test;
    while(1){
        int temp = 0;
        cout<<"input(1=read in a file, 2=show all table names, 3=show a specific table, 4=input a SQL query):";
        cin>>temp;
        if(temp == 1){
            cout<<"read in a table name:";
            string title;
            cin>>title;
            try{
                test.read_in_csv(title);
            }catch(exception e){
                cout<<"No such file!"<<endl;
            }  
        }else if(temp == 2){
            test.display_all_table_name();
        }else if(temp == 3){
            test.display_all_table_name();
            cout<<"Please input a table name:";
            string title;
            cin>>title;
            test.display_table(title);
        }else if(temp == 4){
            // test with query executor here
            cout<<"Please input a SQL query:";
            cin.clear(); 
            cin.ignore(1000, '\n');
            string sql;
            getline(cin, sql);
            query_executor executor(&test, sql);
            executor.execute();
            shared_ptr<table> res = executor.get_result();
            if(res != NULL){
                res->print();
            }
        }else{
            cout<<"wrong input!!!"<<endl;
            cin.clear(); 
            cin.ignore(1000, '\n');
        }
    }
}

 vector< pair< int, int > > make_int_data(int n, bool random){
     vector< pair< int, int > > data;
     if(random)
     {
         for(int i = 0; i < n; i++)
         {
             int rand_val = rand();
             data.emplace_back(rand_val, rand_val);
         }
     }
     else
     {
         for(int i = 0; i < n; i++)
         {
             data.emplace_back(i, i);
         }
     }
     return data;
 }

void b_plus_tree_test(){
    vector< pair< int, int > > data = { {1, 5}, {4, 7}, {5, 43}, {-43, 3},
                                         {99, 2}, {23, 7}, {3, 2}, {9, 4} };
    // auto data = make_int_data(100, false);
    BPlusTree< int, int > b(3);
    // do_inserts(data, b);
    for(auto& key_val : data)
     {
         b.insert(key_val.first, key_val.second);
     }

    //  b.print();

    // verify_finds(data, b);
    for(auto& key_val : data)
     {
         auto ret = b.find(key_val.first);
         if(key_val.second != (*ret).second){
             cout<<"error:"<<key_val.first<<" "<<key_val.second<<" "<<(*ret).second<<endl;
         }
     }

     for(auto iter = b.begin(); iter != b.end(); iter++){
         cout<<(*iter).first<<" ";
     }

     cout<<(b.is_valid() ? 1 : 0)<<endl;
}

int main(){
    // simple_interact();
    // b_plus_tree_test();
    table t("B1.csv");
    t.write_to_disk();
}