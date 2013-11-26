/*
* File: GNGExampleManager.h
* Author: Stanisław "kudkudak" Jastrzebski
*
* Created on 11 sierpień 2012, 10:47
*/
#ifndef GNGDATABASE_H
#define        GNGDATABASE_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include <boost/shared_ptr.hpp>

#include "SHMemoryManager.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGGlobals.h"

struct GNGExample;

typedef boost::interprocess::allocator<GNGExample, boost::interprocess::managed_shared_memory::segment_manager> SHGNGExampleDatabaseAllocator;
typedef boost::interprocess::vector<GNGExample, SHGNGExampleDatabaseAllocator> SHGNGExampleDatabase;


/*
* @brief Example used by database.
*
* It is a vector of numbers. Any extension of GNGExample for simplicity has to add new information as extra dimensions, and after that
* Database should interpret it and pass interpreted version to the algorithm module,.
*
* Every database is casting its examples to this type, because it contains all the information needed by the algorithm (position). All the
* extensions are hidden from the algorithm (like probability).
*
* Every GNGExample can be created from double[] vector, and should be possible to be interpreted as double[] vector (by getPositionPtr),
* which is in GNGExample O(1), but take longer for more sophisticated GNGExample classes
 * 
 * 
 * @note: Think over notion of parameters??
 * 
*
*/
class GNGExample{
    
protected:
    GNGExample(){}
    int dim;
public:
    GNGExample(const double * vect, int _dim):position(_dim), dim(_dim){
        memcpy(&position[0],vect,sizeof(double)*(this->getLength())); //TODO: +1 failure
    }
    GNGExample(int _dim):position(_dim), dim(_dim){
    }
    
    
    vector<double> position;

    double& operator[](unsigned int i) {
        return position[i];
    }
    const double& operator[](unsigned int i) const {
        return position[i];
    }
    
    void operator=(const GNGExample& ex){
        memcpy(&position[0],ex.getPositionPtr(),sizeof(double)*(this->getLength()));
        this->dim = ex.getLength();
    }
    
    bool operator==(const GNGExample & ex) const{
        if(this->getLength()!=ex.getLength()) return false;
        for(int i;i<this->getLength();++i){
            if(getPositionPtr()[i]-ex.getPositionPtr()[i]<-10e-10 ||
                    getPositionPtr()[i]-ex.getPositionPtr()[i]>10e-10 )
                return false;
        }
        return true;
    }
    
    friend ostream & operator<<(ostream & out, const GNGExample & rhs) {
        for(int i;i<rhs.getLength();++i)
            out<<rhs.getPositionPtr()[i]<<",";
        out<<std::endl;
        return out;
    }
     
    int getLength() const{
        return dim;
    }
    
    
    int getDoubleEncodingLength() const{
        return dim;
    }
    
    const double * getDoubleEncoding() const{
        return &this->position[0];
    }
    
    /** Get double[] interpretation of GNGExample */
    const double * getPositionPtr() const{
        return &this->position[0];
    }
};


/** Example used by GNGDatabaseProbabilistic*/
class GNGExampleProbabilistic : public GNGExample{
public:
    GNGExampleProbabilistic(const double * vect, int _dim):GNGExample(_dim+1){
        memcpy(&this->position[0],vect,sizeof(double)*(_dim+1)); //TODO: +1 failure
        
        dim = _dim;
    }
    GNGExampleProbabilistic(int _dim):GNGExample(_dim+1){
        this->position[_dim]=1.0;
        
        dim = _dim;
    }
    double getProbability() const{
        return this->position[dim];
    }
    
    
    int getDoubleEncodingLength() const{
        return dim+1;
    }
};





/** Database for growing neural gas interface
*
* @note For performance issue there is a very strong assumption : once written data point
* doesn't change its pointer position. If it does, it should be very rare, and whole
* algorithm should be stopped (GNGExamples are passed by pointers not copied, maybe it should be changed?)
*/
class GNGDatabase
{
public:
    GNGDatabase(int dim): dim(dim){}
    
    virtual int getDim() const{ return this->dim; }
    virtual GNGExample drawExample() const=0 ;
    virtual void addExample(const GNGExample * ex )=0;
    virtual void removeExample(const GNGExample * ex)=0;
    virtual int getSize() const=0;
    
    virtual ~GNGDatabase(){}
private:
    GNGDatabase(const GNGDatabase& orig){}
    int dim;
    GNGDatabase(){}
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
    boost::shared_ptr<VectorStorage> g_database;
    void grow_database(){
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSimple::resizing");
        #endif
        //TODO: pause the algorithm here, because every vector can hypothetically get reallocated
        g_database->reserve(g_database->capacity()*2);
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
        #endif
    }
public:
    int getSize() const{
        int ret;
        database_mutex->lock();
        ret=(int)(g_database->size());
        database_mutex->unlock();
        return ret;
    }

    
    /**Construct GNGDatabaseProbabilistic
*
* @param database_mutex Mutex used for inner synchronization
* @param alc Object implementing vector functionality used for storage (note: creator should destruct it) of *GNGExampleProbabilistic*
* @param dim Dimensionality of object (note: without "probability dim")
*
*/
    GNGDatabaseProbabilistic(Mutex * database_mutex, boost::shared_ptr<VectorStorage> alc, int dim):
    database_mutex(database_mutex),index(0), g_database(alc) , GNGDatabase(dim){
        g_database->reserve(100);
        __init_rnd();
    }

    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }

    /** Draw example according to the probability distribution */
    GNGExample drawExample() const{
        database_mutex->lock();
        
        GNGExampleProbabilistic * ex; //casting up the tree
        do{ //rejection sampling
         ex=&(*g_database)[__int_rnd(0,g_database->size()-1)];
        }while(ex->position[this->getDim()]>__double_rnd(0,1.0));


        database_mutex->unlock();
        return GNGExample(&ex->position[0],this->getDim());
    }

    void addExample(const GNGExample * ex2){
        if(ex2->getLength() != this->getDim()) throw invalid_argument("Wrong example dimensionality");
        
        const GNGExampleProbabilistic * ex2_casted = reinterpret_cast<const GNGExampleProbabilistic*>(ex2);
        
        database_mutex->lock();
        if(g_database->size() == g_database->capacity()) grow_database();
        g_database->push_back(*ex2_casted); //operator= kopiowania, ale jest to struct wiec nie trzeba nic pisac
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
        dbg.push_back(1,"GNGDatabaseSimple::resizing started");
        #endif
        g_database->reserve(g_database->capacity()*2);
        #ifdef DEBUG
        dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
        #endif
    }
public:
    int getSize() const{ int ret; database_mutex->lock(); ret=(int)(g_database->size()); database_mutex->unlock(); return ret;}
    
    GNGDatabaseSimple( Mutex * database_mutex, VectorStorage * alc, int dim
    ): database_mutex(database_mutex),g_database(alc), index(0),GNGDatabase(dim){
        g_database->reserve(100);
        __init_rnd();
    }
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample() const{
        database_mutex->lock();
        GNGExample & ex= (*g_database)[__int_rnd(0,g_database->size()-1)];
        database_mutex->unlock();
        return ex;
    }
    
    void addExample(const GNGExample * ex){
        database_mutex->lock();
        if(g_database->size() == g_database->capacity()) grow_database();
        g_database->push_back(*ex); //operator= kopiowania, ale jest to struct wiec nie trzeba nic pisac
        database_mutex->unlock();
    }
private:
};















class GNGDatabaseMeshes: public GNGDatabase{
public:
    std::vector<GNGDatabase*> meshes;
    
    GNGDatabaseMeshes(): GNGDatabase(3){
        __init_rnd();
    }
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
    void addMesh(GNGDatabase * mesh){
        meshes.push_back(mesh);
    }
    
    GNGExample drawExample() const{
        int i = __int_rnd(0,SIZE(meshes)-1);
        return meshes[i]->drawExample();
    }
    
    void addExample(const GNGExample * ex){
        throw 1;
    }
    int getSize() const{ return 100000000; }
    ~GNGDatabaseMeshes(){
        
    }
};



/** Example database for GNG */
class GNGDatabaseLine: public GNGDatabase{
public:
    double m_center[3];
    double m_r;
     int getSize() const{ return 100000000; }
    
    GNGDatabaseLine(double *center, double r): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*3);
        m_r=r;
        __init_rnd();
    }
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
   GNGExample drawExample() const{
        GNGExample ret(3);

        
        ret.position[0] = m_center[0];
        ret.position[1] =m_center[1];
        ret.position[2] =((double)rand() / RAND_MAX)*m_r;
      
        
        return ret;
    }
    
    void addExample(const GNGExample * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseLine(){
        
    }
};
/** Example database for GNG */
 class GNGDatabaseSphere : public GNGDatabase{
public:
    double m_center[3];
    double m_r;
     int getSize() const{ return 100000000; }
    
    GNGDatabaseSphere(double *center, double r): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*3);
        m_r=r;
        __init_rnd();
    }
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample() const{
        GNGExample ret(3);
         
     
    
        double alfa=6.18*((double)rand() / RAND_MAX);
        double beta=3.14*((double)rand() / RAND_MAX);

        
       ret.position[0] = m_r*cos(beta) + m_center[0];
        ret.position[1] = m_r*sin(beta)*cos(alfa)+ m_center[1];
        ret.position[2] = m_r*sin(beta)*sin(alfa)+m_center[2];
      
        
        return ret;
    }
    
    void addExample(const GNGExample * ex){
     
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
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
   GNGExample drawExample() const{
        GNGExample ret(3);

        ret.position[0] = ((double)rand() / RAND_MAX);
        ret.position[1] = ((double)rand() / RAND_MAX);
        ret.position[2] = ((double)rand() / RAND_MAX);
      
        
        return ret;
    }
    
    void addExample(const GNGExample * ex){
     
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
     double m_center[3];
     
    GNGDatabasePlane(double * center, double a): GNGDatabase(3){
        memcpy(m_center,center,sizeof(double)*3);
        m_a=a;
        __init_rnd();
    }
    
    void removeExample(const GNGExample * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample() const{
        GNGExample ret(3);
     
        ret.position[1]=m_center[1];
        ret.position[0]=m_a*((double)rand() / RAND_MAX)+m_center[0];
        ret.position[2]=m_a*((double)rand() / RAND_MAX)+m_center[2];
        
        return ret;
    }
    
    void addExample(const GNGExample * ex){
     
        throw 1;
    }
    
    ~GNGDatabasePlane(){
        
    }
};






#endif        /* GNGEXAMPLEMANAGER_H */