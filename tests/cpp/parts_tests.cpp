#include "GNG.h"
#include "Utils.h"
#include "GNGGraph.h"
#include "GNGDataset.h"
#include <algorithm>
#include <utility>
#include <vector>
#include "gtest/gtest.h"
#include <boost/thread/mutex.hpp>
/** Include all tests */

using namespace std;

/*
 * Basic test
 */
TEST(GraphTests, BasicGraphTest){
	cerr<<"Testing GraphTest\n";

    GNGNode::dim = 6;
    int N_start = 1000;
    dbg.set_debug_level(0);

    DBG(1, "BasicGraphTest::dim = "+to_string(GNGNode::dim));

    boost::mutex grow_mutex;
    RAMGNGGraph<GNGNode, GNGEdge> g(&grow_mutex,  GNGNode::dim , N_start );

    ASSERT_EQ(g.existsNode(0), false);
    ASSERT_EQ(g.getNumberNodes(), 0);

    double x[6] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};

    //Just fill in
    for(int i=0;i<N_start;++i){
        int idx = g.newNode(x);
        ASSERT_EQ(idx, i);
    }

    //Enforce growing
    x[0]*=100;


    cerr<<"Getting size before regrowing\n"<<g[0].size()<<" "<<g[0].capacity()<<endl<<flush;

    g.deleteNode(2);
    cerr<<"Checking writing before growing\n";
    string graphml = writeToGraphML(g);

    int idx = g.newNode(x);
    ASSERT_EQ(idx, 2);


    ASSERT_EQ(g[N_start-2].nr, N_start-2); //Check memory consistency
    ASSERT_EQ(g.newNode(x), N_start);
    ASSERT_EQ(g.getNumberNodes(), N_start+1);
    ASSERT_EQ(g.newNode(x), N_start+1);
    ASSERT_EQ(g.getNumberNodes(), N_start+2);

    ASSERT_EQ(g.getMaximumIndex(), N_start+1);


    g.deleteNode(10);
    g.deleteNode(15);
    g.deleteNode(20);




    ASSERT_EQ(g.getNumberNodes(), N_start-1);


    /** WARNING: highly intrusive test ! Can change implementation*/
    DBG(10, "First free = "+to_string(g.firstFree));
    DBG(10, "First free = "+to_string(g.next_free[g.firstFree]));
    ASSERT_EQ(g.firstFree,20);
    ASSERT_EQ(g.next_free[g.firstFree], 15); //might fail if not doubling
    ASSERT_EQ(g.next_free[15], 10);

    DBG(10, "Test OK");

    cerr<<"Getting size\n"<<g[0].size()<<" "<<g[0].capacity()<<endl<<flush;


    g.addUDEdge(0,1);
    g.addUDEdge(0,2);
    g.addUDEdge(2,5);

    cerr<<"Adding edges ok. Removing edge \n"<<flush;


    ASSERT_EQ(g.isEdge(0,2), true);
    g.removeUDEdge(0,2);
    ASSERT_EQ(g.isEdge(0,1), true);
    ASSERT_EQ(g.isEdge(0,2), false);
    ASSERT_EQ(g[0].size(), 1);
    ASSERT_EQ(g[5].size(), 1);

    cerr<<"Removing edge ok. Writing to graphml\n"<<flush;

    for(int i=0;i<g.getMaximumIndex();++i){
    	if(g.existsNode(i))
    		ASSERT_EQ(g[i].position[1], 0.2);
    }

    graphml = writeToGraphML(g);
    cerr<<graphml;

    //Check memory consistency
    ASSERT_EQ(g[0].position[3], 0.4);


    DBG(10, "Test OK");


    cerr<<"Growing\n"<<flush;


    //Check regrowing
    for(int i=0;i<20*N_start;++i){
    	cerr<<"Adding "<<i<<endl;
        int idx = g.newNode(x);
        cerr<<"Added. Adding edge";
        int cn = __int_rnd(0, i);
        while(!g.existsNode(cn)){
        	cn = __int_rnd(0, i);
        }
        g.addUDEdge(idx, cn);
    }

    cerr<<g.reportPool();
}
using namespace std;

/*
 * Basic test
 */
TEST(DatabaseTests, BasicGraphTest){
	dbg.set_debug_level(3);

    GNGNode::dim = 6;
    boost::mutex phase_2_lock;
    unsigned int num_examples = 100, meta_data_dim = 10;
    //Probabilistic dataaset
    GNGDatasetSampling<GNGDatasetStorageRAM> dataset(
    		&phase_2_lock, GNGNode::dim, meta_data_dim, 0, -1);



    double * x = new double[num_examples*(meta_data_dim+GNGNode::dim)];

    dataset.insertExamples(x, num_examples, num_examples*(meta_data_dim+GNGNode::dim));


    GNGDatasetSampling<GNGDatasetStorageRAM> dataset2(
    		&phase_2_lock, GNGNode::dim, meta_data_dim, 1, 0);



    x = new double[num_examples*(meta_data_dim+GNGNode::dim+1)];
    for(int i=0;i<num_examples*(meta_data_dim+GNGNode::dim+1);++i){
    	x[i] = 0.2;
    }

    dataset2.insertExamples(x, num_examples, num_examples*(meta_data_dim+GNGNode::dim+1));

    x = new double[num_examples*(meta_data_dim+GNGNode::dim+1)];
    for(int i=0;i<num_examples*(meta_data_dim+GNGNode::dim+1);++i){
    	x[i] = 0.6;
    }
    dataset2.insertExamples(x, num_examples, num_examples*(meta_data_dim+GNGNode::dim+1));

    num_examples*=2;
    x = new double[num_examples*(meta_data_dim+GNGNode::dim+1)];
    for(int i=0;i<num_examples*(meta_data_dim+GNGNode::dim+1);++i){
    	x[i] = 0.8;
    }
    dataset2.insertExamples(x, num_examples, num_examples*(meta_data_dim+GNGNode::dim+1));


    for(int i=0;i<100000;++i){
		unsigned int a = dataset2.drawExample();
		unsigned int b = dataset2.drawExample();
		unsigned int c = dataset2.drawExample();


		ASSERT_LE(dataset2.getPosition(a)[0], 0.9);
		ASSERT_LE(dataset2.getPosition(b)[0], 0.9);
		ASSERT_LE(dataset2.getPosition(c)[0], 0.9);
    }
}


