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
TEST(GraphTests, BasicGraphTest){
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
    

    cout<<"Getting size before regrowing\n"<<g[0].size()<<" "<<g[0].capacity()<<endl<<flush;

    g.deleteNode(2);
    cout<<"Checking writing before growing\n";
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
    
    cout<<"Getting size\n"<<g[0].size()<<" "<<g[0].capacity()<<endl<<flush;


    g.addUDEdge(0,1);
    g.addUDEdge(0,2);
    g.addUDEdge(2,5);
    
    cout<<"Adding edges ok. Removing edge \n"<<flush;


    ASSERT_EQ(g.isEdge(0,2), true);
    g.removeUDEdge(0,2);
    ASSERT_EQ(g.isEdge(0,1), true);
    ASSERT_EQ(g.isEdge(0,2), false);
    ASSERT_EQ(g[0].size(), 1);
    ASSERT_EQ(g[5].size(), 1);

    cout<<"Removing edge ok. Writing to graphml\n"<<flush;

    for(int i=0;i<g.getMaximumIndex();++i){
    	if(g.existsNode(i))
    		ASSERT_EQ(g[i].position[1], 0.2);
    }

    graphml = writeToGraphML(g);
    cout<<graphml;
    
    //Check memory consistency
    ASSERT_EQ(g[0].position[3], 0.4);
  
    
    DBG(10, "Test OK");
    

    cout<<"Growing\n"<<flush;


    //Check regrowing
    for(int i=0;i<20*N_start;++i){
    	cout<<"Adding "<<i<<endl;
        int idx = g.newNode(x);
        cout<<"Added. Adding edge";
        int cn = __int_rnd(0, i);
        while(!g.existsNode(cn)){
        	cn = __int_rnd(0, i);
        }
        g.addUDEdge(idx, cn);
    }
    
    cout<<g.reportPool();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

