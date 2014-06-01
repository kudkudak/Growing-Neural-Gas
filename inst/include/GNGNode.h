/*
 * File: SHGraphDefs.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 08:18
 */

#ifndef SHGRAPHDEFS_H
#define SHGRAPHDEFS_H



#include <boost/interprocess/containers/list.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include "SHMemoryManager.h"
#include "GNGDataset.h"
#include "GNGGlobals.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iterator>

/**
 * Basic interface for Edge in GNGGraph.
 */
class GNGEdge {
public:
    GNGEdge * rev;
    int nr;
    double error;
    int age;

    GNGEdge() : error(0), age(0) {
    }

    GNGEdge(int nr) : nr(nr), error(0.0), age(0) {
    }
};


//Dump format [N nodes] [dim] (doubles) N * node->dump()

///Warning dev note: GNGNode fields have to be properly copied/initialized form 
///both GNGAlgorithm and 2 functions in GNGNode
///I should improve it shortly
class GNGNode : public std::vector<GNGEdge*> {
public:
    //TODO: change to GNGEdge, no need for dandling pointers
    typedef std::vector<GNGEdge*>::iterator EdgeIterator;


    double utility; //0
    int error_cycle; //1
    double error; //2
    int edgesCount; //3
    int nr; //4
    bool _position_owner; //5
    unsigned int dim; //6   
    double extra_data; //7 - extra data that is voted among vertices when adapting
    double * position; //8... 8+dim-1

    


    /*
     * Construct empty GNGNode - not initialized storage! Non initialized because used only internally
     * in GNGGraph.
     */
    GNGNode(){
    	//prevent memory corruption
    	_position_owner = false;
    	position = 0 ;
    }

    ~GNGNode() {
        if (_position_owner){
        	cerr<<"Position owner, dim = "<<dim<<endl;
        	delete[] position;

        }
    }
   
    double dist(GNGNode * gnode) const { //dist doesnt account for param
        using namespace std;
        double ret = 0;
        for (int i = 0; i < dim; ++i)
            ret += (this->position[i] - gnode->position[i])*(this->position[i] - gnode->position[i]);
        return sqrt(ret);
    }

    friend std::ostream& operator<<(std::ostream& out, const GNGNode & node) {
        out << node.nr << "(" << node.error << ")(";
        for (int i = 0; i < node.dim; ++i) {
            out << node.position[i] << ",";
        }
        out << ")";

        return out;
    }



    vector<double> dumpEdges(){
        vector<double> dump(1+this->size(), 0.0);
        dump[0] = this->size();
        for(int i=0;i<this->size();++i)
        	dump[i+1] = (*this)[i]->nr;
        return dump;
    }
    
    ///Dumps to vector of numbers
    vector<double> dumpVertexData(){
        vector<double> dump(8 + dim, 0.0);
        dump[0] = utility;
        dump[1] = error_cycle;
        dump[2] = error;
        dump[3] = edgesCount;
        dump[4] = nr;
        dump[5] = (int)_position_owner;
        dump[6] = dim;
        dump[7] = extra_data;
        for(int i=0;i<dim;++i){
            dump[i+7] = position[i]; 
        }
        return dump;
    }
    //Loads from vector
    void loadVertexData(vector<double> & x, double * position_ptr){

        utility = x[0];
        error_cycle = x[1];
        error = x[2];
        edgesCount = (int)x[3];
        nr = (int)x[4];
        _position_owner = x[5];
        dim = x[6];
        extra_data = x[7];
        position = position_ptr;
        for(int i=0;i<dim;++i){
            position[i] = x[i+8];
        }       
    }
    void loadVertexData(vector<double>::iterator & itr, int gng_dim,  double * position_ptr){
    	vector<double> dump; dump.reserve(8 + dim);

    	std::copy(itr+1, itr+9+gng_dim, std::back_inserter(dump));
    	std::advance(itr, (8 + gng_dim));

    	this->loadVertexData(dump, position_ptr);
    }


};




#endif        /* SHGRAPHDEFS_H */
