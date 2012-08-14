/* 
 * File:   SHMemoryPool.h
 * Author: staszek
 *
 * Created on 7 sierpień 2012, 18:30
 */


/*
 * TODO:
 * - dopisac niszczenie i zarzadzanie pamiecia
 */

#ifndef SHMEMORYPOOL_H
#define	SHMEMORYPOOL_H

#include <cstdlib> //std::system
#include <cstddef>


#include <string>
#include <list>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>



#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/exception/exception.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
                

#include "Utils.h"
#include "ExtMemoryManager.h"

//narazie statyczna wersja//
class SHMemoryManager:public ExtMemoryManager {
    static int COUNTER;
    static const int SHMEMORYPOOL_DEFAULT_SIZE=10000;
    
    
    std::vector<std::string> m_names; 
    
    std::vector<boost::interprocess::managed_shared_memory*> m_segments;
    boost::interprocess::managed_shared_memory * m_current_segment;
    
    SHMemoryManager(const SHMemoryManager& orig){}
    
    std::string generate_name();
public:
    
    void new_segment(std::size_t min_size);
    
    SHMemoryManager(std::size_t target_size=-1);
    
    /*
     * Returns new pointer
     */

 
    void * allocate(std::size_t,int index=0);
    
    //default constructor will be used
    template<class T>
    T * allocate_named(std::string name, int count,int index=0);
    
    boost::interprocess::managed_shared_memory * get_segment(int index=0){
        return m_segments[index];
    }

    std::string get_name(int index=0) const;
    
    
    bool deallocate(void * ptr,int index =0);
    virtual ~SHMemoryManager(){
        
        FOREACH(seg,m_names) boost::interprocess::shared_memory_object::remove(seg->c_str());
    }
private:

};






//note: named segments cannot be expanded!
template<class T>
T * SHMemoryManager::allocate_named(std::string name, int count,int index){
    
    void* ptr =m_segments[index]->construct<T>(name.c_str())[count]();
    
    //error check
    
    return ptr;

}


#endif	/* SHMEMORYPOOL_H */

