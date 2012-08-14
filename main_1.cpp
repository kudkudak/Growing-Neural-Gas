/* 
 * File:   main.cpp
 * Author: staszek
 *
 * Created on 7 sierpień 2012, 18:27
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

#include "RcppInterface.h"

#include<new>

DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;



 typedef boost::interprocess::interprocess_mutex MyMutex;


GNGAlgorithm * gngAlgorithm;
GNGDatabaseSimple* myDatabase;
SHMemoryManager *shm;
MyMutex * grow_mutex; //koniecznie taka nazwa (Extern)
MyMutex * graph_mutex;


//trzeba updatowac dosyc czesto

struct GNGGraphInfo{
    typedef offset_ptr< GNGNode  > PoolPtr;
    
    
    int nodes;
    int pool_nodes;
    int first_free;
    PoolPtr * ptr;

    
    
    static GNGGraphInfo construct(SHMemoryManager *shm, GNGGraph * graph){
        GNGGraphInfo ggi;
        
        typedef offset_ptr< GNGNode > PoolPtr;
        
        PoolPtr * poolPtr = shm->get_segment()->construct<PoolPtr>("SHPoolPtr")();
        *poolPtr = graph->getPool();
        
        ggi.ptr = poolPtr;
        ggi.nodes = graph->getNumberNodes();
        ggi.pool_nodes = graph->getPoolNodes();
        ggi.first_free = graph->getFirstFree();
        
        return ggi;
    }
};



void gngTrainingThread(){
    while(myDatabase->getSize()==0);
    gngAlgorithm->runAlgorithm();
}
void gngDatabaseThread(){
    boost::posix_time::seconds workTime(0);  
    
    GNGGraphInfo * ggi = shm->get_segment()->construct< GNGGraphInfo >("SHGraphInfo")();    
    *ggi = GNGGraphInfo::construct(shm,gngAlgorithm->get_graph());
    
    
    double pos[3];
    
    while(1)
    {
        boost::this_thread::sleep(workTime);  
        pos[0] = (double)__int_rnd(0,10);
        pos[1] = (double)__int_rnd(0,10);
        pos[2] = (double)__int_rnd(0,10);
        
        myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
        
        grow_mutex->lock();
        
      
        cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        
        *(ggi->ptr) = gngAlgorithm->get_graph()->getPool();
        ggi->nodes = gngAlgorithm->get_graph()->getNumberNodes();
        ggi->pool_nodes = gngAlgorithm->get_graph()->getPoolNodes();
        ggi->first_free = gngAlgorithm->get_graph()->getFirstFree();        
        grow_mutex->unlock();
        
     
    }
}
void gngDatabaseThread2(){
    boost::posix_time::seconds workTime(0);  
    

    
    double pos[3];
    
    while(1)
    {
    
        
        grow_mutex->lock();
        
      
        cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        
        
        grow_mutex->unlock();

    }
}

int main(int argc, char** argv) {
    typedef boost::interprocess::interprocess_mutex MyMutex;
    //SharedMemory Setup
    
    shm=new SHMemoryManager(10000000);
    cout<<shm->get_name()<<endl;
    
    //Memory
    GNGGraph::mm = shm;
    GNGNode::mm = shm;
    GNGVector::mm = shm;
    GNGVector::alloc_inst = new ShmemAllocator(shm->get_segment()->get_segment_manager());
    
    //Dimension
    GNGExample::N=3;
    
    //Inits
    myDatabase = new GNGDatabaseSimple(); 

    grow_mutex = shm->get_segment()->construct< MyMutex >("grow_mutex")();
    gngAlgorithm= new GNGAlgorithm(myDatabase,2000,0);
    
    
    dbg.push_back(3,"Main::Allocated main structures");
    
   // long a;
    //while(1){(grow_mutex)->lock(); grow_mutex->unlock(); }
    
    boost::thread workerThread1(gngTrainingThread);  
    boost::thread workerThread2(gngDatabaseThread); 
   // gngDatabaseThread2();
 
    workerThread1.join();
    workerThread2.join();
   

    
    
    return 0;
}




