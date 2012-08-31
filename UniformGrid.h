

#ifndef UNIFORM_GRID_H
#define UNIFORM_GRID_H

/* 
 * File:   UniformGrid.h
 * Author: staszek
 *
 * Created on August 19, 2012, 7:02 AM
 */

#include <iostream>
#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <cmath>
#include "GNGGlobals.h"

extern int GNG_DIM; //zmienne
extern DebugCollector dbg;

//vector<bool> TMP(100000);

template<class VectorContainer, class ListContainer, class T = int>
class UniformGrid {     
    typedef ListContainer Node;
    typedef VectorContainer NodeArray;
	
public:

	//global variables for search query
	int s_found_cells[4];    
	double s_found_cells_dist[4];
	int s_search_query;
        
        int s_center[GNG_MAX_DIM];
        int s_radius;
        int s_pos[GNG_MAX_DIM];
        double s_query[GNG_MAX_DIM];


	double (*m_dist_fnc)(T,double*); //distance function;

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


    int * _calculateCell(double *p) {
        //int * m_tmp_int = new int[GNG_DIM];
        for (int i = 0; i < GNG_DIM; ++i) {
            m_tmp_int[i] = (int) ((p[i] - m_origin[i]) / m_l);
        }
        return &m_tmp_int[0];
    }
    //thread safe version
    int * _calculateCellTS(double *p) {
        int * tmp_int = new int[GNG_DIM];
        for (int i = 0; i < GNG_DIM; ++i) {
            tmp_int[i] = (int) ((p[i] - m_origin[i]) / m_l);
        }
        return tmp_int;
    }


    bool _inside(int x) {
        return (x) >= 0 && (x) < m_grid.size();
    }


    bool isZero(double x){
        return x>-EPS && x <EPS;
    }
    void purge(double *origin, int* dim, double l);
public:
	NodeArray m_grid;

    void print3d(){
        using namespace std;
        int index[3];
        
        REP(k,m_dim[2]){
            REP(j,m_dim[1]){
                REP(i,m_dim[0]){
                    index[0]=i; index[1]=j; index[2]=k;
                    
                    int inner_index = getIndex(index);
                    
                    cout<<inner_index;
                    //if(TMP[inner_index]) cout<<"::"; 
				 cout<<":";
                    FOREACH(x,m_grid[inner_index]) cout<<*x<<",";
                    cout<<"\t";
                           
                    
                }
                cout<<endl;
            }
            cout<<"\n\n\n";
        }
    
    
    }
    
    bool searchSuccessful(double min_dist=-1){
		REP(i, s_search_query){
			if(s_found_cells[i]==-1 || s_found_cells_dist[i]>min_dist) return false;
		}	
		return true;
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
    bool remove(double *p);
    T find(double *p);
    
    T * findNearest(double *p,int n=2);
    
    void setDistFunction(double (*dist_fnc)(T,double*)){
        m_dist_fnc = dist_fnc;
    }

    //jedna funkcja uzywa m_tmp_int

    int getIndex(int *p) {
        //not working for dim>2 idiot!
		int value= p[0];
		double mul = m_dim[0];
        for (int i =  1; i < GNG_DIM; ++i) {
           value += p[i]*mul; mul*=m_dim[i]; }
		
        return value;
    }
	
	void scanCell(int k,double* query){
		double dist_candidate;

		FOREACH(node,m_grid[k]){
			
			dist_candidate = m_dist_fnc(*node,query);
									
			if(s_found_cells_dist[0]==-1 || dist_candidate<s_found_cells_dist[0]){
					s_found_cells_dist[0] = dist_candidate;
				    s_found_cells[0] = *node;
					
					for(int j=1;j<=s_search_query;++j) {
						if(s_found_cells_dist[j]==-1 || dist_candidate<s_found_cells_dist[j]){
							std::swap(s_found_cells[j],s_found_cells[j-1]);
							std::swap(s_found_cells_dist[j],s_found_cells_dist[j-1]);
						}			
					}
			}
		
			
		}
	}

	//globalne zmienne nie ma po co przechowywac poprawic
	void crawl(int current_dim, int fixed_dim){

		//cout<<"crawl ("<<getIndex(pos)<<")\n";
		//write_array(pos,pos+3);		
		if(current_dim==fixed_dim){
			if(current_dim>=GNG_DIM-1){
						//cout<<"crawled to "; 
						//int d=-1;						
						//REP(i,GNG_DIM){
						//	d = std::max(std::abs(pos[i] - center[i]),d);
						//}
						//cout<<"("<<getIndex(pos)<<"),"<<d;						
						scanCell(getIndex(s_pos),s_query);
						//TMP[getIndex(pos)]=true;			
			}
			//skip current dimension
			else crawl(current_dim+1,fixed_dim);		
		}	
		else{
			int from,to;
			
			//skip corners
				if(current_dim<fixed_dim){
					from = std::max(s_center[current_dim] - s_radius +1 ,0);
					to = std::min(s_center[current_dim] + s_radius -1, m_dim[current_dim]-1);
				}else{
					from = std::max(s_center[current_dim] - s_radius  ,0);
					to = std::min(s_center[current_dim] + s_radius , m_dim[current_dim]-1);
				}
				
				//cout<<"range from "<<from<<" to "<<to<<endl;

				for(int i=from;i<=to;++i){
					

					s_pos[current_dim] = i;

					if(current_dim == GNG_DIM-1){
						//cout<<"crawled to "; 
						//int d=-1;						
						//REP(i,GNG_DIM){
						//	d = std::max(std::abs(pos[i] - center[i]),d);
						//}
						//cout<<"("<<getIndex(pos)<<"),"<<d;						
						scanCell(getIndex(s_pos),s_query);
						//TMP[getIndex(pos)]=true;
					} else{
						crawl(current_dim+1,fixed_dim);					
					}
				}
					
			
  		}	


		return;
	}

	bool scanCorners(){
	
		int left,right;
		
		bool scanned=false;

	
		memcpy(s_pos,s_center,sizeof(int)*GNG_DIM);

                
		REP(i, GNG_DIM){
			left =  s_center[i] - s_radius;
			right = s_center[i] + s_radius;
			
			

			if(s_center[i]-s_radius>=0){
				s_pos[i] = left;
				//cout<<"crawler to "<<getIndex(pos)<<endl;
				//write_array(pos,pos+GNG_DIM);
				crawl(0,i);
				scanned=true;
			}
			if(s_center[i]+s_radius<m_dim[i]){
            	s_pos[i] = right;
				//cout<<"crawler to "<<getIndex(pos)<<endl;
				//write_array(pos,pos+GNG_DIM);
				crawl(0,i);
				scanned=true;
			}

                s_pos[i]=s_center[i];
		}	

		return scanned;	
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
	
	write_array(m_dim,m_dim+GNG_DIM);

    m_density = 0.0;
    m_density_threshold = 0.1;
    m_grow_factor = 1.5;
    m_nodes = 0;

    m_grid.clear();

    int new_size = 1;

    REP(i, GNG_DIM) {
        new_size *= m_dim[i];
    }

    m_grid.resize(new_size);

    REPORT(new_size);
}


//mutates pos!
template<class VectorContainer, class ListContainer, class T>
int UniformGrid<VectorContainer,ListContainer,T>::insert(double *p, T x) {
    //memcpy(&m_copy[0],p,sizeof(double)*GNG_DIM);
    int * index = _calculateCell(p);
    int k = getIndex(index);

    //dbg.push_back(0,"UniformGrid:: "+to_string(k)+" inser");
    m_grid[k].push_back(x);
    m_nodes++;
    m_density = (double) m_nodes / (double) SIZE(m_grid);
    return k;
}
template<class VectorContainer, class ListContainer, class T >


T * UniformGrid<VectorContainer,ListContainer,T>::findNearest(double *p,int n) { //returns indexes (values whatever)
	s_search_query=n;


    int * center = _calculateCell(p);
    memcpy(s_center,center,sizeof(int)*GNG_DIM);
    memcpy(s_query,p,sizeof(double)*GNG_DIM);
 
	int center_id = getIndex(center);
	double border, border_squared,tmp;
	s_found_cells_dist[0]=s_found_cells_dist[1]=-1;
	s_found_cells[0]=s_found_cells[1]=-1;
	
	//REP(i,10000) TMP[i]=false;

 	//init of the search
	scanCell(center_id,s_query);

	
	for (int i = 0; i < GNG_DIM; ++i) {
        tmp = abs((p[i] - m_origin[i] - center[i] * m_l)) < abs((p[i] - m_origin[i] - (center[i] + 1) * m_l)) ?
                abs((p[i] - m_origin[i] - center[i] * m_l)) :
                abs((p[i] - m_origin[i] - (center[i] + 1) * m_l));
       // cout << tmp << endl;
        if (border > tmp || i == 0) border = tmp;
    }
	
	border_squared = border*border;

	//REPORT(center_id);
	//write_array(center,center+GNG_DIM);
	//cout<<"\n\n";

	s_radius = 0;	
	
	while
	(
		!searchSuccessful(border_squared)
	){
		++s_radius;
		border+=m_l;
		border_squared = border*border;


		if(!scanCorners()) break; //false if no cells to check (no cell checked)
		
		//cout<<"results:\n";
		//REPORT(border);
		//write_array(m_found_cells,m_found_cells+2);
		//write_array(m_found_cells_dist,m_found_cells_dist+2); cout<<"--\n";	


	}
	


	int * ret = new int[2];
	memcpy(ret,s_found_cells,sizeof(int)*2);

    return ret;
}



template<class VectorContainer, class ListContainer, class T >
bool UniformGrid<VectorContainer,ListContainer,T>::remove(double *p) { //returns indexes (values whatever)
    int * cell = _calculateCell(p);
	int  index = getIndex(cell);
	FOREACH(node,m_grid[index]){
		if(isZero(m_dist_fnc(*node,p))){
			m_grid[index].erase(node);
                        --m_nodes;
			return true;		
		}	
	}
	return false;
}




#endif