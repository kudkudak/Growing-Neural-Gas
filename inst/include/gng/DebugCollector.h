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


class Logger {    
public:
    static void log(int level, std::string line, int debug_level){
                if(level>= debug_level) std::cerr<<line<<std::endl<<std::flush;
    }
};


#endif	/* DEBUGCOLLECTOR_H */

