/* 
 * File:   main.cpp
 * Author: staszek
 *
 * Created on 7 sierpień 2012, 18:27
 */

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <list>

#include "GNG.h"


DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;



typedef boost::interprocess::interprocess_mutex MyMutex;


GNGAlgorithm * gngAlgorithm;
GNGDatabaseSimple* myDatabase;
GNGGraphInfo * ggi;
SHMemoryManager *shm;
MyMutex * grow_mutex; //koniecznie taka nazwa (Extern)
MyMutex * graph_mutex; //not used
MyMutex * database_mutex; //not used
int * communication_buffer;
//trzeba updatowac dosyc czesto





void gngTrainingThread(){
    while(myDatabase->getSize()<5);
    dbg.push_back(3,"gngTrainingThread::proceeding to algorithm");
    gngAlgorithm->runAlgorithm();
}
void gngDatabaseThread(){
    boost::posix_time::millisec workTime(1);  

    

    dbg.push_back(1,"gngDatabaseThread::created GNGGraphInfo");
    
    int k=0;
    double pos[3];
    
    while(1)
    {
       
        boost::this_thread::sleep(workTime);  
        pos[0] = (double)__int_rnd(0,100);
        pos[1] = (double)__int_rnd(0,100);
        pos[2] = (double)__int_rnd(0,100);
        
       if(myDatabase->getSize()<10000) myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
        
        ++k;
        if(k%100==0) {
            //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            int a = gngAlgorithm->get_graph()->getNumberNodes();
            int b = myDatabase->getSize();
            //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            cout << ":" << a << "," << b << endl;
            grow_mutex->lock();

            // cout<<gngAlgorithm->get_graph()->reportPool()<<endl;
            dbg.push_back(-1, "gngDatabaseThread::updatingStructure()");
            //ggi->update();



            //cout<<(long)(ggi->ptr).get()<<endl;

            ggi->update();

            grow_mutex->unlock();
        }
   
        //problem with grow_mutex 
        
        



        dbg.push_back(-1,"gngDatabaseThread::updatingStructureSuccess()");
     
    }
}
void gngDatabaseThread2(){
    boost::posix_time::millisec workTime(300);  
    

    int k=0;
    double pos[3];
    
    while(1)
    {
    
        boost::this_thread::sleep(workTime); 
        grow_mutex->lock();
        dbg.push_back(-1,"gngDatabaseThread2::updatingInfo()"); 
        int a = gngAlgorithm->get_graph()->getNumberNodes();
        int b = myDatabase->getSize();
        //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        cout<<(++k)<<":"<<a<<","<<b<<endl;
        
        if(k%10==0) cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        
        pos[0] = (double)__int_rnd(0,100);
        pos[1] = (double)__int_rnd(0,100);
        pos[2] = (double)__int_rnd(0,100);
        
       // myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
        
        
        dbg.push_back(-1,"gngDatabaseThread2::updatingStructure()");
        ggi->update();
        grow_mutex->unlock();
        dbg.push_back(-1,"gngDatabaseThread2::updatingStructureSuccess()");
    }
}



int main(int argc, char** argv) {

    typedef boost::interprocess::interprocess_mutex MyMutex;
    //SharedMemory Setup        
    
    shm=new SHMemoryManager(100000000*sizeof(double));
    shm->new_segment(10000000*sizeof(double));
    
    cout<<shm->get_name(1)<<endl;
    cout<<shm->get_name(0)<<endl;
    //Memory
    
    //TO-DO: q()
    //implementation-depend : move
    
    GNGGraph::mm = shm;
    GNGNode::mm = shm;
    GNGList::mm = shm;
    GNGList::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());
    
    //Dimension
    GNGExample::N=3;
    
    //Inits
    
    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());
    
    grow_mutex = shm->get_segment(0)->construct< MyMutex >("grow_mutex")();
    database_mutex = shm->get_segment(1)->construct< MyMutex >("database_mutex")();
    
    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase >("database_vec")(alc);
  
    myDatabase =new GNGDatabaseSimple(database_mutex,database_vec);     
    gngAlgorithm= new GNGAlgorithm(myDatabase,10000,0);
    
    
    
    ggi = shm->get_segment(0)->construct< GNGGraphInfo >("SHGraphInfo")(gngAlgorithm,shm);    
    
    
    communication_buffer = static_cast<int*>(shm->allocate(200*sizeof(int),1));
    
    dbg.push_back(3,"Main::Allocated main structures");
    
    
    
   // long a;
    //while(1){(grow_mutex)->lock(); grow_mutex->unlock(); }
    
   boost::thread workerThread1(gngTrainingThread);  
   boost::thread workerThread2(gngDatabaseThread); 

   workerThread1.join();
   workerThread2.join();
   

    delete shm;
    
    return 0;
}



