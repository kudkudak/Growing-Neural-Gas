/* 
 * File:   GNGAlgorithmControl.h
 * Author: staszek
 *
 * Created on September 2, 2012, 3:18 AM
 */


#ifndef GNGALGORITHMCONTROL_H
#define	 GNGALGORITHMCONTROL_H

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>


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


#include <stdlib.h>
#include "Utils.h"
typedef boost::interprocess::interprocess_mutex MyMutex;
struct GNGAlgorithmControl {
    bool m_pause;
    bool m_terminate;
    MyMutex grow_mutex,database_mutex;
    boost::interprocess::interprocess_mutex m_pause_mutex;
    boost::interprocess::interprocess_condition m_pause_changed;
    void checkPause() {using namespace std;
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(m_pause_mutex);

        while (m_pause) {
        	if(m_terminate) exit(EXIT_SUCCESS);
            m_pause_changed.wait(lock);
        }
    }
    void setRunningStatus(bool new_value) {
        {
            boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(m_pause_mutex);
            m_pause = !new_value;
        }

        m_pause_changed.notify_all();
    }    
    
    void terminate(){
    	using namespace boost::interprocess;
    	shared_memory_object::remove("SHMemoryPool_Segment1");
    	shared_memory_object::remove("SHMemoryPool_Segment2");
    	m_terminate=true;
    }

};



#endif	/* GNGALGORITHMCONTROL_H */



