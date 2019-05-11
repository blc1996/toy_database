#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
using namespace std;

// local includes
#include "kwaymergesort.h"

#define DIV 9

extern int SORT_INDEX;

// a basic struct for a BED entry.
struct STU {
    unsigned int id;
    double gpa;
    
    bool operator < (const STU &b) const
    {
        if      (gpa < b.gpa)  return true;
        else if (gpa > b.gpa)  return false;
        // we get here when chroms are the same. now sort on starts
        if      (id < b.id)  return true;
        else if (id >= b.id) return false;
    }
    
    // overload the << operator for writing a STU struct
    friend ostream& operator<<(ostream &os, const STU &b) 
    {
        os  << b.id  << "\t" 
            << b.gpa;
        return os;
    }
    // overload the >> operator for reading into a STU struct    
    friend istream& operator>>(istream &is, STU &b) 
    {
        char temp;
        is  >> b.id>>temp >> b.gpa;
        return is;
    }    
};

struct Comparable {
    enum TYPE{
        INT = 1,
        STR,
        DOUBLE
    };
    vector<int> dataIdx;
    vector<int> dataInt;
    vector<double> dataDouble;
    vector<string> dataString;
    vector<char> type;
    int sortIndex;
    string originalLine;

    // ~Comparable(){
    //     for(int i = 0; i < data.size(); i++){
    //         switch(type[i]){
    //             case INT:
    //                 delete (int *)data[i];
    //                 break;
    //             case DOUBLE:
    //                 delete (double *)data[i];
    //                 break;
    //             case STR:
    //                 delete (string *)data[i];
    //                 break;
    //         }
    //     }
    // }

    // Comparable(const Comparable& b){
    //     sortIndex = b.sortIndex;
    //     type = b.type;
    //     data = vector<void*>();
    //     for(int i = 0; i < b.data.size(); i++){
    //         switch(type[i]){
    //             case INT:
    //                 data.push_back(new int);
    //                 *(int *)data.back() = *(int *)b.data[i];
    //                 break;
    //             case DOUBLE:
    //                 data.push_back(new double);
    //                 *(double *)data.back() = *(double *)b.data[i];
    //                 break;
    //             case STR:
    //                 data.push_back(new string(*(string *)b.data[i]));
    //                 break;
    //         }
    //     }
    // }

    Comparable(){
        sortIndex = SORT_INDEX;
    }

    bool operator < (const Comparable &b) const
    {
        // if(sortIndex < 0 || sortIndex >= type.size()){
        //     sortIndex = 0;
        // }
        int curIdx = dataIdx[sortIndex];
        switch(type[sortIndex]){
                case INT:
                    return b.dataInt[curIdx] > dataInt[curIdx];
                    break;
                case DOUBLE:
                    return b.dataDouble[curIdx] > dataDouble[curIdx];
                    break;
                case STR:
                    return b.dataString[curIdx] > dataString[curIdx];
                    break;
            }
        return false;
    }

    // overload the << operator for writing a Comparable struct
    friend ostream& operator<<(ostream &os, const Comparable &b) 
    {
        // for(int i = 0; i < b.type.size(); i++){
        //     int curIdx = b.dataIdx[i];
        //     switch(b.type[i]){
        //         case INT:
        //             os << b.dataInt[curIdx] << "\t";
        //             break;
        //         case DOUBLE:
        //             os << b.dataDouble[curIdx] << "\t";
        //             break;
        //         case STR:
        //             // cout<<" type: str"<<i<<endl;
        //             os << b.dataString[curIdx] << "\t";
        //             break;
        //     }
        // }
        os<<b.originalLine;
        // for(int j = 0; j < _col; j++){
        //     memcpy(buffer + size, &divider, 1);
        //     size++;
        //     memcpy(buffer + size, &_types[j], 1);
        //     size++;
        //     memcpy(buffer + size, &divider, 1);
        //     size++;
        //     switch(_types[j]){
        //         case INT32:
        //             memcpy(buffer + size, tuple[j], 4);
        //             size += 4;
        //             break;
        //         case STR:
        //             memcpy(buffer + size, (*(string *)tuple[j]).c_str(), (*(string *)tuple[j]).size());
        //             size += (*(string *)tuple[j]).size();
        //             break;
        //         case DOUBLE64:
        //             memcpy(buffer + size, tuple[j], 8);
        //             size += 8;
        //             break;
        //     }
        // }
        return os;
    }
    // overload the >> operator for reading into a STU struct    
    friend istream& operator>>(istream &is, Comparable &b) 
    {
        static int count = 0;
        // cout<<"iteration: "<<count++<<endl;
        string line;
        getline(is, line);
        b.originalLine = line;
        string buffer;
        b.type.clear();
        b.dataIdx.clear();
        b.dataInt.clear();
        b.dataDouble.clear();
        b.dataString.clear();
        for(int i = 0; i < line.size(); i++){
            if(line[i] == DIV){
                // encounter the divider
                b.type.push_back(line[i + 1]);
                // cout<<"index: "<<i<<" type: "<<(int)b.type.back()<<endl;
                switch(b.type.back()){
                    case INT:
                        b.dataInt.push_back(1);
                        memcpy(&b.dataInt.back(), line.c_str() + i + 3, 4);
                        b.dataIdx.push_back(b.dataInt.size() - 1);
                        // cout<<b.dataInt.back()<<endl;
                        i += 6;
                        break;
                    case DOUBLE:
                        b.dataDouble.push_back(1);
                        memcpy(&b.dataDouble.back(), line.c_str() + i + 3, 8);
                        b.dataIdx.push_back(b.dataDouble.size() - 1);
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
                        b.dataString.push_back(buffer);
                        b.dataIdx.push_back(b.dataString.size() - 1);
                        i += count - 1;
                        break;
                }
            }
        }
        return is;
    }  
};


void sort_index_change(int idx){
    SORT_INDEX = idx;
}


int main(int argc, char* argv[]) {

    string inFile       = argv[1];
    int  bufferSize     = 10000;      // allow the sorter to use 100Kb (base 10) of memory for sorting.  
                                       // once full, it will dump to a temp file and grab another chunk.     
    bool compressOutput = false;       // not yet supported
    string tempPath     = "";        // allows you to write the intermediate files anywhere you want.
    
    KwayMergeSort<Comparable> *COMPARABLE_sorter = new KwayMergeSort<Comparable> (inFile, 
                                                            &cout, 
                                                            bufferSize, 
                                                            compressOutput, 
                                                            tempPath);
                                                            
    cout << "First sort by chrom, then start using the overloaded \"<\" operator\n";
    sort_index_change(1);
    COMPARABLE_sorter->Sort();
}
