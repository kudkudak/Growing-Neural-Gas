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

int GNGNode::dim =0; 
using namespace std;

#include "gtest/gtest.h"
#include "../tests/cpp/basic_convergence.cpp"

int main(int argc, char** argv) {
    dbg.set_debug_level(1);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    pair<double, double> results = test_convergence(&config, 1000, 30000);
    cout<<results.first<<","<<results.second<<endl;
//    ASSERT_GE(fabs(results.first), 60.0);
//    ASSERT_LE(fabs(results.second), 50.0);

    return 0;
}

