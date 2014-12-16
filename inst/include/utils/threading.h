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

#include "utils/tinythread.h" //TODO: path problems
#include "utils/fast_mutex.h"

namespace gmum{
	//Seems to be very slow
	typedef tthread::recursive_mutex recursive_mutex;
	typedef tthread::fast_mutex fast_mutex;

	template<class Mutex>
	class scoped_lock{
		Mutex * m_mutex;
	public:
		scoped_lock(Mutex & mutex): m_mutex(&mutex){
			m_mutex->lock();
		}

		scoped_lock(Mutex * mutex): m_mutex(mutex){
			m_mutex->lock();
		}
		~scoped_lock(){
			m_mutex->unlock();
		}
	};

	typedef tthread::thread gmum_thread ;
	typedef tthread::condition_variable gmum_condition;
	///sleep current thread (equivalent to boost::this_thread::sleep)
	void sleep(int ms);
}

#endif
