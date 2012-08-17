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
        
    double m_accumulated_error;
    
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
               
                if(error<new_error){
                    error = new_error;

                    largest[0] = m_g[i];
                }
            }
        }
        
         //dbg.push_back(2,"gngAlgorithm::LargestError::found  "+to_string(*largest[0]));



            if(largest[0]->edgesCount==0) //{largest[0]->error=0; return largest;} //error?
            {
                //removing the node
                
                
                m_g.deleteNode(largest[0]->nr);
                //largest[0]->error=0;
                return largest;
                
                
                largest[1]= (TwoNearestNodes(largest[0]->position))[0];
                return largest;
                
                        
                int c = __int_rnd(0,m_g.getNumberNodes());
                while(c==largest[0]->nr || !m_g[c]->occupied) c = __int_rnd(0,m_g.getNumberNodes());
                largest[1] = m_g[c];
                return largest;
            }
         
         
         
            int j=0;
            FOREACH(edg,*(largest[0]->edges)){
                ++j;
                
                if(j==1){
                    largest[1] = m_g[edg->nr];
                    error = largest[1]->error;
                    continue;
                }
                
                
                double new_error = m_g[edg->nr]->error;
                if(error<new_error){
                    error = new_error;
                    largest[1] = m_g[edg->nr];
                }
            }
                   
          //
        return largest;
    }
    GNGNode ** TwoNearestNodes(double * position){ //to the example
        
        GNGNode ** nearest = new GNGNode*[2];
        
        nearest[0]=0;
        nearest[1]=0;
        
        int start_index=0;
        
        while(!m_g[start_index]->occupied) ++start_index;
        
        double dist=m_g.getDist(position, m_g[start_index]->position);
        nearest[0] = m_g[start_index];
       
        //dbg.push_back(1,"GNGAlgorithm::starting search of nearest node from "+to_string(start_index));
        
        
        
        for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied){
                
                double new_dist = m_g.getDist(position, m_g[i]->position);
               
                if(dist>new_dist){
                    dist = new_dist;
                 
                    nearest[0] = m_g[i];
                }
            }
        }
        
        start_index=0;
        
        while(!m_g[start_index]->occupied || start_index==nearest[0]->nr) ++start_index;   
        dist=m_g.getDist(position, m_g[start_index]->position);
        nearest[1] = m_g[start_index];
        
       for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied && i!=nearest[0]->nr){
                
                double new_dist = m_g.getDist(position, m_g[i]->position);
               
                if(dist>new_dist){
                    dist = new_dist;
                 
                    nearest[1] = m_g[i];
                }
            }
        }
             
        
       
        /*
        if(nearest[1]==0) {
            start_index = start_index + 1;

            while (!m_g[start_index]->occupied) ++start_index;

            //dbg.push_back(2,"gngAlgorithm::commence next search at "+to_string(start_index));

            dist = m_g.getDist(position, m_g[start_index]->position);
            
            nearest[1] = m_g[start_index];
            
          for(int i=start_index+1;i<=m_g.getMaximumIndex();++i){ //another idea for storing list of actual nodes?
            
            if(m_g[i]->occupied && m_g[i]!=nearest[0]){
                double new_dist = dist=m_g.getDist(position, m_g[i]->position);
                if(dist>new_dist){
                    dist = new_dist;
                    nearest[1] = m_g[i];
                }
            }
          }           
        }*/
        //dbg.push_back(1,"search successful and nearest[1]= "+to_string(nearest[1]));
        return nearest;
    }
    
    void IncreaseError(GNGNode * node, double error){
        node->error+=error;
    }
//R
    void DecreaseAllErrors(){
        int maximumIndex = m_g.getMaximumIndex();
        
        REP(i,maximumIndex){
           
            if(m_g[i]->occupied){
                m_g[i]->error = m_betha*m_g[i]->error;
            }
        }
    }
    
    void DecreaseError(GNGNode * node){
        node->error = m_alpha*node->error;
    }
    
    void SetError(GNGNode * node, double error){
        node->error = error;
    }
        
public:
    GNGAlgorithm(GNGDatabase* db, int start_number,boost::mutex * mutex, int lambda=1):
            m_g(mutex),g_db(db),c(0),s(0) ,
            m_max_nodes(1000),m_max_age(50),
            m_alpha(0.9),m_betha(0.9995),m_lambda(100),
            m_eps_v(0.09),m_eps_n(0.001)
    {
        m_g.init(start_number);
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

        
        //rev edges
        
      
        
    }
    
    void RandomDeletion(){
        int a = __int_rnd(0,m_g.getNumberNodes()-1);
        int b;
        //dbg.push_back(1,"GNGAlgorithm:: erase node with outgoing edges (revs also!)"+to_string(a));
        
         FOREACH(edg,*(m_g[a]->edges)){
             m_g.removeRevEdge(a,edg);
         }
         int nodes;
         nodes = m_g.getNumberNodes();
        ////dbg.push_back(1,"GNGAlgorithm::number of nodes = "+to_string(nodes));
        m_g.deleteNode(a);
         nodes = m_g.getNumberNodes();
        ///bg.push_back(1,"GNGAlgorithm::number of nodes = "+to_string(nodes));
        //dbg.push_back(1,"GNGAlgorithm::removalSuccessful");
           
           
    }
    
    void RandomInit(){
        GNGExample  ex1 = g_db -> drawExample();
        GNGExample  ex2 = g_db -> drawExample();
        
                while(ex2==ex1) ex2 = g_db -> drawExample();

        m_g.newNode(ex1.position);
        m_g.newNode(ex2.position);

    }
    
    void AddNewNode(){
       GNGNode ** error_nodes=LargestErrorNodes();
 
       
        double  position[GNGExample::N];
        for(int i=0;i<GNGExample::N;++i)
            position[i] = (error_nodes[0]->position[i] + error_nodes[1]->position[i])/2;
        
        int new_node_index=m_g.newNode(&position[0]);
        
        //dbg.push_back(4,"GNGAlgorith::AddNewNode::added "+to_string(*m_g[new_node_index]));
        
        m_g.removeEdge(error_nodes[0]->nr, error_nodes[1]->nr);
        
         //dbg.push_back(3,"GNGAlgorith::AddNewNode::removed edge beetwen "+to_string(error_nodes[0]->nr)+" and"+to_string( error_nodes[1]->nr));
         //dbg.push_back(2,"GNGAlgorithm::AddNewNode::largest error node after removing edge : "+to_string(*error_nodes[0]));
        
        m_g.addUDEdge(error_nodes[0]->nr, new_node_index);
        m_g.addUDEdge(new_node_index, error_nodes[1]->nr);
        
        //dbg.push_back(3,"GNGAlgorith::AddNewNode::add edge beetwen "+to_string(error_nodes[0]->nr)+" and"+to_string(new_node_index));
        //dbg.push_back(3,"GNGAlgorith::AddNewNode::add edge beetwen "+to_string(error_nodes[1]->nr)+" and"+to_string(new_node_index));
        DecreaseError(error_nodes[0]);
        DecreaseError(error_nodes[1]);
        
        SetError(m_g[new_node_index],(error_nodes[0]->error + error_nodes[1]->error)/2);
    }

    

    void Adapt(GNGExample * ex){
        
        //MOST OF THE WORK!
        
        
        
        GNGNode ** nearest = TwoNearestNodes(ex->position);
       
        REP(i,m_g.getMaximumIndex()){
            if(m_g.getDist(m_g[i]->position,ex->position)<m_g.getDist(nearest[0]->position,ex->position)) cout<<"XXXXXXXXXXXXXXXXXXXX\n";
        }

        REP(i,m_g.getMaximumIndex()){
            if(m_g[i]->occupied && m_g.getDist(m_g[i]->position,ex->position)<m_g.getDist(nearest[1]->position,ex->position) && i!=nearest[0]->nr){
                cout<<"XXXXXXXXXXXXXXXXXXXX\n";
                cout<<nearest[1]->nr<<" "<<nearest[0]->nr<<" "<<i<<endl;
                cout<<m_g.getDist(nearest[1]->position,ex->position)<<">"<<m_g.getDist(m_g[i]->position,ex->position)<<endl;
            }
        }
        
        /*GNGNode ** nearest=new GNGNode*[2];
        
         int d = __int_rnd(0,m_g.getNumberNodes());
                while( !m_g[d]->occupied) d = __int_rnd(0,m_g.getNumberNodes());
                nearest[0] = m_g[d];
  
         int c = __int_rnd(0,m_g.getNumberNodes());
                while(c==nearest[0]->nr || !m_g[c]->occupied) c = __int_rnd(0,m_g.getNumberNodes());
                nearest[1] = m_g[c];
                
        */
        
        //dbg.push_back(4,"GNGAlgorith::Adapt::found nearest nodes to the drawn example "+to_string(*nearest[0])+" "+to_string(*nearest[1]));
        
        double error=m_g.getDist(nearest[0]->position,ex->position);
      
        
        
        IncreaseError(nearest[0],error);
        
        //dbg.push_back(4,"GNGAlgorith::Adapt::accounted for the error");
        
        for(int i=0;i<GNGExample::N;++i){
            nearest[0]->position[i]+=m_eps_v*(ex->position[i]-nearest[0]->position[i]);
        }
        
        
        if(nearest[0]->edgesCount)
        FOREACH(edg,*(nearest[0]->edges))
        {
            
          for(int i=0;i<GNGExample::N;++i){
              m_g[edg->nr]->position[i]+=m_eps_n*(ex->position[i]-m_g[edg->nr]->position[i]);
          }
          
        }
        
        //dbg.push_back(4,"GNGAlgorith::Adapt::position of the winner and neighbour mutated");
        
        if(!m_g.isEdge(nearest[0]->nr, nearest[1]->nr)){
            m_g.addUDEdge(nearest[0]->nr, nearest[1]->nr);
            //dbg.push_back(4,"GNGAlgorith::Adapt::added edge beetwen "+to_string(nearest[0]->nr)+ " and " +to_string(nearest[1]->nr));
        }
        //cout<<(*nearest[0])<<endl;
        bool BYPASS=false;
        FOREACH(edg,*(nearest[0]->edges))
        {
          //  cout<<edg->nr<<",";
            if(BYPASS){--edg; BYPASS=false;}
            
             edg->age++;
             edg->rev->age++;
             
             if(edg->nr == nearest[1]->nr) {edg->age=0; edg->rev->age=0; }
             
             if(edg->age>m_max_age) {
                // cout<<"DELETION";
                 //dbg.push_back(3,"GNGAlgorith::Adapt::Removing aged edge "+to_string(nearest[0]->nr)+" - "+to_string(edg->nr));
                 int nr = edg->nr;
                 edg=m_g.removeEdge(nearest[0]->nr, edg);
                 if(edg == nearest[0]->edges->end()) break;
                 BYPASS=true;
                 if(m_g[nr]->edgesCount==0)  m_g.deleteNode(nr);
                 if(m_g[nearest[0]->nr]->edgesCount==0)  m_g.deleteNode(nearest[0]->nr);
                 //dbg.push_back(3,"GNGAlgorith::Adapt::Removal completed");
                 //dbg.push_back(2,to_string(*nearest[0]));
        
             }
        }
       //  cout<<(*nearest[0])<<endl;
        
        DecreaseAllErrors();
        
        
               
        
    }  
        
    void test_routine(){
            TotallyRandomAdd(); 
                 RandomDeletion();
                 GNGExample  ex = g_db -> drawExample();
                 int nodes = m_g.getNumberNodes();
                //int index = m_g.getMaximumIndex();
               
               
                 if(m_g.getNumberNodes()>1){
                  //GNGNode ** largest = LargestErrorNodes();
                  //f(largest[1]==0) return;
                    //cout<<"Nearest to "<<ex.position[0]<<","<<ex.position[1]<<","<<ex.position[2]<<" are "<<(*(largest[0]))<<" "<<(*(largest[1]))<<endl;
                 }
    }
    
    double getAccumulatedError() const{
        return m_accumulated_error;
    }
    void CalculateAccumulatedError(){
        int maximumIndex = m_g.getMaximumIndex();
        m_accumulated_error=0.0;
        REP(i,maximumIndex){
           
            if(m_g[i]->occupied){
               m_accumulated_error+=m_g[i]->error;
            }
        }
    }
    void TestAgeCorrectness(){
         int maximumIndex = m_g.getMaximumIndex();
     
        REP(i,maximumIndex){
           
            if(m_g[i]->occupied && m_g[i]->edgesCount){
                FOREACH(edg,*(m_g[i]->edges)){
                    if(edg->age > m_max_age){
                        cout<<"XXXXXXXXXXXXXXXXX\n";
                        cout<<*(m_g[i])<<endl;
                    }
                    //cout<<m_g.getDistEdge(i,edg)<<endl;
                    if(m_g.getDistEdge(i,edg)>0.2){
                        cout<<(*m_g[i])<<" TO "<<edg->nr<<endl;
                    }
                }
            }
        }   
    }
    
    void runAlgorithm(){ //1 thread needed to do it (the one that computes)
        c=0;
        boost::posix_time::microseconds work(1000),waitsleep(40);
        bool waited=false;

        
        //random init (2 nodes)
        RandomInit();
        
        
        //powinien sprawdzac czy juz ma zaczac.
        while(1){
            
            for (s = 0; s<m_lambda ;++s){ //global counter!!
               // boost::this_thread::sleep(boost::posix_time::microseconds(100000)); //to see the progress when the data is small
                //dbg.push_back(-3,"GNGAlgorithm::draw example");
                GNGExample ex = g_db->drawExample();
                Adapt(&ex);
            }
            if(m_max_nodes>m_g.getNumberNodes())AddNewNode();
            ++c; //epoch
            CalculateAccumulatedError();
            if(m_g.getNumberNodes()>800) TestAgeCorrectness();
           // test_routine();
        }
    }
    
    
    
    
    virtual ~GNGAlgorithm(){}
private:
    
};

#endif	/* GNGALGORITHM_H */

