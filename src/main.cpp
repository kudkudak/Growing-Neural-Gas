/*
 * File:   main.cpp
 * Author: Stanislaw "kudkudak" Jastrzebski <staszek [ at ] gmail com>
 *
 * Created on 7 sierpień 2012, 18:27
 */

#include <fstream>
#include <list>
#include <cmath>
#include <iostream>
#include <vector>
#include <iostream>   
#include <utility>   
#include <algorithm>  
#include "GNG.h"

using namespace std;


DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;
using namespace boost;

typedef boost::interprocess::interprocess_mutex MyMutex;

/**
 * 
 * Configuration of GNG algorithm/server
 * TODO: add detailed description for parameters
 */
class GNGConfiguration{
public:
    GNGConfiguration(): gngDim(-1), databaseType(None), serverId(-1){}
    
    enum DatabaseType{
        None,
        DatabaseSimple,
        DatabaseProbabilistic
    };
    /**Maximum number of nodes*/
    int max_nodes;//=1000;
    /**Uniform grid optimization*/
    bool uniformgrid_optimization;//=true,lazyheap=true;
    /**Lazy heap optimization*/
    bool lazyheap_optimization;
    /**Bounding box specification*/
    std::vector<double> orig,axis;
    /**Max edge age*/
    int max_age;//=200;
    /**Alpha coefficient*/
    double alpha;//=0.95;
    /**Beta coefficient*/
    double beta;//=0.9995;
    /**Lambda coefficient*/
    double lambda;//=200;
    /**Epsilion v*/
    double eps_v;//=0.05;
    /**Memory bound*/
    int memory_bound;
    /**Epsilion n*/
    double eps_n;//=0.0006;   
    
    /**Dimensionality of examples*/
    int gngDim;
    /**Type of used database*/
    DatabaseType databaseType;
    /**Id of the server*/
    int serverId;
    
    
    static GNGConfiguration getDefaultConfiguration(){
        GNGConfiguration default_configuration;
        return default_configuration;
    }
    
    bool checkCorrectness(){
        GNGConfiguration empty_configuration;
        return gngDim!=empty_configuration.gngDim && 
               serverId!=empty_configuration.serverId && 
                databaseType != empty_configuration.databaseType;
    }
    
    
    void updateConfiguration(GNGConfiguration update){
        //python dictionary is missing! 
        GNGConfiguration empty_configuration;
        
        if(update.gngDim != empty_configuration.gngDim)
            gngDim = update.gngDim;
        
        if(update.serverId != empty_configuration.serverId)
            serverId = update.serverId;
        
        if(update.databaseType != empty_configuration.databaseType)
            serverId = update.serverId; 
    }
    
};



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

/** Handles communication with server through different communication channels */
class GNGClient{
    GNGClient(){}
public:
    GNGClient(std::string process_identifier){}
};

/** Holds together all logic and objects.*/
class GNGServer{
    int listening_daemon_sleep_time;
public:
    /**OBSOLETE: Create GNGServer (and fill it manually after that)*/
    GNGServer(): listening_daemon_sleep_time(50){}
    /**Construct GNGServer using configuration*/
    GNGServer(GNGConfiguration configuration): listening_daemon_sleep_time(50){}
    
    boost::interprocess::interprocess_mutex communication_bufor_mutex; /** Locking communication bufor in interprocess communication */
    
    GNGAlgorithmControl * gngAlgorithmControl;
    GNGAlgorithm * gngAlgorithm;
    SHGNGGraph * gngGraph;
    GNGDatabase* gngDatabase;
    SHMemoryManager *shm;
    
    GNGAlgorithmControl * getAlgorithmControl(){
        return this->gngAlgorithmControl;
    }
    GNGAlgorithm * getAlgorithm(){
        return this->gngAlgorithm;
    }
    SHGNGGraph * getGraph(){
        return this->gngGraph;
    }
    GNGDatabase * getDatabase(){
        return this->gngDatabase;
    }
 
    SHMemoryManager * getSHM(){
        return this->shm;
    }    
    
    /**Run main processing loop for shared memory communication channel*/
    void runSHListeningDaemon(){
        while(true){
           communication_bufor_mutex.lock();
           
           int status = *this->getSHM()->get_segment(0)->find<int>("GNGMessageState").first;
           if(status == SHGNGMessage::Waiting){
                int type = *this->getSHM()->get_segment(0)->find<int>("GNGMessageType").first;
                #ifdef DEBUG
                dbg.push_back(12, "GNGServer::runListeningDaemon caught message of type "+to_string<int>(type));
                #endif

                *this->getSHM()->get_segment(0)->find<int>("GNGMessageState").first = SHGNGMessage::Processed;
           }
          
           communication_bufor_mutex.unlock();
            
           boost::this_thread::sleep(boost::posix_time::millisec(this->listening_daemon_sleep_time)); 
        }
    }
    
    
};



GNGServer * gngServer;




//Global variables
int GNG_DIM, GNG_EX_DIM;

void gngTrainingThread(){
    dbg.set_debug_level(10);
    while(gngServer->gngDatabase->getSize()<2000);
    #ifdef DEBUG
    dbg.push_back(3,"gngTrainingThread::proceeding to algorithm");
    #endif
    gngServer->gngAlgorithm->runAlgorithm();
}


struct Vertex{
	int m_gngid;
};
struct Edge{

	Edge(double w): weight(w),visited_times(0){}
	double weight;
	int visited_times;
};

void gngDatabaseThread(){
    boost::posix_time::millisec workTime(500);
    int k=0;
    double pos[3];


    while(true){
       boost::this_thread::sleep(workTime);
       REPORT(gngServer->gngGraph->getNumberNodes()); 
       REPORT(gngServer->gngAlgorithm->CalculateAccumulatedError());
    }

}


double * uploadOBJ(const char * filename){
    GNGExample ex(3);
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

    
        gngServer->gngDatabase->addExample(new GNGExample(vertex,3));
    }
    write_array(bbox,bbox+6);
    return bbox;
}

void initGNGServer(){
    gngServer = new GNGServer();
    gngServer->shm = new SHMemoryManager("Server0",200000000 * sizeof (double)); //nodes <-estimate!
    gngServer->shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    SHGNGNode::mm = gngServer->shm;
    SHGNGNode::alloc_inst = new ShmemAllocatorGNG(gngServer->shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator alc(gngServer->shm->get_segment(1)->get_segment_manager());
    SHGNGExampleDatabase * database_vec = 
            gngServer->shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngServer->gngAlgorithmControl = gngServer->shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngServer->gngDatabase = new GNGDatabaseSimple
            <SHGNGExampleDatabase, 
            boost::interprocess::interprocess_mutex>(&gngServer->gngAlgorithmControl->database_mutex,database_vec, 
    GNG_DIM);




    gngServer->gngDatabase = new GNGDatabaseMeshes();//new GNGDatabasePlane();//new GNGDatabaseSimple(database_mutex, database_vec);

    double c1[]={0,1.0,1.0};
    double c2[]={-1,-2,-1};
    double c3[]={-1,-1,-1};
    double c4[]={1,2.10,};

    GNGDatabaseLine l1(c4,4.0);

    GNGDatabaseSphere d1(c1, 1.0),d2(c2,1.5);
    reinterpret_cast<GNGDatabaseMeshes*>(gngServer->gngDatabase)->addMesh(&d1);
    GNGDatabasePlane p(c3,4.5);
    reinterpret_cast<GNGDatabaseMeshes*>(gngServer->gngDatabase)->addMesh(&p);
    reinterpret_cast<GNGDatabaseMeshes*>(gngServer->gngDatabase)->addMesh(&l1);

    gngServer->gngGraph = gngServer->shm->get_segment(0)->construct<SHGNGGraph>("gngGraph")(&gngServer->gngAlgorithmControl->grow_mutex,25000);
    gngServer->gngAlgorithm = new GNGAlgorithm
                (*gngServer->gngGraph,gngServer->gngDatabase, gngServer->gngAlgorithmControl,
                25000, orig, axis,axis[0]/4.0,10000);

    
    gngServer->gngAlgorithmControl->setRunningStatus(true); 
    gngServer->gngAlgorithm->setToggleLazyHeap(false);
    gngServer->gngAlgorithm->setToggleUniformGrid(false);

   
    
    boost::thread workerThread1(gngTrainingThread);
    boost::thread workerThread2(gngDatabaseThread);

    workerThread1.join();
    workerThread2.join();


    delete gngServer->shm;
}


void testDatabase(){
    gngServer->shm = new SHMemoryManager("Server0",200000000 * sizeof (double)); //nodes <-estimate!
    gngServer->shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    SHGNGNode::mm = gngServer->shm;
    SHGNGNode::alloc_inst = new ShmemAllocatorGNG(gngServer->shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator
    alc(gngServer->shm->get_segment(1)->get_segment_manager());

    SHGNGExampleDatabase * database_vec = 
            gngServer->shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngServer->gngAlgorithmControl = gngServer->shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

   gngServer-> gngDatabase = new GNGDatabaseProbabilistic<
        SHGNGExampleDatabase, 
        boost::interprocess::interprocess_mutex
            
    >
    (&gngServer->gngAlgorithmControl->database_mutex,database_vec, GNG_DIM);

    double * pos=new double[4];
    for(int i=0;i<10000;++i){
    	pos[0]=__double_rnd(0,1);
    	pos[1]=__double_rnd(0,1);
        pos[2]=__double_rnd(0,1);
        pos[3]=__double_rnd(0,1);


    	gngServer->gngDatabase->addExample(new GNGExample(pos, 4));
    }
    GNGExample ex = gngServer->gngDatabase->drawExample();
    REPORT(ex);




}

void testDatabaseLocal(){
    gngServer =new GNGServer();
    
    #ifdef DEBUG
        cout<<"testDatabaseLocal\n";
    #endif
    GNG_DIM=3;
    GNG_EX_DIM=4;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    

    GNG_DIM=3;
    
    std::vector<GNGExample> g_database;
    
    gngServer->shm = new SHMemoryManager("Server0",200000000 * sizeof (double)); //nodes <-estimate!
    gngServer->shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    
    
    SHGNGNode::mm =  gngServer->shm;
    SHGNGNode::alloc_inst = new ShmemAllocatorGNG(gngServer->shm->get_segment(0)->get_segment_manager());
    REPORT("success");
     gngServer->gngAlgorithmControl = gngServer->shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

     gngServer->gngDatabase = new GNGDatabaseProbabilistic<std::vector<GNGExample> ,boost::mutex>
            (&gngServer->gngAlgorithmControl->database_mutex_thread, &g_database, 3);
   REPORT("success");
    double * pos=new double[4];
    for(int i=0;i<10000;++i){
    	pos[0]=__double_rnd(0,1);
    	pos[1]=__double_rnd(0,1);
        pos[2]=__double_rnd(0,1);
        pos[3]=__double_rnd(0,1);
        GNGExample * ex = new GNGExample(pos,4);
    	gngServer->gngDatabase->addExample(ex);
        
        
        delete ex;
    }
    REPORT("success");
    GNGExample ex = gngServer->gngDatabase->drawExample();
    REPORT(ex);

     gngServer->gngGraph = gngServer->shm->get_segment(0)->construct<SHGNGGraph>("gngGraph")(&gngServer->gngAlgorithmControl->grow_mutex,25000);
     REPORT("success");
     gngServer->gngAlgorithm = new GNGAlgorithm
                (* gngServer->gngGraph, gngServer->gngDatabase,  gngServer->gngAlgorithmControl,
                25000, orig, axis,axis[0]/4.0,10000);

     REPORT("success");
    gngServer->gngAlgorithmControl->setRunningStatus(true); //skrypt w R inicjalizuje
    gngServer->gngAlgorithm->setToggleLazyHeap(false);
    gngServer->gngAlgorithm->setToggleUniformGrid(false);
    REPORT("success");
    boost::thread workerThread1(gngTrainingThread);
    boost::thread workerThread2(gngDatabaseThread);

    workerThread1.join();
    workerThread2.join();


}


int main(int argc, char** argv) {

 

    
        testDatabaseLocal();

//    initGNGServer();
	//testDatabase();
    return 0;
}

