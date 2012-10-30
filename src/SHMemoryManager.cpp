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
    string name = "";
    name="SHMemoryPool";
    name+="_Segment"+to_string<int>(COUNTER);
    return name;
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
SHMemoryManager::SHMemoryManager(std::size_t target_size) {
    ++COUNTER;

    string name = generate_name();
    m_names.push_back(name);
    shared_memory_object::remove("SHMemoryPool");
    shared_memory_object::remove(name.c_str());
    m_current_segment = new managed_shared_memory(create_only, name.c_str(), target_size);
    
    m_segments.push_back(m_current_segment); 
}


void SHMemoryManager::new_segment(std::size_t min_size) {
    ++COUNTER;
   
    string name = generate_name();
    m_names.push_back(name);


    shared_memory_object::remove(name.c_str());
    m_current_segment = new managed_shared_memory(create_only, name.c_str(), min_size);

    
    m_segments.push_back(m_current_segment); 
}







