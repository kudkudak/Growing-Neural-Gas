/* 
 * File:   GNGAlgorithm.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 10:02
 */

#ifndef GNGALGORITHM_H
#define	GNGALGORITHM_H

#include <memory>

#include "GNGGlobals.h"

#include "GNGAlgorithmControl.h"

#include "GNGGraph.h"
#include "GNGDatabase.h"

#include "UniformGrid.h"
#include "GNGLazyErrorHeap.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeDuration;





/**
 * The main class of the implementation dealing with computations.
 * It should be agnostic of inner working (memory management etc.) of the graph and database.
 * Also should not be concerned with locking logic.
 * 
 * @note TODO: Implement GNG on GPU.
 */
class GNGAlgorithm { 
    typedef std::list<int> Node;
    
    double m_error; //error of the network
    int m_lambda; //lambda parameter
    double m_eps_v, m_eps_n; //epsilon of the winner and of the neighbour
    int m_max_age;
    int m_max_nodes;
    
    bool m_toggle_uniformgrid,m_toggle_lazyheap;
    
    double m_alpha,m_betha;
    double * m_betha_powers;
    double * m_betha_powers_to_n;
    int m_betha_powers_size;
    double m_accumulated_error;
    
    std::map<std::string, long int> times;
    
    double m_density_threshold,m_grow_rate;
  
    int s,c;
    
    SHGNGGraph & m_g; 
    GNGDatabase* g_db;
    GNGAlgorithmControl * m_control;
 
    UniformGrid< std::vector<Node>, Node, int> ug;
    GNGLazyErrorHeap errorHeap;

    
    SHGNGNode ** LargestErrorNodesLazy();
    
    SHGNGNode ** LargestErrorNodes();
    
    /**
     * @brief Return two closest nodes (neurons) to the given example
     * @param[in] position Vector of coordinates of the example 
     */
    SHGNGNode ** TwoNearestNodes(const double * position); 
    
    void RandomInit(); 
    
    void AddNewNode();
    
    void Adapt(GNGExample * ex);
    
    void ResizeUniformGrid();
     
    
    void IncreaseErrorNew(SHGNGNode * node, double error){
        FixErrorNew(node);        
        node->error_new+=m_betha_powers[m_lambda-s]*error;
        errorHeap.updateLazy(node->nr);
    }
    
    void FixErrorNew(SHGNGNode * node){
        if(node->error_cycle==c) return;

        node->error_new = m_betha_powers_to_n[c - node->error_cycle] * node->error_new;
        node->error_cycle = c; 
    }
    
    void DecreaseAllErrorsNew(){
        return;
    }
    
    void DecreaseErrorNew(SHGNGNode * node){
        FixErrorNew(node);
        node->error_new = m_alpha*node->error_new;
        //cout<<node->error_new<<" ?? "<<node->error<<" after decrease\n";
        errorHeap.updateLazy(node->nr);
    }
    
    void SetErrorNew(SHGNGNode * node, double error){
        node->error_new = error;
        node->error_cycle = c;
        errorHeap.insertLazy(node->nr);
    }

    
    void IncreaseError(SHGNGNode * node, double error){
        node->error+=error;
    }

    void DecreaseAllErrors(){
        int maximumIndex = m_g.getMaximumIndex();
        REP(i,maximumIndex+1){
            if(m_g[i].occupied){
                m_g[i].error = m_betha*m_g[i].error;
            }
        }
    }
    
    void DecreaseError(SHGNGNode * node){
        node->error = m_alpha*node->error;
    }
            
    void SetError(SHGNGNode * node, double error){
        node->error = error;
    }      
public:
    void setToggleUniformGrid(bool value){ m_toggle_uniformgrid=value;}
    void setToggleLazyHeap(bool value){ m_toggle_lazyheap=value;}
    void setMaxNodes(int value){m_max_nodes = value;}
    
    
    GNGAlgorithm(SHGNGGraph & g,GNGDatabase* db, GNGAlgorithmControl * control, 
            int start_number,double * boundingbox_origin, double * boundingbox_axis, double l,int max_nodes=1000,

        	int max_age=200, double alpha=0.95, double betha=0.9995, double lambda=200,
        		double eps_v=0.05, double eps_n=0.0006

    );
    double getError() const{ return m_error; }
    bool stoppingCriterion(){ return m_g.getNumberNodes()>m_max_nodes; }
    double getAccumulatedError() const {   return m_accumulated_error; }   
    const SHGNGGraph & get_graph(){ return m_g; }
    
    void resetUniformGrid(double * orig, double *axis, double l) {
        ug.purge(orig,axis,l);
        int maximum_index = m_g.getMaximumIndex();

        REP(i, maximum_index + 1) {
            if (m_g[i].occupied) ug.insert(m_g[i].position, m_g[i].nr);
        }       
    }
    
    
    

    int CalculateAccumulatedError();
    int CalculateAccumulatedErrorNew() ;
    void TestAgeCorrectness();  
   
    void runAlgorithm();
   
    virtual ~GNGAlgorithm(){}
private:
    
};




/**Design hack for passing distance function dist(index, position)*/
struct GNGGraphAccessHack{
    static SHGNGNode * pool;
    static double dist(int index, double *position){
        double x=0.0;
        //arma!
        REP(i,GNG_DIM){
            x+=(pool[index].position[i] - position[i])*(pool[index].position[i] - position[i]);
        }
        return x;
    }
};






#endif	/* GNGALGORITHM_H */



