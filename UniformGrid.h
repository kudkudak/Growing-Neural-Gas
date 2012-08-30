/* 
 * File:   UniformGrid.h
 * Author: staszek
 *
 * Created on August 19, 2012, 7:02 AM
 */

#ifndef UNIFORMGRID_H
#define	UNIFORMGRID_H

#include <iostream>
#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

#include "GNGGlobals.h"

extern int GNG_DIM; //zmienne
extern DebugCollector dbg;

template<class VectorContainer, class ListContainer, class T = int>
class UniformGrid {     
    typedef ListContainer Node;
    typedef VectorContainer NodeArray;

private:
    double (*m_dist_fnc)(T,double*); //distance function;

    double m_l;
    double m_h;

    double m_density;
    double m_density_threshold;
    double m_grow_factor;
    
    double m_axis[GNG_MAX_DIM];

    int m_nodes;

    int m_dim[GNG_MAX_DIM]; //number of uniform cells along certain axis

    vector< int > m_neigh;


    double m_origin[GNG_MAX_DIM];

    double m_tmp_double[GNG_MAX_DIM];
    int m_tmp_int[GNG_MAX_DIM];

    int * _calculateCell(double *p) {
        int * tmp_int = new int[GNG_DIM];
        for (int i = 0; i < GNG_DIM; ++i) {
            tmp_int[i] = (int) ((p[i] - m_origin[i]) / m_l);
        }
        return tmp_int;
    }

    int _getIndex(int * tmp_int) {
        for (int i = GNG_DIM - 1; i > 0; --i) {
            tmp_int[i - 1] += tmp_int[i] * m_dim[i];
        }
        return tmp_int[0];
    }

    bool _inside(int x) {
        return (x) >= 0 && (x) < m_grid.size();
    }

    int _indx(int m, int n) {
        int i = 0;
        while (m % n == 0) {
            ++i;
            m = m / n;
        }
        return i;
    }
    
    bool isZero(double x){
        return x>-EPS && x <EPS;
    }
    void purge(double *origin, int* dim, double l);
public:

    void print3d(){
        using namespace std;
        int index[3];
        
        REP(k,m_dim[2]){
            REP(j,m_dim[1]){
                REP(i,m_dim[0]){
                    index[0]=i; index[1]=j; index[2]=k;
                    
                    int inner_index = _getIndex(index);
                    
                    cout<<inner_index<<":";
                    
                    FOREACH(x,m_grid[inner_index]) cout<<*x<<",";
                    cout<<"\t";
                           
                    
                }
                cout<<endl;
            }
            cout<<"\n\n\n";
        }
    
    
    }
    
    double getCellLength() const{
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


    //unify!
    NodeArray m_grid;
    std::vector<bool> m_grid_lookup;

    void new_l(double l){
        double *org = new double[3];
        double *axis = new double[3];
        memcpy(org,m_origin,GNG_DIM*sizeof(double));
        memcpy(axis,m_axis,GNG_DIM*sizeof(double));
        purge(org,axis,l);
        delete[] org;
        delete[] axis;
    }
    
    void purge(double *origin, double *axis, double l);
    

    UniformGrid(double * origin, int *dim, double l) {
        purge(origin, dim, l);
    }

    UniformGrid(double * origin, double *axis, double l) {
        purge(origin, axis, l);
    }
    //mutates pos!

    int insert(double *p, T x);
    void remove(double *p);
    T find(double *p);
    
    T * findTwoNearest(double *p,bool debug=false);
    
    void setDistFunction(double (*dist_fnc)(T,double*)){
        m_dist_fnc = dist_fnc;
    }

    //jedna funkcja uzywa m_tmp_int

    int getIndex(int *p) {
        //not working for dim>2 idiot!

        memcpy(&m_tmp_int[0], p, sizeof (double) *GNG_DIM);
        for (int i = GNG_DIM - 1; i > 0; --i) {
            m_tmp_int[i - 1] = m_tmp_int[i - 1] + m_tmp_int[i] * m_dim[i];
        }
        return m_tmp_int[0];
    }


};





template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer,ListContainer,T>::purge(double *origin, double *axis, double l) {
    int * dim = new int[GNG_DIM];
    memcpy(m_axis,axis,sizeof(double)*GNG_DIM);
    REP(i, GNG_DIM) {
        //REPORT(((axis[i] - origin[i]) / (l)));
        dim[i] = (int) ((axis[i] - origin[i]) / (l)) + 1;
    }
    purge(origin, dim, l);
    delete[] dim;
}

template<class VectorContainer, class ListContainer, class T>
void UniformGrid<VectorContainer,ListContainer,T>::purge(double *origin, int* dim, double l) {
    m_l = l;
    REPORT(m_l);
    memcpy(&m_dim[0], dim, sizeof (int) *GNG_DIM);
    memcpy(&m_origin, origin, sizeof (double) *GNG_DIM);
    m_density = 0.0;
    m_density_threshold = 0.1;
    m_grow_factor = 1.5;
    m_nodes = 0;
    //dbg.push_back(2, "purge..");
    m_neigh.clear();

    m_grid.clear();
    m_grid_lookup.clear();
    //dbg.push_back(2, "purge ok..");
   // cout<<"NEW DIMS:";
   // write_array(m_dim,m_dim+GNG_DIM);
   // write_array(m_origin,m_origin+GNG_DIM);
  // write_array(m_axis,m_axis+GNG_DIM);
    int new_size = 1;

    REP(i, GNG_DIM) {
        new_size *= m_dim[i];
    }

    m_grid.resize(new_size);
    m_grid_lookup.resize(new_size);

    REPORT(new_size);
    
    FOREACH(cell, m_grid_lookup) (*cell) = false;


    int *w = new int[GNG_DIM];
    int *w_tmp = new int[GNG_DIM];
    REP(i, GNG_DIM) w[i] = 0;
    int *skok = new int[GNG_DIM];
    REP(i, GNG_DIM) skok[i] = 1;

    int m = 0, j = 0, k = GNG_DIM, n = 3;

    do {


        REP(i, GNG_DIM) w_tmp[i] = w[i] - 1;
        m_neigh.push_back(_getIndex(w_tmp));

        ++m;
        j = _indx(m, n) + 1;

        if (j <= k) {
            w[j - 1] += skok[j - 1];
            if (w[j - 1] == 0) skok[j - 1] = 1;
            if (w[j - 1] == n - 1) skok[j - 1] = -1;
        }
    } while (j <= k);

    cout << "GENERATED ";
    write_cnt(m_neigh.begin(), m_neigh.end());
}


//mutates pos!
template<class VectorContainer, class ListContainer, class T>
int UniformGrid<VectorContainer,ListContainer,T>::insert(double *p, T x) {
    //memcpy(&m_copy[0],p,sizeof(double)*GNG_DIM);
    int * index = _calculateCell(p);
    int k = _getIndex(index);
    //dbg.push_back(0,"UniformGrid:: "+to_string(k)+" inser");
    m_grid[k].push_back(x);
    m_nodes++;
    m_density = (double) m_nodes / (double) SIZE(m_grid);
    return k;
}
template<class VectorContainer, class ListContainer, class T >
T * UniformGrid<VectorContainer,ListContainer,T>::findTwoNearest(double *p,bool debug) { //returns indexes (values whatever)
    std::list<int> search_query, checked_cells;
    double b, tmp;

    T * nearest = new T[GNG_DIM];
    nearest[0]=-1;
    nearest[1]=-1;
    double best_dist[2];
    bool occupied[2] = {false, false};
    
    //end of defs



    int * index = _calculateCell(p);



    if(debug){
    cout << "search for";
    write_array(p, p + GNG_DIM);
    write_array(index, index + GNG_DIM);
    }

    for (int i = 0; i < GNG_DIM; ++i) {
        tmp = abs((p[i] - m_origin[i] - index[i] * m_l)) < abs((p[i] - m_origin[i] - (index[i] + 1) * m_l)) ?
                abs((p[i] - m_origin[i] - index[i] * m_l)) :
                abs((p[i] - m_origin[i] - (index[i] + 1) * m_l));
       // cout << tmp << endl;
        if (b > tmp || i == 0) b = tmp;
    }
    if(debug) cout << "closest b = " << b << endl;
    int k = _getIndex(index);
    if(debug) cout<<"Index = "<<k<<endl;
//
    //searching with given b
    search_query.push_back(k);
    int iteration = 0, radius = 0, number_of_cells = 1;

    while (1) {
       if(debug) cout<< "commence radius = " + to_string(radius) + " b = " + to_string(b)<<endl;
        //if(++iteration>2) break;

        FOREACH(cell, search_query) {

            FOREACH(it, m_grid[*(cell)]) {
                double distcompare = m_dist_fnc(*it, p);
               if(debug) cout<<*it<<" distcompare=" + to_string(distcompare)<<endl;
          
                if (!occupied[0] || best_dist[0] > distcompare) {

                    if (occupied[0]) {
                        nearest[1] = nearest[0];
                        best_dist[1] = best_dist[0];
                        occupied[1] = true;
                    }

                    nearest[0] = *it;
                    best_dist[0] = distcompare;
                    occupied[0] = true;
                } else if ((!occupied[1] || best_dist[1] > distcompare) && (*it) != nearest[0]) {
                    nearest[1] = *it;
                    best_dist[1] = distcompare;
                    occupied[1] = true;
                }
            }
            checked_cells.push_back(*cell);
            m_grid_lookup[*cell] = true;
        }
        //dbg.push_back(2, "search result " + to_string(best_dist[0]) + " " + to_string(best_dist[1]));
        if (occupied[0] && occupied[1] && best_dist[0] < b && best_dist[1] < b) break; //within this cell for 100%

        //adding surrounding cells

        b = b + m_l;
        ++radius;

        //adding new cells (klocenie sie o stale, dim^n-1 dodatkowej roboty (wspolne) )
        list<int>::iterator it = search_query.begin(); //invariant: not empty		
        std::list<int> new_search_query;

        int new_number = 0;

        for (int i = 0; i < number_of_cells; ++i) {

            FOREACH(n, m_neigh) {
                if (_inside(*it + *n))
                    if (m_grid_lookup[*it + *n] == false) {
                        ++new_number;
                        m_grid_lookup[*it + *n] = true;
                        new_search_query.push_back(*it + *n);
                    }
            }
            ++it;
        }

        //cout << "\n\n\n\n";


        //clearing search query
        search_query = new_search_query;
        number_of_cells = new_number;


        if(debug){
        cout << "checked";
        FOREACH(cell, checked_cells) cout << *cell << ",";
        cout << endl;
        }


        //cout << "to check";
        //FOREACH(cell, search_query) cout << *cell << ",";
        //cout << endl;

        if (number_of_cells == 0) break;
    }
    FOREACH(cell, checked_cells) m_grid_lookup[*cell] = false;
    return nearest;

}



template<class VectorContainer, class ListContainer, class T >
void UniformGrid<VectorContainer,ListContainer,T>::remove(double *p) { //returns indexes (values whatever)
    std::list<int> search_query, checked_cells;
    int * index = _calculateCell(p);
    int k = _getIndex(index);

    search_query.push_back(k);
    int iteration = 0, radius = 0, number_of_cells = 1;

    while (1) {
        
       // REPORT(radius);
        FOREACH(cell, search_query) {
            
            // cout<<"commence search " <<  *cell<<"\n"; 
            FOREACH(it, m_grid[*(cell)]) {
                double distcompare = m_dist_fnc(*it, p);
               // REPORT(distcompare);
                if (isZero(distcompare)) {m_grid[*cell].erase(it); m_nodes--;
                m_density = (double) m_nodes / (double) SIZE(m_grid); return;}
            }
            checked_cells.push_back(*cell);
            m_grid_lookup[*cell] = true;
        }
  

        ++radius;

      
        std::list<int>::iterator it = search_query.begin(); //invariant: not empty		
        std::list<int> new_search_query;

        int new_number = 0;

        for (int i = 0; i < number_of_cells; ++i) {

            FOREACH(n, m_neigh) {
                if (_inside(*it + *n))
                    if (m_grid_lookup[*it + *n] == false) {
                        ++new_number;
                        m_grid_lookup[*it + *n] = true;
                        new_search_query.push_back(*it + *n);
                    }
            }
            ++it;
        }

        search_query = new_search_query;
        number_of_cells = new_number;

        if (number_of_cells == 0) break;
    }
    FOREACH(cell, checked_cells) m_grid_lookup[*cell] = false;
}


template<class VectorContainer, class ListContainer, class T >
T UniformGrid<VectorContainer,ListContainer,T>::find(double *p) { //returns indexes (values whatever)
    std::list<int> search_query, checked_cells;
    int * index = _calculateCell(p);
    int k = _getIndex(index);

    search_query.push_back(k);
    int iteration = 0, radius = 0, number_of_cells = 1;

    while (1) {
        
       // REPORT(radius);
        FOREACH(cell, search_query) {
            
            // cout<<"commence search " <<  *cell<<"\n"; 
            FOREACH(it, m_grid[*(cell)]) {
                double distcompare = m_dist_fnc(*it, p);
               // REPORT(distcompare);
                if (isZero(distcompare)) {  return *cell;}
            }
            checked_cells.push_back(*cell);
            m_grid_lookup[*cell] = true;
        }
  

        ++radius;

      
        std::list<int>::iterator it = search_query.begin(); //invariant: not empty		
        std::list<int> new_search_query;

        int new_number = 0;

        for (int i = 0; i < number_of_cells; ++i) {

            FOREACH(n, m_neigh) {
                if (_inside(*it + *n))
                    if (m_grid_lookup[*it + *n] == false) {
                        ++new_number;
                        m_grid_lookup[*it + *n] = true;
                        new_search_query.push_back(*it + *n);
                    }
            }
            ++it;
        }

        search_query = new_search_query;
        number_of_cells = new_number;

        if (number_of_cells == 0) break;
    }
    FOREACH(cell, checked_cells) m_grid_lookup[*cell] = false;
    return T();
}



#endif	/* UNIFORMGRID_H */

