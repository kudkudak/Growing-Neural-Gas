/* 
 * File:   GNGAlgorithm.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 10:02
 */

#ifndef GNGALGORITHM_H
#define	GNGALGORITHM_H

#include "GNGGraph.h"
#include "GNGDatabase.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>



//when reading - adding new edges

class GNGAlgorithm {
    double m_error; //error of the network
    int m_lambda; //lambda parameter
    double m_eps_v, m_eps_n; //epsilon of the winner and of the neighbour
    
    int m_max_nodes;
    int s;
    int c;
    
    GNGGraph m_g; //czy nie bedzie za duzo na stosie? sprawdzic miejsce potencjalnego buga
    GNGDatabase* g_db;
public:
    GNGAlgorithm(GNGDatabase* db, int start_number,boost::mutex * mutex, int lambda=1):m_g(mutex),g_db(db),c(0),s(0) ,m_max_nodes(5000){
        m_g.init(start_number);
        m_lambda=lambda;
    }
    
    double getError() const{ return m_error; }
    bool stoppingCriterion(){ return m_g.getNumberNodes()>m_max_nodes; }
    
    GNGGraph * get_graph(){ return &m_g; }
    

    
    void IncreaseError(GNGNode * node, double error){
        node->error+=error;
    }
//R
    
    void TotallyRandomAdd(){

        GNGExample  ex = g_db -> drawExample();
        
       
        
        m_g.newNode(&ex.position[0]);
       // m_g.addDEdge(__int_rnd(0,m_g.getNumberNodes()-1),0);
        
       
        int a=-1,b=-1;
        
         a = __int_rnd(0,m_g.getNumberNodes()-1);
          b = __int_rnd(0,m_g.getNumberNodes()-1);     
        
        while(a==b){
          a = __int_rnd(0,m_g.getNumberNodes()-1);
          b = __int_rnd(0,m_g.getNumberNodes()-1);
        }
          
        
        m_g.addUDEdge(a,b);
       
        dbg.push_back(1,"GNGAlgorithm:: add edge between "+to_string(a)+" and "+to_string(b));
        dbg.push_back(1,"GNGAlgorithm::addSuccessful");
      
        a = __int_rnd(0,m_g.getNumberNodes()-1);
        b = __int_rnd(0,m_g.getNumberNodes()-1);
        
        while(a==b){
          a = __int_rnd(0,m_g.getNumberNodes()-1);
          b = __int_rnd(0,m_g.getNumberNodes()-1);            
        }
        
        m_g.addUDEdge(a,b);
       
        dbg.push_back(1,"GNGAlgorithm:: add edge between "+to_string(a)+" and "+to_string(b));
        dbg.push_back(1,"GNGAlgorithm::addSuccessful");
        
    }
    
    void RandomDeletion(){
        int a = __int_rnd(0,m_g.getNumberNodes()-1);
        int b;
        dbg.push_back(1,"GNGAlgorithm:: erase node with outgoing edges (revs also!)"+to_string(a));
        
         FOREACH(edg,*(m_g[a]->edges)){
             m_g.removeRevEdge(a,edg);
         }
        
        m_g.deleteNode(a);
        dbg.push_back(1,"GNGAlgorithm::removalSuccessful");
           
           
    }
    
    void RandomInit(){
        GNGExample  ex1 = g_db -> drawExample();
        GNGExample  ex2 = g_db -> drawExample();
        
        m_g.newNode(ex1.position);
        m_g.newNode(ex2.position);
    }
    
    void AddNewNode(){
        
    }
    
    GNGNode * TwoNearestNodes(){
        return 0;
    }
    

    void Adapt(GNGExample * ex){
       /* GNGNode * nearest = TwoNearestNodes();
        
        double error=0.0;
        for(int i=0;i<GNGExample::N;++i)
            error+=(nearest[0]->position[i] - ex->position[i])*(nearest[0]->position[i] - ex->position[i]); //armadillo.
        
        
        IncreaseError(nearest[0],error);
        
        for(int i=0;i<GNGExample::N;++i){
            nearest[0]->position[i]+=m_eps_v*(ex->position[i]-nearest[0]->position[i]);
        }
        
        GNGEdge * edg = m_g.getFirstEdge(nearest[0]->nr);
        GNGEdge * end = edg + nearest[0]->edgesCount;
        
        for(;edg!=end;++edg){ //cumbersome - replace?
            
          for(int i=0;i<GNGExample::N;++i){
              nearest[edg->nr]->position[i]+=m_eps_n*(ex->position[i]-nearest[edg->nr]->position[i]);
          }
          
        }
        */
    }  
        
    
    
    void runAlgorithm(){ //1 thread needed to do it (the one that computes)
        c=0;
        boost::posix_time::microseconds work(1000),waitsleep(40);
        bool waited=false;

        
        //random init (2 nodes)
        RandomInit();
        
        
        //powinien sprawdzac czy juz ma zaczac.
        while(!stoppingCriterion()){
            for (s = 0; s<m_lambda ;++s){ //global counter!!
                boost::this_thread::sleep(boost::posix_time::microseconds(10000)); //to see the progress when the data is small
                
                TotallyRandomAdd();
                TotallyRandomAdd();
                RandomDeletion();
                //GNGExample  ex = g_db -> drawExample();
               // dbg.push_back(-3,"GNGAlgorithm::draw example");
               // GNGAdapt(&ex);
            }
            //GNGAddNewNode();
            ++c; //epoch
        }
    }
    
    
    
    
    virtual ~GNGAlgorithm(){}
private:
    
};

#endif	/* GNGALGORITHM_H */

