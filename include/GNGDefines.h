/* 
 * File:   GNGDefines.h
 * Author: staszek
 *
 * Created on August 15, 2012, 6:37 AM
 */

#ifndef GNGDEFINES_H
#define	GNGDEFINES_H




typedef boost::interprocess::interprocess_mutex MyMutex;
typedef boost::interprocess::offset_ptr< SHGNGNode  > PoolPtr;
typedef boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> ScopedLock;


#endif	/* GNGDEFINES_H */

