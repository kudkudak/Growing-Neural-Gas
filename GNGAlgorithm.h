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
    int m_max_age;
    int m_max_nodes;
    
    double m_alpha,m_betha;
    
    
    int s;
    int c;
    
    GNGGraph m_g; //czy nie bedzie za duzo na stosie? sprawdzic miejsce potencjalnego buga
    GNGDatabase* g_db;
    
    
    
       
    GNGNode ** LargestErrorNodes(){
        
        GNGNode ** largest = new GNGNode*[2];
        
        largest[0]=0;
        largest[1]=0;
        
        int start_index=0;
        
        while(!m_g[start_index]->occupied) ++start_index;
        
        double error=m_g[start_index]->error;
        largest[0] =m_g[start_index];
       
        
        
        
        for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied){
                
                double new_error = m_g[i]->error;
               
                if(error>new_error){
                    error = new_error;
                   
                    largest[1] = largest[0];
                    largest[0] = m_g[i];
                }
            }
        }
        
         
        
       
        
        if(largest[1]==0) {
            start_index = start_index + 1;

            while (!m_g[start_index]->occupied) ++start_index;

            dbg.push_back(2,"gngAlgorithm::commence next search at "+to_string(start_index));

            error = m_g[start_index]->error;
            
            largest[1] = m_g[start_index];
            
          for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied && m_g[i]!=largest[0]){
                double new_error = m_g[i]->error;
                if(error>new_error){
                    error = new_error;
                    largest[1] = m_g[i];
                }
            }
          }           
        }
        
        return largest;
    }
    GNGNode ** TwoNearestNodes(GNGExample * ex){ //to the example
        
        GNGNode ** nearest = new GNGNode*[2];
        
        nearest[0]=0;
        nearest[1]=0;
        
        int start_index=0;
        
        while(!m_g[start_index]->occupied) ++start_index;
        
        double dist=m_g.getDist(ex->position, m_g[start_index]->position);
        nearest[0] = m_g[start_index];
       
        dbg.push_back(1,"GNGAlgorithm::starting search of nearest node from "+to_string(start_index));
        
        
        
        for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied){
                
                double new_dist = m_g.getDist(ex->position, m_g[i]->position);
               
                if(dist>new_dist){
                    dist = new_dist;
                   
                    nearest[1] = nearest[0];
                    nearest[0] = m_g[i];
                }
            }
        }
        
         
        
       
        
        if(nearest[1]==0) {
            start_index = start_index + 1;

            while (!m_g[start_index]->occupied) ++start_index;

            dbg.push_back(2,"gngAlgorithm::commence next search at "+to_string(start_index));

            dist = m_g.getDist(ex->position, m_g[start_index]->position);
            
            nearest[1] = m_g[start_index];
            
          for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied && m_g[i]!=nearest[0]){
                double new_dist = dist=m_g.getDist(ex->position, m_g[i]->position);
                if(dist>new_dist){
                    dist = new_dist;
                    nearest[1] = m_g[i];
                }
            }
          }           
        }
        dbg.push_back(1,"search successful and nearest[1]= "+to_string(nearest[1]));
        return nearest;
    }
    
    void IncreaseError(GNGNode * node, double error){
        node->error+=error;
    }
//R
    void DecreaseAllErrors(){
        
    }
    
    void DecreaseError(GNGNode * node){
        
    }
    
    void SetError(GNGNode * node, double error){
    }
        
public:
    GNGAlgorithm(GNGDatabase* db, int start_number,boost::mutex * mutex, int lambda=1):
            m_g(mutex),g_db(db),c(0),s(0) ,
            m_max_nodes(50),m_max_age(20),
            m_alpha(0.1),m_betha(0.1)
    {
        m_g.init(start_number);
        m_lambda=lambda;
    }
    
    double getError() const{ return m_error; }
    bool stoppingCriterion(){ return m_g.getNumberNodes()>m_max_nodes; }
    
    GNGGraph * get_graph(){ return &m_g; }
    

    
    void TotallyRandomAdd(){

        GNGExample  ex = g_db -> drawExample();
        
       
        
        int i=m_g.newNode(&ex.position[0]);
         ex = g_db -> drawExample();
         m_g[i]->error = -(__int_rnd(0,10000));
        
        
        i=m_g.newNode(&ex.position[0]);
       // m_g.addDEdge(__int_rnd(0,m_g.getNumberNodes()-1),0);
        m_g[i]->error = - (__int_rnd(0,10000));
       
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
         int nodes;
         nodes = m_g.getNumberNodes();
        //dbg.push_back(1,"GNGAlgorithm::number of nodes = "+to_string(nodes));
        m_g.deleteNode(a);
         nodes = m_g.getNumberNodes();
        ///bg.push_back(1,"GNGAlgorithm::number of nodes = "+to_string(nodes));
        dbg.push_back(1,"GNGAlgorithm::removalSuccessful");
           
           
    }
    
    void RandomInit(){
        GNGExample  ex1 = g_db -> drawExample();
        GNGExample  ex2 = g_db -> drawExample();

        m_g.newNode(ex1.position);
        m_g.newNode(ex2.position);

    }
    
    void AddNewNode(){
      /*  GNGNode * error_nodes=LargestErrorNodes();
        
        double  position[GNGExample::N];
        for(int i=0;i<GNGExample::N;++i)
            position[i] = (error_nodes[0].position[i] + error_nodes[1].position[i])/2;
        
        int new_node_index=m_g.newNode(&position[0]);
        
        m_g.removeEdge(error_nodes[0].nr, error_nodes[1].nr);
        
        m_g.addUDEdge(error_nodes[0].nr, new_node_index);
        m_g.addUDEdge(new_node_index, error_nodes[1].nr);
        
        DecreaseError(&error_nodes[0]);
        DecreaseError(&error_nodes[1]);
        SetError(m_g[new_node_index],(error_nodes[0].error + error_nodes[1].error)/2);*/
    }

    

    void Adapt(GNGExample * ex){
       /* GNGNode * nearest = TwoNearestNodes();
        
        double error=0.0;
        for(int i=0;i<GNGExample::N;++i)
            error+=(nearest[0].position[i] - ex->position[i])*(nearest[0].position[i] - ex->position[i]); //armadillo.
        
        
        IncreaseError(&nearest[0],error);
        
        for(int i=0;i<GNGExample::N;++i){
            nearest[0].position[i]+=m_eps_v*(ex->position[i]-nearest[0].position[i]);
        }
        
   
        FOREACH(edg,*(nearest[0].edges))
        {
            
          for(int i=0;i<GNGExample::N;++i){
              nearest[edg->nr].position[i]+=m_eps_n*(ex->position[i]-nearest[edg->nr].position[i]);
          }
          
        }
        
        if(!m_g.isEdge(nearest[0].nr, nearest[1].nr)){
            m_g.addUDEdge(nearest[0].nr, nearest[1].nr);
        }
        
        FOREACH(edg,*(nearest[0].edges))
        {
            
             edg->age++;
             edg->rev->age++;
             
             if(edg->age>m_max_age) m_g.removeEdge(nearest[0].nr, edg->nr);
          
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
                boost::this_thread::sleep(boost::posix_time::microseconds(100000)); //to see the progress when the data is small
                 TotallyRandomAdd(); 
                 RandomDeletion();
                 GNGExample  ex = g_db -> drawExample();
                 int nodes = m_g.getNumberNodes();
                 int index = m_g.getMaximumIndex();
               
                 dbg.push_back(1,"GNGAlgorithm::maximum index = "+to_string(index));
                 if(m_g.getNumberNodes()>1){
                  GNGNode ** largest = LargestErrorNodes();
                    cout<<"Nearest to "<<ex.position[0]<<","<<ex.position[1]<<","<<ex.position[2]<<" are "<<(*(largest[0]))<<" "<<(*(largest[1]))<<endl;
                 }
                  //  dbg.push_back(-3,"GNGAlgorithm::draw example");
              //  Adapt(&ex);
            }
            //AddNewNode();
            ++c; //epoch
        }
    }
    
    
    
    
    virtual ~GNGAlgorithm(){}
private:
    
};

#endif	/* GNGALGORITHM_H */

