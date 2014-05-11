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

/**
 * Basic interface for Node in GNGGraph.
 * 
 * Every inheriting class should implement = operator, but note, that = operator
 * doesn't copy edges
 */
class GNGNode : public std::vector<GNGEdge*> {
public:
    //TODO: change to GNGEdge, no need for dandling pointers
    typedef std::vector<GNGEdge*>::iterator EdgeIterator;

    static int dim;

    int error_cycle;
    double error;
    int edgesCount;
    double * position;
    int nr;
    bool _position_owner;


    
    /*
     * Construct GNGNode - please remember to pass pointer to position,
     * if you do not want GNGNode to use your storage pass 0.
     * 
     */
    GNGNode(double * position) :
    _position_owner(false), error(0.0), error_cycle(0), 
     nr(-1), edgesCount(0), position(position) {

        this->reserve(GNGNode::dim);
        if (!position) {
            position = new double[dim];
            _position_owner = true;
        }
    }

    GNGNode(const GNGNode & rhs){

        error_cycle = rhs.error_cycle;
        error = rhs.error;
        edgesCount = rhs.edgesCount;
        position = rhs.position;
        nr = rhs.nr;
        _position_owner = rhs._position_owner;

    	std::vector<GNGEdge*>::operator=(rhs);

    	this->reserve(GNGNode::dim);
    }


    /*
     * Construct empty GNGNode - not initialized storage!
     */
    GNGNode() : _position_owner(false), error(0.0), error_cycle(0),
    nr(-1), edgesCount(0), position(0) {

    }

    ~GNGNode() {
        if (_position_owner) delete[] position;
    }

    void reset(){
        error = 0.0;
        error_cycle = 0;
        nr = 0;
        position = 0;
        this->clear();
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
        for (int i = 0; i < dim; ++i) {
            out << node.position[i] << ",";
        }
        out << ")";

        return out;
    }
};




#endif        /* SHGRAPHDEFS_H */
