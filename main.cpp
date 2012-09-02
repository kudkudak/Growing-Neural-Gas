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
#include <fstream>
#include <list>
#include <cmath>

#include "GNG.h"


DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;



typedef boost::interprocess::interprocess_mutex MyMutex;

GNGAlgorithmControl * gngAlgorithmControl;
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
    
   /*
    while((myDatabase->getSize()<54000) ){
            double r = 1;//(double)rand()*100;
        double alfa = (double)rand()*6.28;
        double beta = (double)rand()*3.14;
        pos[0] = r*cos(beta);
        pos[1] = r*sin(beta)*cos(alfa);
        pos[2] = r*sin(beta)*sin(alfa);
    
     
       
       myDatabase->addExample(new GNGExample(&pos[0])); //memory leak
    }*/
    while(1)
    {
       
        boost::this_thread::sleep(workTime);  
        

        
        ++k;
        if(k%1000==0) {
            
            //cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            int a = gngAlgorithm->get_graph()->getNumberNodes();
            int b = myDatabase->getSize();
           // cout<<(gngAlgorithm->get_graph()->reportPool(false))<<endl;
            cout<<"NODE 0 "<<*(gngAlgorithm->m_g[0])<<endl;
            cout << ":" << a << "," << b << " error = "<<gngAlgorithm->getAccumulatedError()<<endl;
            
        }
             

            // cout<<gngAlgorithm->get_graph()->reportPool()<<endl;
            //dbg.push_back(-1, "gngDatabaseThread::updatingStructure()");
            //ggi->update();

              if(k
                      ){ grow_mutex->lock();
                  ggi->update();grow_mutex->unlock();
              }

            //cout<<(long)(ggi->ptr).get()<<endl;

            

            
        //problem with grow_mutex 
        
        



        //dbg.push_back(-1,"gngDatabaseThread::updatingStructureSuccess()");
     
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

//uploades v to database outputs GNG_DIM*2 array with bounding box
double * uploadOBJ(const char * filename){
    GNGExample ex;
    ifstream ifs;
    ifs.open(filename);
    double vertex[3];
    double *bbox = new double[6];
    REP(i,6) bbox[i]=-1;
    std::string v;
    int iteration=0;
    while(!ifs.eof()){
        ifs>>v;
        if(v!="v") continue;
        ++iteration;
        REP(i,3){
            ifs>>vertex[i];
            if(iteration==1){
                bbox[2*i]=vertex[i];
                bbox[2*i+1]=vertex[i];
            }
            else{
                bbox[2*i]=std::min(bbox[2*i],vertex[i]);
                bbox[2*i+1]=std::max(bbox[2*i+1],vertex[i]);
            }
           // write_array(vertex,vertex+3);
           // cout<<bbox[1]<<endl;
            
        }
        
        memcpy(ex.position,vertex,3*sizeof(double));
        myDatabase->addExample(&ex);
    }
    write_array(bbox,bbox+6);
    return bbox;
}

void initGNGServer(){

    typedef boost::interprocess::interprocess_mutex MyMutex;
    //SharedMemory Setup        

    shm = new 
            
            
            
            
            
            SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

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
    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};
    

    
    
    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());

    grow_mutex = shm->get_segment(0)->construct< MyMutex > ("grow_mutex")();
    database_mutex = shm->get_segment(1)->construct< MyMutex > ("database_mutex")();

    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);
    
    
    /*
    myDatabase = new GNGDatabaseSimple(database_mutex,database_vec);
    
    
    double * bbox = uploadOBJ("buddha2.obj");
    orig[0] = bbox[0]-0.1;
    orig[1] = bbox[2]-0.1;
    orig[2] = bbox[4]-0.1;
    
    axis[0] = bbox[1] - orig[0] + 0.1;
    axis[1] = bbox[3] - orig[1]+0.1;
    axis[2] = bbox[5] - orig[2]+0.1;
    */
    
     
    
   
    myDatabase = new GNGDatabaseMeshes();//new GNGDatabasePlane();//new GNGDatabaseSimple(database_mutex, database_vec);
  
    
    
    double c1[]={0,0.5,0,5};
    double c2[]={-1,-2,-1};
    double c3[]={-3,-3,-3};
    
    
    GNGDatabaseSphere d1(c1, 1.0),d2(c2,1.5);
    reinterpret_cast<GNGDatabaseMeshes*>(myDatabase)->addMesh(&d1);
    reinterpret_cast<GNGDatabaseMeshes*>(myDatabase)->addMesh(&d2);
    GNGDatabasePlane p(c3,4.5);
    reinterpret_cast<GNGDatabaseMeshes*>(myDatabase)->addMesh(&p);
    
    
    //A little bit to complicated init  
    //gngAlgorithm = new GNGAlgorithm(myDatabase, 25000, orig, axis,2.0);
    
    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();
    
    gngAlgorithm = new GNGAlgorithm
                (myDatabase, gngAlgorithmControl,
                25000, orig, axis,axis[0]/4.0,5000);
    
    

    
    ggi = shm->get_segment(0)->construct< GNGGraphInfo > ("SHGraphInfo")(gngAlgorithm, shm);

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



