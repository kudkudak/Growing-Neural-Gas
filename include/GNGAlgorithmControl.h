/* 
 * File:   GNGAlgorithmControl.h
 * Author: staszek
 *
 * Created on September 2, 2012, 3:18 AM
 */


#ifndef GNGALGORITHMCONTROL_H
#define	GNGALGORITHMCONTROL_H

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "Utils.h"
typedef boost::interprocess::interprocess_mutex MyMutex;
struct GNGAlgorithmControl {
    bool m_pause;
    MyMutex grow_mutex,database_mutex;
    boost::interprocess::interprocess_mutex m_pause_mutex;
    boost::interprocess::interprocess_condition m_pause_changed;
    void checkPause() {
        boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(m_pause_mutex);

        while (m_pause) {
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
    
};



#endif	/* GNGALGORITHMCONTROL_H */



