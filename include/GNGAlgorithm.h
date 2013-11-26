/*
* File: GNGAlgorithm.h
* Author: staszek
*
* Created on 11 sierpień 2012, 10:02
*/

#ifndef GNGALGORITHM_H
#define GNGALGORITHM_H

#include <memory>

#include "GNGGlobals.h"

#include "GNGAlgorithmControl.h"

#include "GNGGraph.h"
#include "GNGDatabase.h"

#include "UniformGrid.h"
#include "GNGLazyErrorHeap.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>




/**
* The main class of the implementation dealing with computations.
* It should be agnostic of inner working (memory management etc.) of the graph and database.
* Also should not be concerned with locking logic.
*
* @note TODO: Implement GNG on GPU.
*/
class GNGAlgorithm {
public:

    /** Run main loop of the algorithm*/
    void runAlgorithm();
    
    /**Construct main algorithm object, that will hold mid-results
    * @param g SHGNGGraph object implementing graph interface
    * @param db GNGDatbase object
    * @param control GNGAlgorithmControl object (*warning*: will become obsolete in near future)
    * @param boundingbox_origin Starting point for reference system
    * @param boundingbox_axis Axis lengths for reference system
    * @param l Starting box size for uniform grid. Advised to be set to axis[0]/4 (TODO: move to the end of parameters list)
    * @param max_nodes Maximum number of nodes
    * @param max_age Maximum age of edge
    * @param alpha See original paper(TODO: add description)
    * @param betha See original paper (TODO: add description)
    * @param lambda Every lambda new vertex is added
    * @param eps_v See original paper(TODO: add description)
    * @param eps_n See original paper (TODO: add description)
    */
    GNGAlgorithm(SHGNGGraph & g,GNGDatabase* db, GNGAlgorithmControl * control,
        double * boundingbox_origin, double * boundingbox_axis, double l,int max_nodes=1000,
        int max_age=200, double alpha=0.95, double betha=0.9995, double lambda=200,
        double eps_v=0.05, double eps_n=0.0006);
   
    
    double getAccumulatedError() const { return m_accumulated_error; }
    const SHGNGGraph & get_graph(){ return m_g; }
    
    void resetUniformGrid(double * orig, double *axis, double l) {
        ug.purge(orig,axis,l);
        int maximum_index = m_g.getMaximumIndex();

        REP(i, maximum_index + 1) {
            if (m_g[i].occupied) ug.insert(m_g[i].position, m_g[i].nr);
        }
    }
    
    
    
    void setToggleUniformGrid(bool value){ m_toggle_uniformgrid=value;}
    void setToggleLazyHeap(bool value){ m_toggle_lazyheap=value;}
    void setMaxNodes(int value){m_max_nodes = value;}
    
    
    /**
     * Set utility option
     * @note This is not optimized version and is here only for research purposes. In particular
     * you cannot use this feature with uniform grid nor lazy heap optimizations
     * @param option None or BasicUtility
     * @param k
     */
    void setUtilityOption(int option, double k =-1.0){
        m_utility_option = option;
        m_utility_k = k;
#ifdef DEBUG
        dbg.push_back(4, "GNGAlgorithm::setUtilityOption k="+to_string<double>(k));
#endif
        if(m_utility_option !=0 && (m_toggle_uniformgrid==true || m_toggle_lazyheap==true)) throw "Exception"; //todo: poprawic
        if(option != 0){
            m_local_utility.clear();
            m_local_utility.resize(100);
        }
    }
    
    int CalculateAccumulatedError();
    void TestAgeCorrectness();
    enum UtilityOptions{
        None,
        BasicUtility
    };     
    virtual ~GNGAlgorithm(){}
private:
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
     
    bool stoppingCriterion(){ return m_g.getNumberNodes()>m_max_nodes; }
    
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
    
    double GetMaximumError() const{
        double max_error = 0;
        int maximumIndex = m_g.getMaximumIndex();
        REP(i,maximumIndex+1){
            if(m_g[i].occupied){
                max_error = std::max(max_error, m_g[i].error);
            }
        }
        return max_error;
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
    
    
    
    // Note: this code is not optimal and is inserted only for research purposes
    //TODO: optimize
    vector<double> m_local_utility;
    double m_utility_k;
    int m_utility_option;
    
    double get_utility(int i){
        if(i+1>m_local_utility.size()) throw "Illegal utility access";
        return m_local_utility[i];
    }
    
    void set_utility(int i, double u){
        if(i+1>m_local_utility.size()){
            #ifdef DEBUG
           dbg.push_back(2,"GNGAlgorithm::set_utility resizing");
           #endif
            m_local_utility.resize(2*m_local_utility.size());
        }
        
        m_local_utility[i] = u;
    }
    
    void utility_criterion_check(){
        
        if(m_g.getNumberNodes()<10) return; //just in case
        
        double max_error = this->GetMaximumError();
        int maximumIndex = m_g.getMaximumIndex();
        for(int i=0;i<=maximumIndex;++i){
            if (m_g[i].occupied && max_error/get_utility(i)>m_utility_k){
                if(m_g.getNumberNodes()<10) return;
                #ifdef DEBUG
                dbg.push_back(2,"GNGAlgorithm::utility_criterion_check Not passed for "+to_string<int>(i));
                dbg.push_back(2,to_string<double>(max_error));
                #endif

                //replace with while please..
                FOREACH(edg, (m_g[i].edges)) {
                        edg = m_g.removeEdge(i, edg);
                        if(edg != m_g[i].edges.end()) --edg;
                        else break;
                }
                
                
                m_g.deleteNode(i);
                set_utility(i,0);
            }
        }
        
    }
     void decrease_all_utility(){
        int maximumIndex = m_g.getMaximumIndex();
        for(int i=0;i<=maximumIndex;++i){
            if (m_g[i].occupied) {
                set_utility(i, get_utility(i)*(m_betha)); //INACZEJ NIZ W BOORU.NET
            }
        }
        
    }   
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

typedef boost::posix_time::ptime Time;
typedef boost::posix_time::time_duration TimeDuration;

#endif