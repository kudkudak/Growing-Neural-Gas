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

#include "gtest/gtest.h"

DebugCollector dbg;

pair<double, double> test_convergence2(GNGConfiguration * cnf=0, int num_database=1000,
        int ms_loop = 5000,  string save_filename="", double* extra_examples=0, int extra_samples_size=0) {
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization = true;
    if(cnf) config=*cnf;
    config.datasetType = GNGConfiguration::DatasetSamplingProb;

    GNGServer *s = GNGServer::constructTestServer(config);


//    cerr<<"Running GNGServer3\n"<<flush;
    s->run();

    //Probabilistic dataset

    //Layout pos0 pos1 pos2 .. posDim-1 prob

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


    DBG(10, "Allocated examples\n");
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
    DBG(12, "testNewInterface::Server running");






    cerr<< "testNewInterface::Collecting results\n";

    int iteration = 0;



    while(true){
       ++iteration;
       REPORT_PRODUCTION(iteration);
       gmum::sleep(100);
       REPORT_PRODUCTION(s->getGraph().getNumberNodes());
       vector<double> stats = s->getErrorStatistics();
       write_array(&stats[0], &stats[stats.size()-1]);
       REPORT_PRODUCTION(s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.));
       if(iteration >= ms_loop/500) break;
    }


    s->getAlgorithm().terminate();

    int test;
    while(s->getAlgorithm().running == true){
    	gmum::sleep(100);
    }

    gmum::sleep(100);

    pair<double , double> t = pair<double, double>(s->getGraph().getNumberNodes(),
            s->getAlgorithm().CalculateAccumulatedError()
               /(s->getGraph().getNumberNodes()+0.));


    if(save_filename!=""){
        cerr<<"BasicTests::Saving to GraphML\n";
        writeToGraphML(s->getGraph(), save_filename);
    }


    delete s;
    return t;
 }

int main(int argc, char** argv){
//    dbg.set_debug_level(0);
//    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
//    config.uniformgrid_optimization =  true;
//    config.max_nodes = 1000;
//    config.lazyheap_optimization =  true;
//    config.dim = 5;
//    config.axis = vector<double>(config.dim , 20.0);
//    config.orig = vector<double>(config.dim , -1.0);
//    config.orig[2] = -4.0;
//
//    //vector would be better here obviously.
//    int num_extra=50000;
//    double * extra_examples = new double[num_extra*(config.dim+1)];
//    for (int i = 0; i < num_extra; ++i) {
//        for(int j=0;j<= config.dim;++j)
//             extra_examples[j+(i)*(config.dim+1)] = __double_rnd(0, 2)+(2.0);
//    }
//
//    pair<double, double> results = test_convergence2(&config, 10000, 60000, "fewdims.graphml",
//    		extra_examples, num_extra*(config.dim+1));

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
//	return 0;
}

