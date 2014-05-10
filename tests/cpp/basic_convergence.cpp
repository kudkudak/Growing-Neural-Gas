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
        int ms_loop = 5000,  string save_filename="", double* extra_examples=0, int extra_samples_size=0) {
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();  
    config.uniformgrid_optimization = true;
    if(cnf) config=*cnf;
    config.datasetType = GNGConfiguration::DatasetSamplingProb;
    
    GNGServer *s = GNGServer::constructTestServer(config);


    cout<<"Running server\n"<<flush;
    s->run();   
    
    //Probabilistic dataset

    //Layout pos0 pos1 pos2 .. posDim-1 prob

    cout<<"Allocating "<<(config.dim+1)*num_database<<endl<<flush;
    double * vect = new double[(config.dim+1)*num_database];
    for (int i = 0; i < num_database; ++i) {
        for(int j=0;j<= config.dim;++j)
             if(j==0)
                 vect[j+(i)*(config.dim+1)] = 0.0;
             else
                 vect[j+(i)*(config.dim+1)] = __double_rnd(0, 1);
    }
    
    
    DBG(10, "Allocated examples\n");
    cout<<"Allocated examples\n";
    
    if(extra_examples){
        DBG(14, "adding extra examples");
        s->addExamples(&extra_examples[0], extra_samples_size/(config.dim+1), extra_samples_size);
        cout<<"Database size="<<s->getDatabase().getSize()<<endl;
    }    
    
    s->addExamples(&vect[0], num_database, num_database*(config.dim+1));
    

    
    DBG(12, "testNewInterface::Server running");

   
    boost::posix_time::millisec workTime(500);

    cout<< "testNewInterface::Collecting results\n";
    
    int iteration = 0;
    
    
    
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
    
    int test;
    while(s->getAlgorithm().running == true){
        boost::this_thread::sleep(workTime);
    }

    boost::this_thread::sleep(workTime);

            
    pair<double , double> t = pair<double, double>(s->getGraph().getNumberNodes(),
            s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.));
    

    if(save_filename!=""){
        cout<<"BasicTests::Saving to GraphML\n";
        writeToGraphML(s->getGraph(), save_filename);
    }
    
 
    delete s;
    return t;
 }
#include <cmath>


TEST(BasicTests, BasicConvergence){
    dbg.set_debug_level(12);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    pair<double, double> results = test_convergence(&config, 1000, 3000,   "basic_convergence.graphml");
    ASSERT_GE(fabs(results.first), 60.0);
    ASSERT_LE(fabs(results.second), 50.0);
}

TEST(BasicTests, FewDimsSkewedUGConvergence){
    dbg.set_debug_level(10);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.max_nodes = 1000;
    config.lazyheap_optimization =  true;
    config.dim = 5;
    config.axis = vector<double>(config.dim , 20.0);
    config.orig = vector<double>(config.dim , -1.0);
    config.orig[2] = -4.0;

    //vector would be better here obviously.
    double * extra_examples = new double[50000*(config.dim+1)];
    for (int i = 0; i < 50000; ++i) {
        for(int j=0;j<= config.dim;++j)
             extra_examples[j+(i)*(config.dim+1)] = __double_rnd(0, 2)+(2.0);
    }
    
    pair<double, double> results = test_convergence(&config, 100000, 60000, "fewdims.graphml",
    		extra_examples, 50000*(config.dim+1));
    
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}


TEST(BasicTests, FewDimsUGConvergence){
    dbg.set_debug_level(6);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.max_nodes = 2000;
    config.lazyheap_optimization =  true;
    config.dim = 4;
    config.axis = vector<double>(config.dim , 1.0);
    config.orig = vector<double>(config.dim , 0.0);

    pair<double, double> results = test_convergence(&config, 1000, 60000, "fewdimsugconvergence.graphml");
    
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 5.0);
}
TEST(BasicTests, ManyDimsUGConvergence){
    dbg.set_debug_level(6);
    
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.lazyheap_optimization =  true;
    config.dim = 10;
    config.axis = vector<double>(config.dim , 1.0);
    config.orig = vector<double>(config.dim , 0.0);
    pair<double, double> results = test_convergence(&config, 100, 3000);
    
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}


TEST(BasicTests, ManyDimsNoUG){
    cout<<"BasicTests::ManyDimsNoUG"<<endl;
    dbg.set_debug_level(12);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  false;
    config.dim = 50;
    config.axis = vector<double>(config.dim, 1.0);
    config.orig = vector<double>(config.dim, 0.0);
    pair<double, double> results = test_convergence(&config, 100, 5000);
    
    ASSERT_GE(fabs(results.first), 100.0);
    ASSERT_LE(fabs(results.second), 2000.0);
}

TEST(BasicTests, BasicConvergeLazyHeapUG){
    dbg.set_debug_level(12);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.lazyheap_optimization = true;
    config.uniformgrid_optimization = true;
    pair<double, double> results = test_convergence(&config, 1000, 1000);
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}

