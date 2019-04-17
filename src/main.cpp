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
        cout<<"input(1=read in a file, 2=show all table names, 3=show a specific table, 4=join two tables, 6 = projection, 7 = Rename a table, 8 = Rename specific attributes of the table):";
        cin>>temp;
        if(temp == 0){
            cout<<"wrong input!!!"<<endl;
            cin.clear(); 
            cin.ignore(1000, '\n');
        }
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
            cout<<"join tables, please type two table names exactly:"<<endl;
            test.display_all_table_name();
            string a, b;
            cout<<"First table:";
            cin>>a;
            cout<<"Second table:";
            cin>>b;
            shared_ptr<table> result = test.simple_join(a, b, string("whatever"));
            cout<<result->get_height()<<" "<<result->get_width()<<endl;
            result->print();
        }else if(temp == 5){
            cout<<"Input a sql SELECT-FROM-WHERE:";
            cin.clear(); 
            cin.ignore(1000, '\n');
            string sql;
            getline(cin, sql);
            // cout<<sql<<endl;
            test.execute_query(sql);
        }else if(temp == 6){
            cout<<"union tables, please type two table names exactly:"<<endl;
            test.display_all_table_name();
            string a, b;
            cout<<"First table:";
            cin>>a;
            cout<<"Second table:";
            cin>>b;
            shared_ptr<table> result = test.simple_union(a, b, string("whatever"));
            // cout<<result->get_height()<<" "<<result->get_width()<<endl;
            result->print();
        }else if(temp == 7){
            cout<<"difference tables, please type two table names exactly:"<<endl;
            test.display_all_table_name();
            string a, b;
            cout<<"First table:";
            cin>>a;
            cout<<"Second table:";
            cin>>b;
            shared_ptr<table> result = test.simple_diff(a, b, string("whatever"));
            // cout<<result->get_height()<<" "<<result->get_width()<<endl;
            result->print();
        } else if (temp == 8) {
            string table_name;
            cout<<"Select Name of the table"<<endl;
            cin>>table_name;
            vector<string> attr_names;
            string colname;
            cout<<"Enter the name of column"<<endl;
            cin>>colname;
            attr_names.push_back(colname);
            shared_ptr<table> result = test.projection(table_name, attr_names);
            cout<<__LINE__<<endl;
            result->print();
        } else if (temp == 9) {
            string table_name;
            cout<<"Select Name of the table you want to change"<<endl;
            cin>>table_name;
            string newName;
            cout<<"Enter the new Name of the Table"<<endl;
            cin>>newName;
            test.RenameTable(table_name, newName);
        } else if (temp == 10) {
            string table_name;
            cout<<"Select the name of the table whose attribute names you want to change"<<endl;
            cin>>table_name;
            vector<string> current_list = test.display_current_attributeNames_of_a_table(table_name);
            vector<string> new_list;
            cout<<"Enter the new attribute names in the same order, if you don't wish to change a name, please type the same name"<<endl;
            for (int i = 0; i < current_list.size(); i++) {
                cout<<"The name of attribute is "<<current_list[i]<<" please enter the new name for this attribute, if you dont want to rename it, please enter the same name again"<<endl;
                string newName;
                cin>>newName;
                new_list.push_back(newName);
            }
            test.RenameTableAttributes(table_name, new_list);
<<<<<<< HEAD
        } else if (temp == 11) {
            cout<<"Input a sql INSERT INTO clause"<<endl;
=======
        }else if(temp == 11){
            // test with query executor here
>>>>>>> 74ac7ee7c6cb557f4f1fb5e8b9f1c2aa59ff4399
            cin.clear(); 
            cin.ignore(1000, '\n');
            string sql;
            getline(cin, sql);
<<<<<<< HEAD
            // cout<<sql<<endl;
            test.insert_query(sql);
=======
            query_executor executor(&test, sql);
            executor.execute();
            executor.get_result()->print();
>>>>>>> 74ac7ee7c6cb557f4f1fb5e8b9f1c2aa59ff4399
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
    simple_interact();
    // b_plus_tree_test();
}