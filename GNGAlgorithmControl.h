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

struct GNGAlgorithmControl {
    bool m_pause;
   
    boost::interprocess::interprocess_mutex m_pause_mutex;
    boost::interprocess::interprocess_condition m_pause_changed;
};



#endif	/* GNGALGORITHMCONTROL_H */



