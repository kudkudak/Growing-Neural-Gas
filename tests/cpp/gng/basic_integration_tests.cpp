#include "gng/GNG.h" //TODO: path problems
#include "gng/GNGServer.h"
#include "gng/Utils.h"

#include "gtest/gtest.h"
#include <algorithm>
#include <utility>
#include <vector>

using namespace std;
using namespace gmum;

unsigned int sleep_ms = 200;
/** Run GNGAlgorithm on cube (3-dimensional) with given parameters
 * @returns pair<double, double> : nodes, mean_error
 */
pair<double, double> test_convergence(GNGConfiguration * cnf=0, int num_database=1000, int ms_loop = 5000,  string save_filename="", double* extra_examples=0, int extra_samples_size=0) {

	GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization = true;
    if(cnf) config=*cnf;
    config.datasetType = GNGConfiguration::DatasetSamplingProb;

    GNGServer *s = GNGServer::constructTestServer(config);



    cerr<<s->getGraph().reportPool()<<endl;
    cerr<<config.verbosity<<endl;

    s->run();

    cerr<<"Allocating "<<(config.dim+1)*num_database<<endl<<flush;
    double * vect = new double[(config.dim+1)*num_database];
    for (int i = 0; i < num_database; ++i) {
        for(int j=0;j<= config.dim;++j)
             if(j==0)
                 vect[j+(i)*(config.dim+1)] = 0.0;
             else if(j<config.dim)
                 vect[j+(i)*(config.dim+1)] = __double_rnd(0, 1);
             else
            	 vect[j+(i)*(config.dim+1)] = 0.5;
    }


    cerr<<"Allocated examples\n";

    if(extra_examples){
        cerr<<"Adding extra examples\n";
        s->insertExamples(&extra_examples[0],
        		extra_samples_size/(config.dim+1), extra_samples_size);
        cerr<<"Database size="<<s->getDatabase().getSize()<<endl;
    }
    cerr<<"Adding main examples\n";
    s->insertExamples(&vect[0], num_database, num_database*(config.dim+1));
    cerr<<"Database size="<<s->getDatabase().getSize()<<endl;
    cerr<<"Dimensionality of example is "<<s->getDatabase().getDataDim()<<endl;
    for(int i=0;i<10;++i){
    	cerr<<"Exemplary sample (testing memory correctness):\n";
    	int ex = s->getDatabase().drawExample();
    	write_array(s->getDatabase().getPosition(ex), s->getDatabase().getPosition(ex)+(config.dim+1));
    }

    cerr<< "testNewInterface::Collecting results\n";

    int iteration = 0;

    while(true){
       ++iteration;

       REPORT_PRODUCTION(iteration);
       gmum::sleep(sleep_ms);
       REPORT_PRODUCTION(s->getGraph().get_number_nodes());
       vector<double> stats = s->getErrorStatistics();
       write_array(&stats[0], &stats[stats.size()-1]);
       REPORT_PRODUCTION(s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().get_number_nodes()+0.));
       if(iteration >= ms_loop/sleep_ms) break;
    }


    s->terminate();

    int test;
    while(s->getAlgorithm().running == true){
    	gmum::sleep(sleep_ms);
    }

    gmum::sleep(sleep_ms);

    pair<double , double> t = pair<double, double>(s->getGraph().get_number_nodes(),
            s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().get_number_nodes()+0.));


    if(save_filename!=""){
        cerr<<"BasicTests::Saving to GraphML\n";
        writeToGraphML(s->getGraph(), save_filename);
    }


    delete s;
    return t;
 }
#include <cmath>

TEST(BasicTests, BasicConvergenceUtility){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.experimental_utility_option = GNGConfiguration::UtilityBasicOn;

    pair<double, double> results = test_convergence(&config, 1000, 6000);
    ASSERT_GE(fabs(results.first), 60.0);
    ASSERT_LE(fabs(results.second), 50.0);
}

TEST(BasicTests, Serialization){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization = true;
    config.datasetType = GNGConfiguration::DatasetSamplingProb;
    int num_database = 4000;
    int ms_loop = 10000;

    GNGServer *s = GNGServer::constructTestServer(config);

    s->run();

    cerr<<"Allocating "<<(config.dim+1)*num_database<<endl<<flush;
    double * vect = new double[(config.dim+1)*num_database];
    for (int i = 0; i < num_database; ++i) {
        for(int j=0;j<= config.dim;++j)
             if(j==0)
                 vect[j+(i)*(config.dim+1)] = 0.0;
             else if(j<config.dim)
                 vect[j+(i)*(config.dim+1)] = __double_rnd(0, 1);
             else
            	 vect[j+(i)*(config.dim+1)] = 0.5;
    }


    cerr<<"Adding main examples\n";
    s->insertExamples(&vect[0], num_database, num_database*(config.dim+1));
    cerr<<"Database size="<<s->getDatabase().getSize()<<endl;
    cerr<<"Dimensionality of example is "<<s->getDatabase().getDataDim()<<endl;
    for(int i=0;i<10;++i){
    	cerr<<"Exemplary sample (testing memory correctness):\n";
    	int ex = s->getDatabase().drawExample();
    	write_array(s->getDatabase().getPosition(ex), s->getDatabase().getPosition(ex)+(config.dim+1));
    }

    cerr<< "testNewInterface::Collecting results\n";

    int iteration = 0;

    while(true){
       ++iteration;
       REPORT_PRODUCTION(iteration);
       gmum::sleep(sleep_ms);
       REPORT_PRODUCTION(s->getGraph().get_number_nodes());
       vector<double> stats = s->getErrorStatistics();
       write_array(&stats[0], &stats[stats.size()-1]);
       REPORT_PRODUCTION(s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().get_number_nodes()+0.));
       if(iteration >= ms_loop/sleep_ms) break;
    }


    s->terminate();


    int test;
    while(s->getAlgorithm().running == true){
    	gmum::sleep(sleep_ms);
    }

    gmum::sleep(sleep_ms);

    s->serializeGraph("test.graph.bin");
    delete s;


	GNGConfiguration config2 = GNGConfiguration::getDefaultConfiguration();
	config2.load_graph_filename = "test.graph.bin";
	config2.uniformgrid_optimization = true;
	pair<double, double> results = test_convergence(&config2, 1000, 1000);

	ASSERT_GE(fabs(results.first), 550.0);
	ASSERT_LE(fabs(results.second), 10.0);
}


TEST(BasicTests, BasicConvergence){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    pair<double, double> results = test_convergence(&config, 1000, 6000,
    		"basic_convergence.graphml");
    ASSERT_GE(fabs(results.first), 60.0);
    ASSERT_LE(fabs(results.second), 50.0);
}

TEST(BasicTests, FewDimsSkewedUGConvergence){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.max_nodes = 1000;
    config.lazyheap_optimization =  true;
    config.dim = 5;
    config.axis = vector<double>(config.dim , 20.0);
    config.orig = vector<double>(config.dim , -1.0);
    config.orig[2] = -4.0;

    //vector would be better here obviously.
    int num_extra=50000;
    double * extra_examples = new double[num_extra*(config.dim+1)];
    for (int i = 0; i < num_extra; ++i) {
        for(int j=0;j<= config.dim;++j)
             extra_examples[j+(i)*(config.dim+1)] = __double_rnd(0, 2)+(2.0);
    }

    pair<double, double> results = test_convergence(&config, 100000, 60000, "fewdims.graphml",
    		extra_examples, num_extra*(config.dim+1));

    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}

TEST(BasicTests, FewDimsUGConvergence){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  true;
    config.max_nodes = 2000;
    config.verbosity = 8;
    config.lazyheap_optimization =  true;
    config.dim = 4;
    config.axis = vector<double>(config.dim , 1.0);
    config.orig = vector<double>(config.dim , 0.0);



    pair<double, double> results = test_convergence(&config, 1000, 60000, "fewdimsugconvergence.graphml");

    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 5.0);
}
TEST(BasicTests, ManyDimsUGConvergence){

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
    cerr<<"BasicTests::ManyDimsNoUG"<<endl;

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  false;
    config.dim = 50;
    config.axis = vector<double>(config.dim, 1.0);
    config.orig = vector<double>(config.dim, 0.0);
    pair<double, double> results = test_convergence(&config, 100, 50000);

    ASSERT_GE(fabs(results.first), 100.0);
    ASSERT_LE(fabs(results.second), 2000.0);
}

TEST(BasicTests, BasicConvergeLazyHeapUG){

    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.lazyheap_optimization = true;
    config.max_nodes = 2000;
    config.uniformgrid_optimization = true;
    cerr<<"Checking correctness "+to_string(config.check_correctness())<<endl;
    config.check_correctness();
    pair<double, double> results = test_convergence(&config, 10000, 1000);
    ASSERT_GE(results.first, 10.0);
    ASSERT_LE(results.second, 50.0);
}
