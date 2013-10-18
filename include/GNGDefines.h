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


/** Carries message about message to look for in shared memory used for interprocess communication
 * @Note : We are using shared memory for interprocess communication, not message queue, because storing objects in shared memory is quite convenient
 */
struct SHGNGMessage{
    enum State{
        Waiting,
        Processed
    };
    enum Type{
        GNGExample, /**Interprocess:vector of GNGExample*/
        GNGConfiguration, /**Struct GNGConfiguration*/
        Request /**String, @note: for subset of request we can create more efficient communication protocol*/
    };
};

#endif	/* GNGDEFINES_H */

