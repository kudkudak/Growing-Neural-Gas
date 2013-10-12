/* 
 * File:   DebugCollector.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 18:24
 */

#ifndef DEBUGCOLLECTOR_H
#define DEBUGCOLLECTOR_H


#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define DEBUG_COLLECTOR_DEBUG
#define DEBUG_ERROR_LEVEL 5

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
    int m_debug_level;
public:
    DebugCollector(int debug_level=0): m_debug_level(debug_level){}
    std::string report(int cutting_level=0){
        using namespace std;
        stringstream ss;
        
        ss<<"DEBUG REPORT\n --------- \n";
        
        for(int i=0;i<(int)records.size();++i){
            if(records[i].level>=cutting_level) ss<<records[i].line<<std::endl;
        }

        return ss.str();
    }

    void set_debug_level(int debug_level){ m_debug_level = debug_level; }
    void push_back(int level, std::string line){
                if(level>= m_debug_level) std::cout<<line<<std::endl<<std::flush;
    }
private:

};

#endif	/* DEBUGCOLLECTOR_H */

