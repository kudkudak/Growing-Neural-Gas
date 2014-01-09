
#include "MemoryAllocator.h"
#include "GNGServer.h"
#include "Utils.h"


void test_convergence(GNGConfiguration * cnf) {
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();  
    
    if(cnf) config=*cnf;
    
    config.databaseType = GNGConfiguration::DatabaseProbabilistic;
    config.uniformgrid_optimization = true;
    
    
    
    
    GNGServer::setConfiguration(config); 
    GNGServer::getInstance().run();
    
    
    
    double * vect = new double[4*100];
    for (int i = 0; i < 100; ++i) {
        
        vect[0+(i)*4] = __double_rnd(0, 1);
        vect[1+(i)*4] = __double_rnd(0, 1);
        vect[2+(i)*4] = __double_rnd(0, 1);
        vect[3+(i)*4] = __double_rnd(0, 1);
        
    }
    DBG(12, "testNewInterface::Server running");
    
    
    GNGServer::getInstance().addExamples(&vect[0], 1000);
    
    
    
    
    
    delete[] vect;
    
   
    boost::posix_time::millisec workTime(500);

    dbg.push_back(12, "testNewInterface::Collecting results");
    
    int iteration = 0;
    
    
    //Should converge to 0.06-0.07 in 300-400 iterations
    while(true){
       ++iteration;
       REPORT(iteration);
       boost::this_thread::sleep(workTime);
       REPORT(GNGServer::getInstance().getGraph().getNumberNodes()); 
       REPORT(GNGServer::getInstance().getAlgorithm().CalculateAccumulatedError()
               /(GNGServer::getInstance().getGraph().getNumberNodes()+0.)); 
    }
    
    dbg.push_back(12, "testNewInterface::Completed testLocal()");
 
 }


