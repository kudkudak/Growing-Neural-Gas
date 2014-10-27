/*
 * File:   threading_wrapper.h
 * Author: staszek
 *
 * Created on June 6, 2014, 6:37 AM
 *
 * Contains code wrapping threading independently of platform. Currently using tinythread
 */

#ifndef THREADING_H
#define THREADING_H

#include "../tinythread/tinythread.h" //TODO: path problems

namespace gmum{
	///recursive mutex providing interface as boost::recursive_mutex
	typedef tthread::recursive_mutex gmum_recursive_mutex;
	typedef tthread::thread gmum_thread ;
	typedef tthread::condition_variable gmum_condition;
	///sleep current thread (equivalent to boost::this_thread::sleep)
	void sleep(int ms);
}

#endif
