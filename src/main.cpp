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

using namespace std;

int GNGNode::dim =0; 

#include "tests/cpp/basic_convergence.cpp"

int main(int argc, char** argv) {
    dbg.set_debug_level(12);
    testNewInterface();

    
    
    return 0;
}

