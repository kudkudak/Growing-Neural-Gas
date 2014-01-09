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

#include "../tests/cpp/basic_convergence.cpp"

int main(int argc, char** argv) {
    dbg.set_debug_level(12);
//    testNewInterface();
    dbg.set_debug_level(3);
    GNGConfiguration config = GNGConfiguration::getDefaultConfiguration();
    config.uniformgrid_optimization =  false;
    config.databaseType = GNGConfiguration::DatabaseProbabilistic;
    config.dim = 3;
    config.axis = vector<double>(3, 1.0);
    config.orig = vector<double>(3, 0.0);
    pair<double, double> results = test_convergence(&config, 100, 5000);
    
    
    
    return 0;
}

