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
int GNGNode::dim =0;

int main(int argc, char** argv){

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
//	return 0;
}

