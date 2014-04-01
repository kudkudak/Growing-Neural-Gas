/*
* File: SHMemoryPool.cpp
* Author: Stanislaw "kudkudak" Jastrzebski
*
* Created on 7 sierpień 2012, 18:30
*/

#include "SHMemoryManager.h"
using namespace std;
using namespace boost::interprocess;

bool SHMemoryManager::deallocate(void * ptr, std::string segment_name) {
    //TODO: error handling
    m_segments[segment_name]->deallocate(ptr);
    m_segments.erase(m_segments.find(segment_name));
    return true;
}

SHMemoryManager::SHMemoryManager(std::string process_identifier = "Server0"):
process_identifier(process_identifier){
    #ifdef DEBUG
        dbg.push_back(10, "SHMemoryManager::SHMemoryManager object creation success of "+process_identifier);
    #endif
}