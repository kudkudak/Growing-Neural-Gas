/* 
 * File:   ExtMemoryManager.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 15:58
 */

#ifndef EXTMEMORYMANAGER_H
#define	 EXTMEMORYMANAGER_H
#include <cstddef>
#include <string>

#include "Utils.h"

//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/noncopyable.hpp>
//#include <boost/interprocess/managed_shared_memory.hpp>
//#include <boost/interprocess/managed_heap_memory.hpp>
//#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/mapped_region.hpp>
//#include <boost/interprocess/sync/scoped_lock.hpp>
//#include <boost/exception/exception.hpp>
//#include <boost/uuid/uuid_generators.hpp>
//#include <boost/uuid/uuid_io.hpp>
//
namespace gmum{
	/** Interface abstracting away implementation of allocation.
	 */
	class MemoryAllocator{
	public:
		virtual void * allocate(std::size_t)=0;
		virtual bool deallocate(void * ptr)=0;
		virtual ~MemoryAllocator(){}
	};


	class BoostSHMemoryAllocator : public MemoryAllocator{
		BoostSHMemoryAllocator(){}
	//    size_t allocated;
		boost::interprocess::managed_shared_memory * msm_ptr;
	public:
		BoostSHMemoryAllocator(boost::interprocess::managed_shared_memory * msm_ptr): msm_ptr(msm_ptr){

		}
		void * allocate(std::size_t size) {
			void * ptr =static_cast<void*>
					(msm_ptr->allocate(size));
			if (!ptr)
				throw BasicException("Memory allocation error");
			return ptr;
		}

		bool deallocate(void * ptr) {
			msm_ptr->deallocate(ptr);
			//TODO: any error handling ? why no documentation..
			return true;
		}

	};
}

#endif	/* EXTMEMORYMANAGER_H */

