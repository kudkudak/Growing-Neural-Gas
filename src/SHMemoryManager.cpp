/* 
 * File:   SHMemoryPool.cpp
 * Author: staszek
 * 
 * Created on 7 sierpień 2012, 18:30
 */

#include "SHMemoryManager.h"


using namespace std;
using namespace boost::interprocess;

int SHMemoryManager::COUNTER = 0;

string SHMemoryManager::generate_name(){
    return SHMemoryManager::generate_name(this->process_identifier, SHMemoryManager::COUNTER);
}
string SHMemoryManager::get_name(int index) const{
    return m_names[index];
}

void * SHMemoryManager::allocate(std::size_t size,int index) {
    void * ptr =static_cast<void*>
            (m_segments[index]->allocate(size));

    if (!ptr) {
        std::cout << "Memory allocation error";
         exit(1);
    }
    return ptr;
}


bool SHMemoryManager::deallocate(void * ptr,int index) {
    m_segments[index]->deallocate(ptr);
    return true;
}
SHMemoryManager::SHMemoryManager(std::string process_identifier = "Server0", std::size_t target_size):
process_identifier(process_identifier){
    ++COUNTER;

    string name = generate_name();
    m_names.push_back(name);
    shared_memory_object::remove("SHMemoryPool");
    shared_memory_object::remove(name.c_str());
    m_current_segment = new managed_shared_memory(create_only, name.c_str(), target_size);
    
    m_segments.push_back(m_current_segment); 
    
    
    #ifdef DEBUG
        dbg.push_back(10, "SHMemoryManager::SHMemoryManager object creation success");
    #endif    
}


void SHMemoryManager::new_segment(std::size_t min_size) {
    ++COUNTER;
   
    string name = generate_name();
    m_names.push_back(name);


    shared_memory_object::remove(name.c_str());
    m_current_segment = new managed_shared_memory(create_only, name.c_str(), min_size);

    
    m_segments.push_back(m_current_segment); 
}






