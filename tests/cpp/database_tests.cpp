#include "Utils.h"
#include "GNGDataset.h"
#include <algorithm>
#include <utility>
#include <vector>
#include "gtest/gtest.h"
#include <boost/thread/mutex.hpp>
/** Include all tests */
#include "Utils.h"
#include "GNGGraph.h"
#include <algorithm>
#include <utility>
#include <vector>
#include "gtest/gtest.h"
#include <boost/thread/mutex.hpp>
/** Include all tests */

DebugCollector dbg;
int GNGNode::dim =0;



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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

