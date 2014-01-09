
#include "GNGServer.h"
#include "Utils.h"

#include <algorithm>
#include <utility>
using namespace std;


/** Run GNGAlgorithm on cube (3-dimensional) with given parameters
 * @returns pair<double, double> : nodes, mean_error
 */
pair<double, double> test_convergence(GNGConfiguration * cnf=0, int num_database=1000,
        int ms_loop = 5000) {
    
    
    
    dbg.set_debug_level(12);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();  
    config.uniformgrid_optimization = true;
    if(cnf) config=*cnf;
    config.databaseType = GNGConfiguration::DatabaseProbabilistic;
    
    GNGServer *s = GNGServer::constructTestServer(config);
    
//    GNGServer::setConfiguration(config); 
//    GNGServer::getInstance()->run();
    
    s->run();
    
    double * vect = new double[4*num_database];
    for (int i = 0; i < num_database; ++i) {
        
        vect[0+(i)*4] = __double_rnd(0, 1);
        vect[1+(i)*4] = __double_rnd(0, 1);
        vect[2+(i)*4] = __double_rnd(0, 1);
        vect[3+(i)*4] = __double_rnd(0, 1);
        
    }
    DBG(12, "testNewInterface::Server running");
    
    
    s->addExamples(&vect[0], num_database);

    
    delete[] vect;
    
   
    boost::posix_time::millisec workTime(500);

    dbg.push_back(12, "testNewInterface::Collecting results");
    
    int iteration = 0;
    
    
    //Should converge to 0.06-0.07 in 300-400 iterations
    while(true){
       ++iteration;
       REPORT(iteration);
       boost::this_thread::sleep(workTime);
       REPORT(s->getGraph().getNumberNodes()); 
       REPORT(s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.)); 
       if(iteration >= ms_loop/500) break;
    }
    
    s->getAlgorithm().terminate();
    boost::this_thread::sleep(workTime);
    
    dbg.push_back(12, "testNewInterface::Completed testLocal()");
    
    
    
            
    pair<double , double> t = pair<double, double>(s->getGraph().getNumberNodes(),
            s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.));
    
    
    delete s;
    
    return t;
 }


TEST(BasicTests, BasicConvergence){
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    pair<double, double> results = test_convergence(&config, 1000, 5000);
    ASSERT_GE(results.first, 100.0);
    ASSERT_LE(results.second, 40.0);
}