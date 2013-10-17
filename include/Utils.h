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
#include <list>

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
#define IWAS(x) cout<<x<<endl<<flush;
#define PB push_back
#define ST first
#define ND second

#define REPORT(x) cout<<#x<<"="<<(x)<<endl<<flush;

#include <sstream>
#include <string>
#include <map>
#include <cmath>


//c-like random functions
#include <stdlib.h>
#include <time.h>

#include "DebugCollector.h"

void __init_rnd();
int __rnd(int min, int max);
int __int_rnd(int min, int max);
double __double_rnd(double min, double max);

template<class T>
void write_array(T* begin, T*end){
	for(;begin!=end;++begin){
		std::cout<<*begin<<",";
	}
	std::cout<<endl;
}


template<class T>
void write_cnt(T begin, T end){
	for(;begin!=end;++begin){
		std::cout<<*begin<<",";
	}
	std::cout<<endl;
}

template<class T>
std::string write_cnt_str(T begin, T end){
    std::stringstream ss;
    
	for(;begin!=end;++begin){
		ss<<*begin<<",";
	}
    return ss.str();
}


template<class T>
std::string to_string(T& x){
    stringstream ss;
    ss<<x;
    return ss.str();
}

struct BasicException : public std::exception
{
   std::string s;
   BasicException(std::string ss) : s(ss) {}
   ~BasicException() throw () {} // Updated
   const char* what() const throw() { return s.c_str(); }
};

#endif	/* UTILS_H */
