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
#include <iostream>
using namespace std;
#include "GNG.h"



DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;



typedef boost::interprocess::interprocess_mutex MyMutex;

GNGAlgorithmControl * gngAlgorithmControl;
GNGAlgorithm * gngAlgorithm;
GNGGraph * gngGraph;
GNGDatabase* gngDatabase;
SHMemoryManager *shm;


int GNG_DIM;


void gngTrainingThread(){
    while(gngDatabase->getSize()<2000);
    #ifdef DEBUG
    dbg.push_back(3,"gngTrainingThread::proceeding to algorithm");
    #endif
    gngAlgorithm->runAlgorithm();
}
void gngDatabaseThread(){
    boost::posix_time::millisec workTime(1);
    int k=0;
    double pos[3];

    while(1)
    {
        boost::this_thread::sleep(workTime);

        ++k;


    }
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
        gngDatabase->addExample(&ex);
    }
    write_array(bbox,bbox+6);
    return bbox;
}

void initGNGServer(){

    shm = new SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    GNGNode::mm = shm;
    GNGNode::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());

    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngDatabase = new GNGDatabaseSimple(&gngAlgorithmControl->database_mutex,database_vec);




    gngDatabase = new GNGDatabaseMeshes();//new GNGDatabasePlane();//new GNGDatabaseSimple(database_mutex, database_vec);

    double c1[]={0,1.0,1.0};
    double c2[]={-1,-2,-1};
    double c3[]={-1,-1,-1};
    double c4[]={1,2.10,};

    GNGDatabaseLine l1(c4,4.0);

    GNGDatabaseSphere d1(c1, 1.0),d2(c2,1.5);
    reinterpret_cast<GNGDatabaseMeshes*>(gngDatabase)->addMesh(&d1);
    GNGDatabasePlane p(c3,4.5);
    reinterpret_cast<GNGDatabaseMeshes*>(gngDatabase)->addMesh(&p);
    reinterpret_cast<GNGDatabaseMeshes*>(gngDatabase)->addMesh(&l1);



   cout<<"cos robie\n";
    gngGraph = shm->get_segment(0)->construct<GNGGraph>("gngGraph")(&gngAlgorithmControl->grow_mutex,25000);
    gngAlgorithm = new GNGAlgorithm
                (*gngGraph,gngDatabase, gngAlgorithmControl,
                25000, orig, axis,axis[0]/4.0,10000);
	cout<<"DZIALAM\n";
    gngAlgorithmControl->setRunningStatus(true); //skrypt w R inicjalizuje
    gngAlgorithm->setToggleLazyHeap(false);
    gngAlgorithm->setToggleUniformGrid(false);

    boost::thread workerThread1(gngTrainingThread);
    boost::thread workerThread2(gngDatabaseThread);

    workerThread1.join();
    workerThread2.join();


    delete shm;
}


void testDatabase(){
    shm = new SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    GNGNode::mm = shm;
    GNGNode::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());

    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngDatabase = new GNGDatabaseProbabilistic(&gngAlgorithmControl->database_mutex,database_vec);

    double * pos=new double[4];
    for(int i=0;i<10000;++i){
    	pos[0]=__double_rnd(0,1);
    	pos[1]=__double_rnd(0,1);
		pos[2]=__double_rnd(0,1);
		pos[3]=__double_rnd(0,1);

    	gngDatabase->addExample(new GNGExample(pos));
    }
    gngDatabase->drawExample();

}

int main(int argc, char** argv) {

    //initGNGServer();
	testDatabase();
    return 0;
}

