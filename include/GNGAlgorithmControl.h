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

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

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


/**
 *
 * Class responsible for handling communication between main core and clients. It adds examples, pauses algorithm, stores references to
 * main mutexes, etc. It is used in server.
 * 
 * 
 */
class GNGAlgorithmControl {
public:
    bool m_pause; /**< Is the algorithm paused? */ 
    bool m_terminate; /**< Has the algorithm(server) terminated? */
    boost::interprocess::interprocess_mutex grow_mutex; /**< Mutex locked when the graph pool is expanded @see ExtGraphNodeManager */
    boost::interprocess::interprocess_mutex database_mutex;
    
    boost::mutex grow_mutex_thread; /**< Grow mutex used, when the graph is not stored in shared memory, but in local memory*/
    boost::mutex database_mutex_thread;
    
    boost::interprocess::interprocess_mutex m_pause_mutex;
    boost::interprocess::interprocess_condition m_pause_changed;

    //used gng_database, gng_graph, etc.
    
    void checkPause() {
        using namespace std;
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
    	shared_memory_object::remove("SHMemoryPool_Segment1"); //TODO: error with multiservers
    	shared_memory_object::remove("SHMemoryPool_Segment2");
    	m_terminate=true;
    }

};



#endif	/* GNGALGORITHMCONTROL_H */



