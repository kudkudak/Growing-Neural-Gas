
#include "RcppInterface.h"
#include <fstream>


using namespace boost::interprocess;
using namespace std;
using namespace arma;
using namespace Rcpp;

int GNG_DIM=3;
DebugCollector dbg;
int max_nodes=1000;
bool uniformgrid=true,lazyheap=true;
double *orig=0,*axis=0;
int database_type=1;
int max_age=200;
double alpha=0.95;
double betha=0.9995;
double lambda=200;
double eps_v=0.05;
int memory_bound;
double eps_n=0.0006;

//check what is reffered int ptr-> and implement as standalones

#undef DEBUG

double * uploadOBJ(const char * filename, GNGDatabase * gngDatabase){
	

    GNGExample ex;
    ifstream ifs;
    ifs.open(filename);
    double vertex[3];
    double *bbox = new double[6];
    REP(i,6) bbox[i]=-1;
    std::string v;
    int iteration=0;
    while(!ifs.eof()){
        ifs>>v;
        if(v!="v") continue;
        ++iteration;
        REP(i,3){
            ifs>>vertex[i];
            if(iteration==1){
                bbox[2*i]=vertex[i];
                bbox[2*i+1]=vertex[i];
            }
            else{
                bbox[2*i]=std::min(bbox[2*i],vertex[i]);
                bbox[2*i+1]=std::max(bbox[2*i+1],vertex[i]);
            }
           // write_array(vertex,vertex+3);
           // cout<<bbox[1]<<endl;
            
        }
        
        memcpy(ex.position,vertex,3*sizeof(double));
        gngDatabase->addExample(&ex);
    }
    write_array(bbox,bbox+6);
    return bbox;
}
RcppExport SEXP GNGGetServerID(){
	managed_shared_memory interserver_variables(open_or_create,"GNGInterServerSegment", 65536);
	int *ptr = interserver_variables.find_or_construct<int>("GNG__serverCount") ();
	return wrap(*ptr);
}
RcppExport SEXP GNGRunServer() {
        managed_shared_memory interserver_variables(open_or_create,"GNGInterServerSegment", 65536);
	int *ptr = interserver_variables.find_or_construct<int>("GNG__serverCount") ();
	SHMemoryManager::COUNTER = *ptr;


    GNGAlgorithmControl * gngAlgorithmControl;
    GNGAlgorithm * gngAlgorithm;
    GNGGraph * gngGraph;
    GNGDatabase* gngDatabase;
    SHMemoryManager *shm;

    shm = new SHMemoryManager(memory_bound); //nodes <-estimate!
    shm->new_segment(memory_bound); //database <-estimate!
	
    *ptr = SHMemoryManager::COUNTER;

    cout<<"New GNG__serverCount = "<<*ptr<<endl;

    GNGNode::mm = shm;
    GNGNode::alloc_inst = new ShmemAllocatorGNG(shm->get_segment(0)->get_segment_manager());

    if(!orig){
        orig=new double[3];
        orig[0]=orig[1]=orig[2]=-2;
    }
    if(!axis){
        axis=new double[3];
        axis[0]=axis[1]=axis[2]=4;
    }
   
    SHGNGExampleDatabaseAllocator alc(shm->get_segment(1)->get_segment_manager());

    SHGNGExampleDatabase * database_vec = shm->get_segment(1)->construct< SHGNGExampleDatabase > ("database_vec")(alc);

    gngAlgorithmControl = shm->get_segment(1)->construct<GNGAlgorithmControl > ("gngAlgorithmControl")();



    if(database_type==1)
    	gngDatabase = new GNGDatabaseSimple(&gngAlgorithmControl->database_mutex, database_vec);
    else if(database_type==2)
    	gngDatabase = new GNGDatabaseProbabilistic(&gngAlgorithmControl->database_mutex, database_vec);


    gngGraph = shm->get_segment(0)->construct<GNGGraph > ("gngGraph")(&gngAlgorithmControl->grow_mutex, max_nodes);
    gngAlgorithm = new GNGAlgorithm
            (*gngGraph, gngDatabase, gngAlgorithmControl,
            max_nodes, orig, axis, axis[0] / 4.0, max_nodes,
    		max_age, alpha, betha, lambda, //params for the algorithm
    		eps_v, eps_n
            );
    gngAlgorithm->setToggleUniformGrid(uniformgrid);
    gngAlgorithm->setToggleLazyHeap(lazyheap);
    gngAlgorithmControl->setRunningStatus(false); //skrypt w R inicjalizuje  

    gngAlgorithm->runAlgorithm();

    return wrap(0);
}


//TODO: lepsze settery i gettery (dataframe? nie wiem)
RcppExport SEXP GNGSet__memory_bound(SEXP val){
	memory_bound = as<int>(val);
	return wrap(0);
}
RcppExport SEXP GNGSet__max_age(SEXP val){
	max_age = as<int>(val);
	return wrap(0);
}
RcppExport SEXP GNGSet__alpha(SEXP val){
	alpha = as<double>(val);
	return wrap(0);
}
RcppExport SEXP GNGSet__betha(SEXP val){
	betha = as<double>(val);
	return wrap(0);
}
RcppExport SEXP GNGSet__lambda(SEXP val){
	lambda = as<double>(val);
	return wrap(0);
}
RcppExport SEXP GNGSET__eps_v(SEXP val){
	eps_v = as<double>(val);
	return wrap(0);
}
RcppExport SEXP GNGSet__eps_n(SEXP val){
	eps_n = as<double>(val);
	return wrap(0);
}

//settery parametrów przed odpaleniem servera
RcppExport SEXP GNGSet__boundingbox(SEXP _orig, SEXP _axis){
    NumericVector orig_rcpp(_orig) ,axis_rcpp(_axis);
    
    orig=new double[(int)orig_rcpp.size()];
    axis=new double[(int)axis_rcpp.size()];
    
    REP(i,(int)orig_rcpp.size()){
        orig[i]=orig_rcpp[i];
        axis[i]=axis_rcpp[i];
    }
    
    return wrap(0);
}
RcppExport SEXP GNGSet__debug_level(SEXP _xp) {
    dbg.set_debug_level(as<int>(_xp));
    return wrap(0);
}
RcppExport SEXP GNGSet__database_type(SEXP _xp) {
    database_type=as<int>(_xp);
    return wrap(0);
}
RcppExport SEXP GNGSet__dim(SEXP _xp) {
    GNG_DIM = as<int>(_xp);
    return wrap(0);
}
RcppExport SEXP GNGSet__max_nodes(SEXP _xp) {
    max_nodes = as<int>(_xp);
    return wrap(0);
}
 RcppExport SEXP GNGSet__uniformgrid(SEXP _xp) {
    uniformgrid = as<bool>(_xp);
    return wrap(0);
}
 RcppExport SEXP GNGSet__lazyheap(SEXP _xp) {
    lazyheap = as<bool>(_xp);
    return wrap(0);
}
 


RcppExport SEXP GNGClient__loadObj(SEXP _xp,SEXP _name) {
    Rcpp::XPtr<GNGClient> ptr(_xp);

    std::string filename = as<std::string>(_name);
    
    double * bbox = uploadOBJ(filename.c_str(), ptr->g_database);
    double orig[3],axis[3];
    orig[0] = bbox[0] - 0.1;
    orig[1] = bbox[2] - 0.1;
    orig[2] = bbox[4] - 0.1;

    axis[0] = bbox[1] - orig[0] + 0.1;
    axis[1] = bbox[3] - orig[1] + 0.1;
    axis[2] = bbox[5] - orig[2] + 0.1;   
    
    cout<<"make sure bb is set as:\n";
    write_array(orig,orig+2);
    write_array(axis,axis+2);
    
    return wrap(0);
}

RcppExport SEXP GNGClient__new(SEXP val_id){
   int id = as<int>(val_id);

   cout<<"Opening "+id<<"\n";

   int id_tmp1 = id+1;
   int id_tmp2 = id+2;

   managed_shared_memory *  mshm1= new managed_shared_memory(open_only,("SHMemoryPool_Segment"+to_string<int>(id_tmp1)).c_str());
   managed_shared_memory *  mshm2= new managed_shared_memory(open_only,("SHMemoryPool_Segment"+to_string<int>(id_tmp2)).c_str());

   GNGAlgorithmControl * gngAlgorithmControl = (mshm2->find< GNGAlgorithmControl > ("gngAlgorithmControl")).first;
   GNGGraph * gngGraph = (mshm1->find< GNGGraph >("gngGraph")).first;		
   SHGNGExampleDatabase * database_vec = (mshm2->find< SHGNGExampleDatabase >("database_vec")).first;	

   ScopedLock sc(gngAlgorithmControl->grow_mutex); 

   GNGClient * gngClient = new GNGClient();

   gngClient->graph = gngGraph;
   gngClient->g_database = new GNGDatabaseSimple(&gngAlgorithmControl->database_mutex, database_vec);
   gngClient->control = gngAlgorithmControl;
  
   Rcpp::XPtr<GNGClient> ptr(gngClient,true);



   return wrap(ptr); 
}

//czyta 3 node


RcppExport SEXP GNGClient__getErrorNode(SEXP _xp, SEXP _i) {
    Rcpp::XPtr<GNGClient> ptr(_xp);
    //alg nie w ggi ale dobra
    return wrap(ptr->graph->getErrorNodeShare(as<int>(_i)));
}

RcppExport SEXP GNGClient__getAccumulatedError(SEXP _xp) {
    Rcpp::XPtr<GNGClient> ptr(_xp);

    
    //alg nie w ggi ale dobra
    return wrap(ptr->graph->getAccumulatedErrorShare());
}

RcppExport SEXP GNGClient__updateBuffer(SEXP _xp){
    
    
    Rcpp::XPtr<GNGClient> ptr(_xp);

    ScopedLock sc(ptr->control->grow_mutex);
    
    //GraphAccess & graph = *(ptr->readGraph());
    GNGNode * g_pool = ptr->graph->getPoolShare();
    //copy routine !
    
    int maximumIndex = ptr->graph->getMaximumIndex();
    
    cout<<"Size of buffer = "<<maximumIndex<<endl;
    
    ptr->buffer.clear();
    
    cout<<SIZE(ptr->buffer)<<endl;
    
    ptr->buffer.resize(maximumIndex+1);
    
    
    
    
    
    for(int i=0;i<=maximumIndex;++i){
        //hack!
        if(!g_pool[i].occupied) { 
            ptr->buffer[i].occupied=false; 
            for(int i=0;i<GNG_DIM;++i) ptr->buffer[i].position[i]=0; continue;  
        } //no more data is needed here (for in getNodeMatrix it will simply check for correctness of buffer line
       
        
        ptr->buffer[i] = g_pool[i]; //operator= !! wazne zeby dawac tego typu rzeczy do klas
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
    ScopedLock sc(ptr->control->grow_mutex);
    
    return wrap((int)(ptr->graph->getNumberNodes()));
}

/*!
 *
 */
RcppExport SEXP GNGClient__getBufferSize(SEXP _xp){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    return wrap((int)(ptr->buffer.size()));    
}

RcppExport SEXP GNGClient__getNumberNodes(SEXP _xp){
    
    Rcpp::XPtr<GNGClient> ptr(_xp);
    
    return wrap((int)(ptr->graph->getNumberNodes()));
}

RcppExport SEXP GNGClient__getNodeNumberEdges(SEXP _xp, SEXP _nr){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    return wrap((int)
            (ptr->buffer[as<int>(_nr)].edges.size())
    );
}


RcppExport SEXP GNGClient__runServer(SEXP _xp){
	Rcpp::XPtr<GNGClient> ptr(_xp);
	ptr->control->setRunningStatus(true);
	return wrap(0);
}

RcppExport SEXP GNGClient__terminateServer(SEXP _xp){
	Rcpp::XPtr<GNGClient> ptr(_xp);

	ptr->control->setRunningStatus(false);
	ptr->control->terminate();

	return wrap(0);
}

RcppExport SEXP GNGClient__pauseServer(SEXP _xp){
	Rcpp::XPtr<GNGClient> ptr(_xp);
	
	ptr->control->setRunningStatus(false);
	return wrap(0);
}

RcppExport SEXP GNGClient__getNode(SEXP _xp, SEXP _nr){

    Rcpp::XPtr<GNGClient> ptr(_xp);
    int nr = as<int>(_nr);

    int a = 1;
 
   
    GNGNodeOffline * requested_node = &ptr->buffer[nr];
 
    int edg = requested_node->edgesCount;
    
    //cout<<edg<<endl;
    
    Rcpp::NumericVector node(edg+GNG_DIM+1);
    
    
    //memcpy?, arma position ?
    
    node[0] = requested_node->occupied;
    
    //slight change of the model
    
   // cout<<"node read\n";
    
    if(! requested_node->occupied) return wrap(node);
    
    
    int i;
    for(i=1;i<=GNG_DIM;++i) {
        node[i]=requested_node->position[i-1];
    }

    i = GNG_DIM;
    
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

RcppExport SEXP GNGClient__getNodeWithParam(SEXP _xp, SEXP _nr){

    Rcpp::XPtr<GNGClient> ptr(_xp);
    int nr = as<int>(_nr);

    int a = 1;


    GNGNodeOffline * requested_node = &ptr->buffer[nr];

    int edg = requested_node->edgesCount;

    //cout<<edg<<endl;

    Rcpp::NumericVector node(edg+GNG_DIM+1+1);


    //memcpy?, arma position ?

    node[0] = requested_node->occupied;

    //slight change of the model

   // cout<<"node read\n";

    if(! requested_node->occupied) return wrap(node);


    int i;
    for(i=1;i<=GNG_DIM+1;++i) {
        node[i]=requested_node->position[i-1];
    }

    i = GNG_DIM+1;

   // cout<<"pos read\n";

    if(edg>0){
        FOREACH(it,requested_node->edges){

           ++i;
           node[i] = it->nr;
          if(i==edg+4) break; //added  new edge meanwhile
        }
    }


    return wrap(node);
}


RcppExport SEXP GNGClient__addExamples(SEXP _xp, SEXP _examples){
    Rcpp::XPtr<GNGClient> ptr(_xp);
    cout<<"wszedlem do metody";

    Rcpp::NumericMatrix examples(_examples);
    cout<<examples.nrow()<<","<<examples.ncol()<<endl;

    ScopedLock sc(ptr->control->grow_mutex);
    if(database_type==1){
    double * pos = new double[GNG_DIM];
      for(int i=0;i<examples.nrow();++i){
        for(int j=0;j<GNG_DIM;++j) pos[j] = (double)(examples(i,j));
        GNGExample ex(&pos[0]);
        ptr->g_database->addExample(&ex);
      }
    }
    else if(database_type==2){
        double * pos = new double[GNG_DIM+1];
        for(int i=0;i<examples.nrow();++i){
            for(int j=0;j<=GNG_DIM;++j) pos[j] = (double)(examples(i,j));
            GNGExample ex(&pos[0]);
            ptr->g_database->addExample(&ex);
        }
    }
    
#ifdef DEBUG
    int size = ptr->g_database->getSize();
    dbg.push_back(2,"RcppInterface::addExample::size of db = "+to_string(size));
#endif

    
    return wrap(1);
}

