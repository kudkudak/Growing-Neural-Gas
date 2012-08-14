/* 
 * File:   test_client.cpp
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 08:25
 */

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "SHMemoryManager.h"
#include "SHGraphDefs.h"

#include "GNGGraph.h"
#include "GNGDatabase.h"
#include "GNGAlgorithm.h"


#include "ExtGraphNodeManager.h"
#include "ExtMemoryManager.h"

#include "DebugCollector.h"

#include<new>

DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;

typedef boost::interprocess::interprocess_mutex MyMutex;

//trzeba updatowac dosyc czesto
template<int N>
struct GNGGraphInfo{
    typedef offset_ptr< GNGNode<3>  > PoolPtr;
    
    
    int nodes;
    int pool_nodes;
    int first_free;
    PoolPtr * ptr;

    static GNGGraphInfo construct(SHMemoryManager *shm, GNGGraph<N> * graph){
        GNGGraphInfo ggi;
        
        typedef offset_ptr< GNGNode<3>  > PoolPtr;
        
        PoolPtr * poolPtr = shm->get_segment()->construct<PoolPtr>("SHPoolPtr")();
        *poolPtr = graph->getPool();
        
        ggi.ptr = poolPtr;
        ggi.nodes = graph->getNumberNodes();
        ggi.pool_nodes = graph->getPoolNodes();
        ggi.first_free = graph->getFirstFree();
        
        return ggi;
    }
};



int main(int argc, char** argv) { 
    typedef offset_ptr< GNGNode<3>  > PoolPtr;
    
    managed_shared_memory *  mshm=new managed_shared_memory(open_only,"SHMemoryPool");
    
   
    GNGGraphInfo<3> * ggi = mshm->find_or_construct<GNGGraphInfo<3> >("SHGraphInfo")();
    PoolPtr* ptr = mshm->find_or_construct< PoolPtr>("SHPoolPtr")();
    
    MyMutex * grow_mutex = mshm->find_or_construct< MyMutex >("grow_mutex")();
   //  long a;
    //while(1){(grow_mutex)->lock(); grow_mutex->unlock(); }
   // SHNode::mm = &mshm;
   // SHVector::mm = &mshm;
   // SHVector::alloc_inst = new ShmemAllocator(mshm->get_segment_manager());
    
    cout<<"read\n";
    
    while(1) {
        
        cout<<"zzz\n";
        
        grow_mutex->lock();
        
        
        ExtGraphNodeManager<GNGNode<3>, GNGEdge, GNGVector> egnm(&(*(*ptr)),ggi->nodes,ggi->pool_nodes,ggi->first_free);
        cout<<"done\n";
        egnm.reportPool();
       
        grow_mutex->unlock();
    }
    
    
    
    return 0;
}

