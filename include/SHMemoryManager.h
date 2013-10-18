/* 
 * File:   SHMemoryPool.h
 * Author: staszek
 *
 * Created on 7 sierpień 2012, 18:30
 */

#ifndef SHMEMORYPOOL_H
#define	SHMEMORYPOOL_H

#include <cstdlib> //std::system
#include <cstddef>

#include <map>
#include <exception>

#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/managed_heap_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/exception/exception.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
                

#include "Utils.h"
#include "MemoryAllocator.h"


/* @brief Shared memory manager.
 *
 * Allows for constructing named segments, destructing them, etc. 
 * 
 */
class SHMemoryManager
{
    std::map<std::string, boost::interprocess::managed_shared_memory*> m_segments;
    
    SHMemoryManager(const SHMemoryManager& orig){}
    
    std::string process_identifier;
public: 
    
    
    /**Create new SHMemoryManager object. 
     * @note This doesn't create first segment``` !
     */
    SHMemoryManager(std::string process_identifier);
 
    void * allocate(std::size_t, std::string segment_name);

    std::string getProcessIdentifier() const{
        return this->process_identifier;
    }

    /**Generate system segment name based on user segment name*/
    static std::string generate_name(std::string process_identifier, std::string segment_name){
        std::string name = "";
        name="SHMemoryPool_"+process_identifier;
        name+="_Segment"+segment_name;
        return name;        
    }
//  
//     /** Allocate named object in segment
//     * @param segment_name Name of the segment
//     * @param name Name of the object
//     * @param count Number of objects (1 for only one object)
//     */
//     template<class T>
//     T * allocate_named(std::string segment_name, std::string name, int count) {
//         void* ptr = m_segments[segment_name]->construct<T>(name.c_str())[count]();
//         if(!ptr)
//             throw BasicException("Memory allocation error:"); 
//         return ptr;
//     }





    /** Retrieve named constructed segment */
    boost::interprocess::managed_shared_memory * get_named_segment(std::string name){
        return m_segments[SHMemoryManager::generate_name(this->process_identifier, name)];
    }   




    /* Build new shared segment
    * @param name Name of the segment
    * @param min_size Number of bytes
    * 
    * Throws exception if shared memory fails (interprocess_exception)
    */
    void new_named_segment(std::string name,  std::size_t min_size){
        using namespace boost::interprocess;

    	std::string segment_name = SHMemoryManager::generate_name(this->process_identifier, name);

        //TODO: add throwing error that such segment exists (or flag overwrite
        //in function params)
    	boost::interprocess::shared_memory_object::remove(segment_name.c_str());

    	boost::interprocess::managed_shared_memory *  segment = new boost::interprocess::managed_shared_memory(create_only, segment_name.c_str(), min_size);

    	this->m_segments[segment_name] = segment; 
    }
   
    std::string get_name(std::string segment_name) const;  
    
    bool deallocate(void * ptr, std::string segment_name);   
    
    virtual ~SHMemoryManager(){
        #ifdef DEBUG
        dbg.push_back(10, "Removing segments");
        #endif            
 
        for(std::map<std::string, boost::interprocess::managed_shared_memory* >::iterator it = this->m_segments.begin();
            it!=this->m_segments.end();++it)
        {
            #ifdef DEBUG
            dbg.push_back(10, "Removing "+it->first);
            #endif            
            boost::interprocess::shared_memory_object::remove(it->first.c_str());
        }      
    }
private:

};


#endif	/* SHMEMORYPOOL_H */
