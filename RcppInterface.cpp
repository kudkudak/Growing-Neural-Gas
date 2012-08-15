#include "RcppInterface.h"

#include "Utils.cpp"

using namespace boost::interprocess;
using namespace std;
using namespace arma;
using namespace Rcpp;


DebugCollector dbg;
GNGGraphInfo * ggi;
int GNGExample::N=3; 
RcppExport SEXP GNGClient__new(){
   //GNGExample::N=3; g
   
    
    
   managed_shared_memory *  mshm1= new managed_shared_memory(open_only,"SHMemoryPool_Segment1");
   managed_shared_memory *  mshm2= new managed_shared_memory(open_only,"SHMemoryPool_Segment2");
   
   MyMutex * grow_mutex = (mshm1->find< MyMutex >("grow_mutex")).first;		
   MyMutex * database_mutex = (mshm2->find< MyMutex >("database_mutex")).first;	
   SHGNGExampleDatabase * database_vec = (mshm2->find< SHGNGExampleDatabase >("database_vec")).first;	
   
   ScopedLock sc(*grow_mutex); 
   
  // grow_mutex->lock();

   GNGGraphInfo * ggi2 = (mshm1->find<GNGGraphInfo >("SHGraphInfo")).first; //musi byc stworzony : kolejne miejsce na blad !
   
   
   GNGClient * gngClient = new GNGClient();

   
   gngClient->ggi = ggi2;
   gngClient->g_database = new GNGDatabaseSimple(database_mutex, database_vec);
   

   gngClient->grow_mutex = grow_mutex;

   Rcpp::XPtr<GNGClient> ptr(gngClient,true);


   
   //grow_mutex->unlock();

   return wrap(ptr); 
}

//czyta 3 node

RcppExport SEXP GNGClient__updateBuffer(SEXP _xp){
    
    
    Rcpp::XPtr<GNGClient> ptr(_xp);

    ScopedLock sc(*ptr->grow_mutex);
    
    GraphAccess & graph = *(ptr->readGraph());
    
    //copy routine !
    
    int maximumIndex = graph.getMaximumIndex();
    
    //cout<<maximumIndex<<endl;
    
    //ptr->buffer.clear();
    ptr->buffer.resize(maximumIndex+1);
    
    
    
    
    
    for(int i=0;i<=maximumIndex;++i){
        //hack!
        if(!graph[i]->occupied) { ptr->buffer[i].occupied=false; for(int i=0;i<GNGExample::N;++i) ptr->buffer[i].position[i]=0; continue;  } //no more data is needed here (for in getNodeMatrix it will simply check for correctness of buffer line
       
        
        ptr->buffer[i] = (graph.getPool())[i]; //operator= !! wazne zeby dawac tego typu rzeczy do klas
        int k=0;
        
        
        
        //verification
        FOREACH(edg,ptr->buffer[i].edges){
            if(edg->nr>((int)ptr->buffer.size()-1)) cout<<k+1<<" "<<ptr->buffer[i].edgesCount<<"X\n";
            ++k;
        }
        
        
    }

    
    return wrap(0);    
}

RcppExport SEXP GNGClient__getNumberNodesOnline(SEXP _xp){
    
    
    
    Rcpp::XPtr<GNGClient> ptr(_xp);
    ScopedLock sc(*ptr->grow_mutex);
    
    GraphAccess & graph = *(ptr->readGraph());
    return wrap((int)(graph.getNumberNodes()));
}

RcppExport SEXP GNGClient__getBufferSize(SEXP _xp){

    Rcpp::XPtr<GNGClient> ptr(_xp);
    
    return wrap((int)(ptr->buffer.size()));
    
}

RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp){
    
    Rcpp::XPtr<GNGClient> ptr(_xp);
    
    return wrap((int)(ptr->ggi->nodes));
}

RcppExport SEXP GNGClient__getNodeNumberEdges(SEXP _xp, SEXP _nr){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    return wrap((int)
            (ptr->buffer[as<int>(_nr)].edges.size())
    );
}

/*
RcppExport SEXP GNGClient__getNodeMatrix(SEXP _xp){
    Rcpp::XPtr<GNGClient> ptr(_xp); 
    
    int pool_nodes = (int)ptr->buffer.size();

    
    cout<<"nodeMatrix size = "<<pool_nodes<<endl;
    
    Rcpp::NumericMatrix node_matrix((int)ptr->buffer.size(),GNGExample::N);
    
    
    
    int k=0;
    for(int i=0;i<(int)ptr->buffer.size();++i){
        if(!(ptr->buffer[i].occupied)) {
            for (int j = 0; j < GNGExample::N; ++j) {
                node_matrix(k++, j) = 0;//ptr->buffer[i].position[j];
            }
        } else
        for(int j=0;j<GNGExample::N;++j){
            node_matrix(k++,j) = ptr->buffer[i].position[j];
        }
    }
    
    return wrap(node_matrix);
}
*/

RcppExport SEXP GNGClient__getNode(SEXP _xp, SEXP _nr){

    Rcpp::XPtr<GNGClient> ptr(_xp);
    int nr = as<int>(_nr);

    int a = 1;
 
   
    GNGNodeOffline * requested_node = &ptr->buffer[nr];
 
    int edg = requested_node->edgesCount;
    
    //cout<<edg<<endl;
    
    Rcpp::NumericVector node(edg+3+1);
    
    
    //memcpy?, arma position ?
    
    node[0] = requested_node->occupied;
    
    //slight change of the model
    
   // cout<<"node read\n";
    
    if(! requested_node->occupied) return wrap(node);
    
    
    int i;
    for(i=1;i<=3;++i) {
        node[i]=requested_node->position[i-1];
    }

    i = 3;
    
   // cout<<"pos read\n";
    
    if(edg>0){
        FOREACH(it,requested_node->edges){
            
           ++i;
           node[i] = it->nr;
          if(i==edg+3) break; //added  new edge meanwhile
        }  
    }    

    
    return wrap(node);
}



RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    Rcpp::NumericMatrix examples(_examples);
    cout<<examples.nrow()<<","<<examples.ncol()<<endl;

    ptr->grow_mutex->lock();
    double * pos = new double[3];
    for(int i=0;i<examples.nrow();++i){
        for(int j=0;j<3;++j) pos[j] = (double)(examples(i,j));
        GNGExample ex(&pos[0]);
        ptr->g_database->addExample(&ex);
    }

    
    ptr->grow_mutex->unlock();   
    
    return wrap(1);
}
