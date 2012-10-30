#include "Utils.h"

void __init_rnd(){
  srand(time(NULL));
}

int __rnd(int min, int max){
	return (rand()%(max-min+1) + min);
}


int __int_rnd(int min, int max){
	return (rand()%(max-min+1) + min);
}


double __double_rnd(double min, double max){
	return min+(max-min)*((double)rand())/RAND_MAX;
}