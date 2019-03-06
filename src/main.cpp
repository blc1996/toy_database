#include "table.h"
#include "database.h"
#include "sql_parser/SQLParser.h"
#include <iostream>
#include <string>

using namespace std;

int main(){
    // string file("Book3.csv");table test(file);
    // test.print();
    database test;
    while(1){
        int temp = 0;
        cout<<"input(1=read in a file, 2=show all table names, 3=show a specific table):";
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
            cout<<"read the table name:";
            string title;
            cin>>title;
            test.display_table(title);
        }
    }
}