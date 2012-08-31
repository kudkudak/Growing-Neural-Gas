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
#include <cmath>

#include "GNG.h"


DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;



typedef boost::interprocess::interprocess_mutex MyMutex;


GNGAlgorithm * gngAlgorithm;
GNGDatabase* myDatabase;
GNGGraphInfo * ggi;
SHMemoryManager *shm;
MyMutex * grow_mutex; //koniecznie taka nazwa (Extern)
MyMutex * graph_mutex; //not used
MyMutex * database_mutex; //not used
int * communication_buffer;
int GNG_DIM=3;
//trzeba updatowac dosyc czesto





void gngTrainingThread(){
    while(myDatabase->getSize()<2000);
    //dbg.push_back(3,"gngTrainingThread::proceeding to algorithm");
    gngAlgorithm->runAlgorithm();
}
void gngDatabaseThread(){
    boost::posix_time::millisec workTime(1);  

    

    //dbg.push_back(1,"gngDatabaseThread::created GNGGraphInfo");
    
    int k=0;
    double pos[3];
    
    
    while((myDatabase->getSize()<54000) ){
            double r = 1;//(double)rand()*100;
        double alfa = (double)rand()*6.28;
        double beta = (double)rand()*3.14;
        pos[0] = r*cos(beta);
        pos[1] = r*sin(beta)*cos(alfa);
        pos[2] = r*sin(beta)*sin(alfa);
    
     
       
       myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
    }
    while(1)
    {
       
        boost::this_thread::sleep(workTime);  
        

        
        ++k;
        if(k%1000==0) {
            //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            int a = gngAlgorithm->get_graph()->getNumberNodes();
            int b = myDatabase->getSize();
           // cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            cout << ":" << a << "," << b << endl;
 
        }
              grow_mutex->lock();

            // cout<<gngAlgorithm->get_graph()->reportPool()<<endl;
            //dbg.push_back(-1, "gngDatabaseThread::updatingStructure()");
            //ggi->update();



            //cout<<(long)(ggi->ptr).get()<<endl;

            ggi->update();

            grow_mutex->unlock();
        //problem with grow_mutex 
        
        



        //dbg.push_back(-1,"gngDatabaseThread::updatingStructureSuccess()");
     
    }
}
void gngDatabaseThread3(){
    boost::posix_time::millisec workTime(1);  

    

    //dbg.push_back(1,"gngDatabaseThread::created GNGGraphInfo");
    
    int k=0;
    double pos[3];
    

    while(1)
    {
       
        boost::this_thread::sleep(workTime);  
        

        
        ++k;
        if(k%100==0) {
            //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            int a = gngAlgorithm->get_graph()->getNumberNodes();
            int b = myDatabase->getSize();
           // cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            cout << ":" << a << "," << b << endl;
 
        }
              grow_mutex->lock();

            // cout<<gngAlgorithm->get_graph()->reportPool()<<endl;
            //dbg.push_back(-1, "gngDatabaseThread::updatingStructure()");
            //ggi->update();



            //cout<<(long)(ggi->ptr).get()<<endl;

            ggi->update();

            grow_mutex->unlock();
        //problem with grow_mutex 
        
        



        //dbg.push_back(-1,"gngDatabaseThread::updatingStructureSuccess()");
     
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
        //dbg.push_back(-1,"gngDatabaseThread2::updatingInfo()"); 
        int a = gngAlgorithm->get_graph()->getNumberNodes();
        int b = myDatabase->getSize();
        //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        cout<<(++k)<<":"<<a<<","<<b<<endl;
        
        if(k%10==0) cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
        
        pos[0] = (double)__int_rnd(0,100);
        pos[1] = (double)__int_rnd(0,100);
        pos[2] = (double)__int_rnd(0,100);
        
       // myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
        
        
        //dbg.push_back(-1,"gngDatabaseThread2::updatingStructure()");
        ggi->update();
        grow_mutex->unlock();
        //dbg.push_back(-1,"gngDatabaseThread2::updatingStructureSuccess()");
    }
}
 std::vector<GNGExample> pool;
double dist(int a, double* b){
	double ret=0.0;
	for(int i=0;i<GNG_DIM;++i){
		ret+=(pool[a].position[i]-b[i])*(pool[a].position[i]-b[i])	;
	} 
	return ret;
}
void testUG(){
     myDatabase = new GNGDatabaseSphere();
    __init_rnd();
    
    typedef std::list<int> Node;
    
    double orig[3]={-2.0,-2.0,-2.0};
    int dim[3]={100,100,100};
    double query[3];
    
    UniformGrid<std::vector< Node >, Node> ug(orig,dim,0.04);
    ug.setDistFunction(dist);
    

   
   
    
    REP(i,100000){
        GNGExample tmp = myDatabase->drawExample();
        //write_array(tmp.position,tmp.position+3);
        pool.push_back(tmp);
        ug.insert(tmp.position,i);
    }
    cout<<ug.getDensity()<<endl;

}



void initGNGServer(){

    typedef boost::interprocess::interprocess_mutex MyMutex;
    //SharedMemory Setup        

    shm = new SHMemoryManager(200000000 * sizeof (double));
    shm->new_segment(220000000 * sizeof (double));

    cout << shm->get_name(1) << endl;
    cout << shm->get_name(0) << endl;
    //Memory

    //TO-DO: q()
    //implementation-depend : move

    GNGGraph::mm = shm;
    GNGNode::mm = shm;
    GNGList::mm = shm;
    GNGList::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    //Dimension
    GNGExample::N = 3;

    //Inits
    double orig[3]={0.0,0.0,0.0};
    double axis[3]={1.0,1.0,1.0};
    

    
    
    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());

    grow_mutex = shm->get_segment(0)->construct< MyMutex > ("grow_mutex")();
    database_mutex = shm->get_segment(1)->construct< MyMutex > ("database_mutex")();

    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    myDatabase =new GNGDatabasePlane();//new GNGDatabaseSimple(database_mutex, database_vec);
   
   
    
    //A little bit to complicated init  
    gngAlgorithm = new GNGAlgorithm(myDatabase, 25000, orig, axis,2.0);
    
    
    
    


    ggi = shm->get_segment(0)->construct< GNGGraphInfo > ("SHGraphInfo")(gngAlgorithm, shm);


    communication_buffer = static_cast<int*> (shm->allocate(200 * sizeof (int), 1));

    //dbg.push_back(3,"Main::Allocated main structures");



    // long a;
    //while(1){(grow_mutex)->lock(); grow_mutex->unlock(); }

    boost::thread workerThread1(gngTrainingThread);
    boost::thread workerThread2(gngDatabaseThread);

    workerThread1.join();
    workerThread2.join();


    delete shm;
}




int main(int argc, char** argv) {
    //uniform grid dev
    //testUG();
    initGNGServer();
    
    return 0;
}



