

/* 
 * File:   UniformGrid.h
 * Author: staszek
 *
 * Created on August 19, 2012, 7:02 AM
 */

#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H


#include "Utils.h"


#include "GNGGlobals.h"

extern DebugCollector dbg;

/**
 * Is not thread safe !! Very important (speed purposes)
 */
template<class VectorContainer, class ListContainer, class T = int>
class UniformGrid {
    typedef ListContainer Node;
    typedef VectorContainer NodeArray;

public:
    NodeArray m_grid;

    //global variables for search query
    int s_found_cells[4];
    double s_found_cells_dist[4];
    int s_search_query;

    int s_center[GNG_MAX_DIM];
    int s_radius;
    int s_pos[GNG_MAX_DIM];
    double s_query[GNG_MAX_DIM];


    double (*m_dist_fnc)(T, double*); //distance function;

    double m_l;
    double m_h;

    double m_density;
    double m_density_threshold;
    double m_grow_factor;

    double m_axis[GNG_MAX_DIM];

    int m_nodes;

    int m_dim[GNG_MAX_DIM]; //number of uniform cells along certain axis

    int m_tmp_int[GNG_MAX_DIM]; //avoid alloc on heap all the time in calccell <- one thread!

    vector< int > m_neigh;


    double m_origin[GNG_MAX_DIM];

    int getIndex(int *p) {
        //not working for dim>2 idiot!
        int value = p[0];
        double mul = m_dim[0];
        for (int i = 1; i < GNG_DIM; ++i) {
            value += p[i] * mul;
            mul *= m_dim[i];
        }

        return value;
    }
    int * calculateCell(const double *p) {
        //int * m_tmp_int = new int[GNG_DIM];
        for (int i = 0; i < GNG_DIM; ++i) {
            m_tmp_int[i] = (int) ((p[i] - m_origin[i]) / m_l);
        }
        return &m_tmp_int[0];
    }

    bool _inside(int x) {
        return (x) >= 0 && (x) < m_grid.size();
    }

    bool isZero(double x) {
        return x>-EPS && x <EPS;
    }
    void purge(double *origin, int* dim, double l);
public:

    UniformGrid(double * origin, int *dim, double l) {
        purge(origin, dim, l);
    }

    UniformGrid(double * origin, double *axis, double l) {
        purge(origin, axis, l);
    }    
   
    void print3d();

    bool searchSuccessful(double min_dist = -1) {
        REP(i, s_search_query) {
            if (s_found_cells[i] == -1 || s_found_cells_dist[i] > min_dist) return false;
        }
        return true;
    }

    double getCellLength() const {
        return m_l;
    }

    double getDensity() const {
        return m_density;
    }

    int getCapacity() const {
        return SIZE(m_grid);
    }

    int getNodes() const {
        return m_nodes;
    }

    int getDimension(int axis) const {
        return m_dim[axis];
    }

    void new_l(double l);
    
    void purge(double *origin, double *axis, double l);
    
    

    //mutates pos!

    int insert(double *p, T x);
    bool remove(double *p);
    T find(double *p);

    T * findNearest(const double *p, int n = 2);

    void setDistFunction(double (*dist_fnc)(T, double*)) {
        m_dist_fnc = dist_fnc;
    }

    //jedna funkcja uzywa m_tmp_int


    void scanCell(int k, double* query);
    void crawl(int current_dim, int fixed_dim);
    bool scanCorners();

};

#include "UniformGrid.hpp"




#endif