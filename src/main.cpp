/*
 * File:   main.cpp
 * Author: staszek
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


GNGAlgorithmControl * gngAlgorithmControl;
GNGAlgorithm * gngAlgorithm;
SHGNGGraph * gngGraph;
GNGDatabase* gngDatabase;
SHMemoryManager *shm;


//Global variables
int GNG_DIM, GNG_EX_DIM;

void gngTrainingThread(){
    dbg.set_debug_level(100);
    while(gngDatabase->getSize()<2000);
    #ifdef DEBUG
    dbg.push_back(3,"gngTrainingThread::proceeding to algorithm");
    #endif
    gngAlgorithm->runAlgorithm();
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
    boost::posix_time::millisec workTime(5000);
    int k=0;
    double pos[3];




    boost::this_thread::sleep(workTime);
    REPORT(gngGraph->getNumberNodes());

    /*
    //tworzenie grafug
    GNGNode * g_pool = gngGraph->getPoolShare();
    int maximumIndex = gngGraph->getMaximumIndex();
    REPORT(gngGraph->getMaximumIndex());
    //w nowej implementacji to bedzie sto razy prostsze
    typedef adjacency_list<listS, vecS, bidirectionalS,Vertex,Edge> Graph;
    typedef Graph::vertex_descriptor VertexID;
    typedef Graph::edge_descriptor  EdgeID;

    Graph G(maximumIndex);
    {
    	ScopedLock sc(gngAlgorithmControl->grow_mutex);
    	ScopedLock sc2(gngAlgorithmControl->database_mutex);

    for(int i=0;i<=maximumIndex;++i){
    	if(g_pool[i].occupied){
    		FOREACH(edg,g_pool[i].edges){

    			REPORT(g_pool[i].dist(&g_pool[edg->nr]));
    			boost::add_edge(i,edg->nr,Edge(g_pool[i].dist(&g_pool[edg->nr])),G);
    		}
    	}
    }
    }
    boost::property_map<Graph, double Edge::*>::type
    	weightPropertyMap = boost::get(&Edge::weight, G);
    VertexID v0 =0;
    std::vector<int> distance(maximumIndex+1, (std::numeric_limits < int >::max)());
    std::vector<std::size_t> parent(maximumIndex+1);
    for (int i = 0; i < maximumIndex; ++i) parent[i] = i;
    distance[v0] = 0;


    typedef graph_traits<Graph>::edge_iterator edge_iter;
    typedef graph_traits<Graph>::out_edge_iterator out_edge_iter;
    std::pair<edge_iter, edge_iter> ep;
    edge_iter ei, ei_end;
    for (tie(ei, ei_end) = edges(G); ei != ei_end; ++ei){
    	G[*ei].visited_times=0;
    }

    out_edge_iter out_i, out_end;
    for (boost::tie(out_i, out_end) = boost::out_edges(v0, G);
               out_i != out_end; ++out_i) {
    	VertexID v1=boost::source(*out_i,G); //bo iterator //moze byc tez target(*out_i,G)
    	G[v1].m_gngid=0;
    }

    bool r = bellman_ford_shortest_paths
        (G, int (maximumIndex), weight_map(weightPropertyMap).distance_map(&distance[0]).
         predecessor_map(&parent[0]));

    REPORT(r);

    FOREACH(dist, distance){
    	REPORT(*dist);
    }

	*/

}


//uploades v to database outputs GNG_DIM*2 array with bounding box
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

    
        gngDatabase->addExample(new GNGExample(vertex,3));
    }
    write_array(bbox,bbox+6);
    return bbox;
}

void initGNGServer(){

    shm = new SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    SHGNGNode::mm = shm;
    SHGNGNode::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());
    SHGNGExampleDatabase * database_vec = 
            shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngDatabase = new GNGDatabaseSimple
            <SHGNGExampleDatabase, 
            boost::interprocess::interprocess_mutex>(&gngAlgorithmControl->database_mutex,database_vec, 
    GNG_DIM);




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

    gngGraph = shm->get_segment(0)->construct<SHGNGGraph>("gngGraph")(&gngAlgorithmControl->grow_mutex,25000);
    gngAlgorithm = new GNGAlgorithm
                (*gngGraph,gngDatabase, gngAlgorithmControl,
                25000, orig, axis,axis[0]/4.0,10000);

    
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

    SHGNGNode::mm = shm;
    SHGNGNode::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    GNG_DIM=3;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};

    SHGNGExampleDatabaseAllocator
    alc(shm->get_segment(1)->get_segment_manager());

    SHGNGExampleDatabase * database_vec = 
            shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngDatabase = new GNGDatabaseProbabilistic<
        SHGNGExampleDatabase, 
        boost::interprocess::interprocess_mutex
            
    >
    (&gngAlgorithmControl->database_mutex,database_vec, GNG_DIM);

    double * pos=new double[4];
    for(int i=0;i<10000;++i){
    	pos[0]=__double_rnd(0,1);
    	pos[1]=__double_rnd(0,1);
        pos[2]=__double_rnd(0,1);
        pos[3]=__double_rnd(0,1);


    	gngDatabase->addExample(new GNGExample(pos, 4));
    }
    GNGExample ex = gngDatabase->drawExample();
    REPORT(ex);




}

void testDatabaseLocal(){

    GNG_DIM=3;
    GNG_EX_DIM=4;

    double orig[3]={-4.0,-4.0,-4.0};
    double axis[3]={8.0,8.0,8.0};


    std::vector<GNGExample> g_database;
    
    shm = new SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!
    
    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();

    gngDatabase = new GNGDatabaseProbabilistic<std::vector<GNGExample> ,boost::mutex>
            (&gngAlgorithmControl->database_mutex_thread, &g_database, 3);
   
    double * pos=new double[4];
    for(int i=0;i<10000;++i){
    	pos[0]=__double_rnd(0,1);
    	pos[1]=__double_rnd(0,1);
        pos[2]=__double_rnd(0,1);
        pos[3]=__double_rnd(0,1);
        GNGExample * ex = new GNGExample(pos,4);
    	gngDatabase->addExample(ex);
        
        
        delete ex;
    }
    REPORT("success");
    GNGExample ex = gngDatabase->drawExample();
    REPORT(ex);




}

void testSharedGraph(){
    
    shm = new SHMemoryManager(200000000 * sizeof (double)); //nodes <-estimate!
    shm->new_segment(220000000 * sizeof (double)); //database <-estimate!

    
    SHGNGNode::mm = shm;
    SHGNGNode::alloc_inst = 
            new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());
    
    REPORT(SHGNGNode::size());
    
    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl >("gngAlgorithmControl")();
    gngGraph = 
            shm->get_segment(0)->construct<SHGNGGraph>("gngGraph")
            (&gngAlgorithmControl->grow_mutex,25000);
    
    
}

int main(int argc, char** argv) {

 

    
        testDatabaseLocal();

//    initGNGServer();
	//testDatabase();
    return 0;
}

