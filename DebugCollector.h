/* 
 * File:   DebugCollector.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 18:24
 */

#ifndef DEBUGCOLLECTOR_H
#define	DEBUGCOLLECTOR_H



#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define DEBUG_COLLECTOR_LEVEL 3
#define DEBUG_COLLECTOR_DEBUG

class DebugCollector {
    
    struct Record{
        std::string line;
        int level;
        Record(int level, std::string line):line(line),level(level)
        {}
        friend std::ostream & operator<<(std::ostream & out, const Record & r){ 
            std::string a = r.line;
            out<<a;
            return out;
        }
    };
    std::vector<Record> records; 
public:
    DebugCollector(){}
    std::string report(int cutting_level=0){
        using namespace std;
        stringstream ss;
        
        ss<<"DEBUG REPORT\n --------- \n";
        
        for(int i=0;i<(int)records.size();++i){
            if(records[i].level>=cutting_level) ss<<records[i].line<<std::endl;
        }
        
        
        
        return ss.str();
    }
    void push_back(int level, std::string line){
        #ifdef DEBUG_COLLECTOR_DEBUG
                if(level>= DEBUG_COLLECTOR_LEVEL) std::cout<<line<<std::endl;
        #endif
        //records.push_back(Record(level,line));
    }
private:

};

#endif	/* DEBUGCOLLECTOR_H */

