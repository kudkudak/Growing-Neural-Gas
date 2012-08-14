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




class GNGAlgorithm {
    double m_error; //error of the network
    int m_lambda; //lambda parameter
    
    int c;
    
    GNGGraph m_g; //czy nie bedzie za duzo na stosie? sprawdzic miejsce potencjalnego buga
    GNGDatabase* g_db;
public:
    GNGAlgorithm(GNGDatabase* db, int start_number,boost::mutex * mutex):m_g(mutex),g_db(db),c(0) {
        m_g.init(start_number);
        m_lambda=10;
    }
    
    double getError() const{ return m_error; }
    bool stoppingCriterion(){ return false; }
    
    GNGGraph * get_graph(){ return &m_g; }
    
    void GNGAdapt(GNGExample * ex){
        if(ex) m_g.newNode(ex->position);
        if(ex) m_g.addDEdge(__int_rnd(0,m_g.getNumberNodes()-1),__int_rnd(0,m_g.getNumberNodes()-1) );
    }
    void GNGAddNewNode(){
        
    }
    
    void runAlgorithm(){ //1 thread needed to do it (the one that computes)
        c=0;
        boost::posix_time::microseconds work(1000),waitsleep(40);
        bool waited=false;

        
        //powinien sprawdzac czy juz ma zaczac.
        while(!stoppingCriterion()){
            for (int s = 0; s<m_lambda ;++s){
                
              //
                boost::this_thread::sleep(boost::posix_time::microseconds(10000));
                
                if(m_g.getNumberNodes()<500) { 
                    dbg.push_back(-3,"GNGAlgorithm::draw example");
                    GNGExample  ex = g_db -> drawExample();
                    dbg.push_back(-3,"GNGAlgorithm::draw example succesful");
                    GNGAdapt(&ex); 
                }
            }
            GNGAddNewNode();
            c=c+1; //epoch
        }
    }
    
    
    
    
    virtual ~GNGAlgorithm(){}
private:
    
};

#endif	/* GNGALGORITHM_H */

