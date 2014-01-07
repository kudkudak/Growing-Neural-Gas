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








GNGServer * gngServer;

//Global variables
int GNG_DIM, GNG_EX_DIM;



struct Vertex{
	int m_gngid;
};
struct Edge{

	Edge(double w): weight(w),visited_times(0){}
	double weight;
	int visited_times;
};

//void gngDatabaseThread(){
//    boost::posix_time::millisec workTime(500);
//    int k=0;
//    double pos[3];
//
//
//    while(true){
//       boost::this_thread::sleep(workTime);
//       REPORT(gngServer->getGraph()->getNumberNodes()); 
//       REPORT(gngServer->getAlgorithm()->CalculateAccumulatedError());
//    }
//
//}


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

    
        gngServer->getDatabase()->addExample(new GNGExample(vertex,3));
    }
    write_array(bbox,bbox+6);
    return bbox;
}

#include "informal_tests/test_database.cpp"

int main(int argc, char** argv) {
    dbg.set_debug_level(0);
    testNewInterface();

    
    
    return 0;
}

