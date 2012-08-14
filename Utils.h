/* 
 * File:   Utils.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 16:53
 */

#ifndef UTILS_H
#define	UTILS_H

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>

using namespace std;
typedef vector<int> VI;
typedef long long LL;
#define FOR(x, b, e) for(int x=b; x<=(e); ++x)
#define FORD(x, b, e) for(int x=b; x>=(e); ––x)
#define REP(x, n) for(int x=0; x<(n); ++x)
#define VAR(v,n) typeof(n) v=(n)
#define ALL(c) c.begin(),c.end()
#define SIZE(x) (int)(x).size()
#define FOREACH(i,c) for(VAR(i,(c).begin());i!=(c).end();++i)
#define PB push_back
#define ST first
#define ND second

#define REPORT(x) cout<<#x<<"="<<(x)<<endl;

#include <sstream>
#include <string>



//c-like random functions
#include <stdlib.h>
#include <time.h>


void __init_rnd();
int __rnd(int min, int max);
int __int_rnd(int min, int max);

#include "DebugCollector.h"


template<class T>
std::string to_string(T& x){
    stringstream ss;
    ss<<x;
    return ss.str();
}



#endif	/* UTILS_H */

