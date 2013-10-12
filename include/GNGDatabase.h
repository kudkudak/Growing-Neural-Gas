/* 
 * File:   GNGExampleManager.h
 * Author: staszek
 *
 * Created on 11 sierpień 2012, 10:47
 */
#ifndef GNGDATABASE_H
#define	GNGDATABASE_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "SHMemoryManager.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGGlobals.h"

extern DebugCollector dbg;

/*
 * @brief Example used by database. 
 * 
 * It is a vector of numbers. Any extension of GNGExample for simplicity has to add new information as extra dimensions, and after that
 * Database should interpret it and pass interpreted version to the algorithm module,.
 */
struct GNGExample{
    int dim;
public:
    vector<double> position;
    
    int getLength() const{
        return dim;
    }
    
    /** Hack, TODO: replace by [] operator */
    const double  * getPositionPtr() const{
        return &this->position[0];
    }
    
    void operator=(const GNGExample& ex){
        memcpy(&position[0],ex.getPositionPtr(),sizeof(double)*(this->getLength()));
        this->dim = ex.getLength();
    }
    
    bool operator==(const GNGExample & ex) const{
        if(this->getLength()!=ex.getLength()) return false;
        REP(i,this->getLength()){
            if(getPositionPtr()[i]-ex.getPositionPtr()[i]<-EPS || 
                    getPositionPtr()[i]-ex.getPositionPtr()[i]>EPS ) 
                return false;
        }
        return true;
    }
    friend ostream & operator<<(ostream & out, const GNGExample & x) {
        REP(i, x.getLength())
            out<<x.getPositionPtr()[i]<<",";
        out<<std::endl;
        return out;
    }

    GNGExample(const double * vect, int _dim):position(_dim), dim(_dim){ 
        memcpy(&position[0],vect,sizeof(double)*(this->getLength())); //TODO: +1 failure
    }
    GNGExample(int _dim):position(_dim), dim(_dim){
    }
};


/** Database for growing neural gas interface
 * 
 * @note For performance issue there is a very strong assumption : once written data point
 * doesn't change its pointer position. If it does, it should be very rare, and whole 
 * algorithm should be stopped
 */
class GNGDatabase
{
public:
    int dim;
    GNGDatabase(int dim): dim(dim){}
    
    virtual GNGExample drawExample() const=0 ;
    virtual void addExample(const GNGExample  * ex )=0;
    virtual void removeExample(const GNGExample  * ex)=0;
    virtual int getSize() const=0;
    
    virtual ~GNGDatabase(){}
private:
    GNGDatabase(const GNGDatabase& orig){}
};













class GNGDatabaseMeshes: public GNGDatabase{
public:
    std::vector<GNGDatabase*> meshes;    
    
    GNGDatabaseMeshes(): GNGDatabase(3){
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){     
        throw 1; //not implemented
    }
    
    void addMesh(GNGDatabase * mesh){
        meshes.push_back(mesh);
    }
    
    GNGExample drawExample() const{
        int i = __int_rnd(0,SIZE(meshes)-1);
        return meshes[i]->drawExample();
    }
    
    void addExample(const GNGExample  * ex){
        throw 1;
    }
    int getSize() const{ return 100000000; }
    ~GNGDatabaseMeshes(){
        
    }    
};

typedef boost::interprocess::allocator<GNGExample, boost::interprocess::managed_shared_memory::segment_manager>  SHGNGExampleDatabaseAllocator;
typedef boost::interprocess::vector<GNGExample, SHGNGExampleDatabaseAllocator> 
SHGNGExampleDatabase;


/** Example database for GNG */
class GNGDatabaseLine: public GNGDatabase{
public:
    double m_center[GNG_MAX_DIM];
    double m_r;
     int getSize() const{ return 100000000; }
    
    GNGDatabaseLine(double *center, double r): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*GNG_DIM);
        m_r=r;
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){  
        throw 1; //not implemented
    }
    
   GNGExample drawExample()  const{
        GNGExample ret(3);

        
        ret.position[0] =  m_center[0];
        ret.position[1] =m_center[1];
        ret.position[2] =((double)rand() / RAND_MAX)*m_r;
      
        
        return ret;
    }
    
    void addExample(const GNGExample  * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseLine(){
        
    }
};
/** Example database for GNG */
 class GNGDatabaseSphere : public GNGDatabase{
public:
    double m_center[GNG_MAX_DIM];
    double m_r;
     int getSize() const{ return 100000000; }
    
    GNGDatabaseSphere(double *center, double r): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*GNG_DIM);
        m_r=r;
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){  
        throw 1; //not implemented
    }
    
    GNGExample drawExample()  const{
        GNGExample ret(3);
         
     
    
        double alfa=6.18*((double)rand() / RAND_MAX);
        double beta=3.14*((double)rand() / RAND_MAX);

        
       ret.position[0] = m_r*cos(beta) + m_center[0];
        ret.position[1] = m_r*sin(beta)*cos(alfa)+ m_center[1];
        ret.position[2] = m_r*sin(beta)*sin(alfa)+m_center[2];
      
        
        return ret;
    }
    
    void addExample(const GNGExample  * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseSphere(){
        
    }
};

/** Example database for GNG */
class GNGDatabaseRec : public GNGDatabase{
public:
     int getSize() const{ return 100000000; }
    
    GNGDatabaseRec(): GNGDatabase(3){
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){   
        throw 1; //not implemented
    }
    
   GNGExample drawExample()  const{
        GNGExample ret(3);

        ret.position[0] = ((double)rand() / RAND_MAX);
        ret.position[1] = ((double)rand() / RAND_MAX);
        ret.position[2] = ((double)rand() / RAND_MAX);
      
        
        return ret;
    }
    
    void addExample(const GNGExample  * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseRec(){
        
    }
};

/** Example database for GNG */
class GNGDatabasePlane : public GNGDatabase{
public:
     
     int getSize() const{ return 100000000; }
     double m_a;
     double m_center[GNG_MAX_DIM];
     
    GNGDatabasePlane(double * center, double a): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*GNG_DIM);
        m_a=a;
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample()  const{
        GNGExample ret(3);
     
        ret.position[1]=m_center[1];
        ret.position[0]=m_a*((double)rand() / RAND_MAX)+m_center[0];
        ret.position[2]=m_a*((double)rand() / RAND_MAX)+m_center[2];
        
        return ret;
    }
    
    void addExample(const GNGExample  * ex){
     
        throw 1;
    }
    
    ~GNGDatabasePlane(){
        
    }
};

/**
 * Database basing its storage in template class parameter. Each input has
 * N+1 dimensionality, where N+1th coordinate is probability of drawing
 * this sample.
 * 
 * @note VectorStorage should implement main functions of std::vector collection
 * @note Mutex should be inteprocess::inteprocess_mutex or boost::mutex
 */
template<typename VectorStorage ,typename Mutex>
class GNGDatabaseProbabilistic: public GNGDatabase
{
    int index;   
    
    
    /**
     * Synchronizes threads that accesses the database (to add incoming examples and etc.)
     */
    mutable Mutex * database_mutex; 


    /** Pointer to storage vector. Note that it is a pointer, because we can choose quite strange storage policy (file-based, network-based),
     * and it is convenient to be able to specify it by passing object that is already configured and constructed.
     */
    VectorStorage * g_database;
    void grow_database(){
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSmain.cpp:273:40: error: no matching function for call to ‘GNGDatabaseSimple::GNGDatabaseSimple()’imple::resizing");
        #endif
        //TODO: pause the algorithm here, because every vector can hypothetically get reallocated
        g_database->reserve(g_database->capacity()*2);
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
        #endif
    }
public:
    int getSize() const{ 
        int ret; database_mutex->lock();   
        ret=(int)(g_database->size());  database_mutex->unlock(); return ret;
    }

    GNGDatabaseProbabilistic(Mutex * database_mutex, VectorStorage* alc, int dim): 
    database_mutex(database_mutex),index(0), g_database(alc) , GNGDatabase(dim){
        g_database->reserve(100);
        __init_rnd();
    }

    void removeExample(const GNGExample  * ex){
        throw 1; //not implemented
    }

    /** Draw example according to the probability distribution */
    GNGExample drawExample() const{
        database_mutex->lock();
        
        GNGExample ex(dim+1); //casting up the tree
        do{ //rejection sampling
         ex=(*g_database)[__int_rnd(0,g_database->size()-1)]; 
        }while(ex.position[GNG_DIM]>__double_rnd(0,1.0));


        database_mutex->unlock();
        return ex;
    }

    void addExample(const GNGExample  * ex2){
        
        if(ex2->getLength() != this->dim + 1) throw invalid_argument("Wrong example dimensionality");
        
        database_mutex->lock();
        if(g_database->size() == g_database->capacity()) grow_database();
        g_database->push_back(*ex2); //operator= kopiowania, ale jest to struct wiec nie trzeba nic pisac
        database_mutex->unlock();
    }
private:

};

/**
 * Database basing its storage in template class parameter. Each input has
 * N dimensionality
 * 
 * @note VectorStorage should implement main functions of std::vector collection
 * @note Mutex should be inteprocess::inteprocess_mutex or boost::mutex
 */
template<class VectorStorage = SHGNGExampleDatabase,
        class Mutex = boost::interprocess::interprocess_mutex
>
class GNGDatabaseSimple: public GNGDatabase
{
    mutable Mutex * database_mutex; //wazne zeby stworzone przed kreacja watkow
    
    int index;
    VectorStorage *g_database;
    void grow_database(){
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSmain.cpp:273:40: error: no matching function for call to ‘GNGDatabaseSimple::GNGDatabaseSimple()’imple::resizing");
        #endif
        g_database->reserve(g_database->capacity()*2);
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
        #endif
    }
public:
    int getSize() const{ int ret; database_mutex->lock();     ret=(int)(g_database->size());  database_mutex->unlock(); return ret;}
    
    GNGDatabaseSimple( Mutex * database_mutex, VectorStorage * alc, int dim
    ): database_mutex(database_mutex),g_database(alc), index(0),GNGDatabase(dim){
        g_database->reserve(100);
        __init_rnd();
    }
    
    void removeExample(const GNGExample  * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample() const{
        database_mutex->lock();
        GNGExample & ex= (*g_database)[__int_rnd(0,g_database->size()-1)];
        database_mutex->unlock();
        return ex;
    }
    
    void addExample(const GNGExample  * ex){
        database_mutex->lock();
        if(g_database->size() == g_database->capacity()) grow_database();
        g_database->push_back(*ex); //operator= kopiowania, ale jest to struct wiec nie trzeba nic pisac
        database_mutex->unlock();
    }
private:
};

#endif	/* GNGEXAMPLEMANAGER_H */

