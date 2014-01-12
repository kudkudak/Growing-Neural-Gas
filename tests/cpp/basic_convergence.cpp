
#include "GNGServer.h"
#include "Utils.h"

#include <algorithm>
#include <utility>
#include <vector>
using namespace std;


/** Run GNGAlgorithm on cube (3-dimensional) with given parameters
 * @returns pair<double, double> : nodes, mean_error
 */
pair<double, double> test_convergence(GNGConfiguration * cnf=0, int num_database=1000,
        int ms_loop = 5000) {
    
    
    
    
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();  
    config.uniformgrid_optimization = true;
    if(cnf) config=*cnf;
    config.databaseType = GNGConfiguration::DatabaseProbabilistic;
    
    GNGServer *s = GNGServer::constructTestServer(config);
    
//    GNGServer::setConfiguration(config); 
//    GNGServer::getInstance()->run();
    
    cout<<"Running server\n"<<flush;
    s->run();
    
    
    cout<<"Allocating "<<(config.dim+1)*num_database<<endl<<flush;
    double * vect = new double[(config.dim+1)*num_database];
    for (int i = 0; i < num_database; ++i) {
        for(int j=0;j<= config.dim;++j)
             vect[j+(i)*(config.dim+1)] = __double_rnd(0, 1);
    }
    
    DBG(10, "Allocated examples\n");
    cout<<"Allocated examples\n";
    
    
    
    s->addExamples(&vect[0], num_database);

    
    delete[] vect;
    
    DBG(12, "testNewInterface::Server running");

   
    boost::posix_time::millisec workTime(500);

    cout<< "testNewInterface::Collecting results\n";
    
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
    
    
    cout<< "testNewInterface::Terminating server\n";
    s->getAlgorithm().terminate();
    
    while(s->getAlgorithm().running);
    
    boost::this_thread::sleep(workTime);

            
    pair<double , double> t = pair<double, double>(s->getGraph().getNumberNodes(),
            s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.));
    
    
    delete s;
    
    return t;
 }
#include <cmath>


//TEST(BasicTests, BasicConvergence){
//    dbg.set_debug_level(12);
//    
//    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
//    pair<double, double> results = test_convergence(&config, 1000, 5000);
//    ASSERT_GE(fabs(results.first), 100.0);
//    ASSERT_LE(fabs(results.second), 40.0);
//}

TEST(BasicTests, ManyDimsUGConvergence){
    dbg.set_debug_level(12);
    
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.lazyheap_optimization =  true;
    config.dim = 10;
    config.axis = vector<double>(config.dim , 1.0);
    config.orig = vector<double>(config.dim , 0.0);
    pair<double, double> results = test_convergence(&config, 100, 10000);
    
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}

//

////
////
//TEST(BasicTests, ManyDimsNoUG){
//    cout<<"BasicTests::ManyDimsNoUG"<<endl;
//    dbg.set_debug_level(12);
//    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
//    config.uniformgrid_optimization =  false;
//    config.dim = 50;
//    config.axis = vector<double>(config.dim, 1.0);
//    config.orig = vector<double>(config.dim, 0.0);
//    pair<double, double> results = test_convergence(&config, 100, 5000);
//    
//    ASSERT_GE(fabs(results.first), 100.0);
//    ASSERT_LE(fabs(results.second), 2000.0);
//}
//
//
//
//
//TEST(BasicTests, BasicConvergeLazyHeapUG){
//    dbg.set_debug_level(12);
//    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
//    config.lazyheap_optimization = true;
//    config.uniformgrid_optimization = true;
//    pair<double, double> results = test_convergence(&config, 1000, 1000);
//    ASSERT_GE(results.first, 10.0);
//    ASSERT_LE(results.second, 50.0);
//}
//
