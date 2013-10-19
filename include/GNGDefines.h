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
        AddExamples, /**Interprocess:vector of GNGExample*/
        Request /**String, @note: for subset of request we can create more efficient communication protocol*/
    };

   
    int state;
    int type;
};

/** Filled struct to pass add example SHGNGMessage
 * @note All post message memory managment is held by server (if there is need of course)
 * 
 * count - number of examples
 * dim - number of dimensions for example
 * pointer_reference_name - pointer name to look up in shared memory
 */
struct SHGNGMessageAddExamples{
    int count;
    int dim;
    std::string pointer_reference_name;
};

#endif	/* GNGDEFINES_H */

