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

#include <boost/shared_ptr.hpp>

#include "SHMemoryManager.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGGlobals.h"

struct GNGExample;

typedef boost::interprocess::allocator<GNGExample, boost::interprocess::managed_shared_memory::segment_manager> SHGNGExampleDatabaseAllocator;
typedef boost::interprocess::vector<GNGExample, SHGNGExampleDatabaseAllocator> SHGNGExampleDatabase;




/** Database for growing neural gas interface
*
 * Example is its interiors just an double array with relevant data at different positions.
 * What specifies 
 * 
* @note Drawing example is not very time-expensive comapred to other problems (like
* neighbour search). Therefore it is locking on addExample and drawExample
*/
class GNGDatabase
{
public:
    GNGDatabase(int dim): dim(dim){}
    
    /*
     * @returns Layout for example. We want examples to be laid in continous
     * array of doubles for performance and storage convenience so basically
     * an example can have any dimensionality and this function returns array
     * with specified 3 checkpoints:
     *    * Start of position data
     *    * Start of vertex data (that will be majority voted in algorithm)
     *    * Start of metadata (most likely probability of being sampled)
     */
    virtual std::vector<int> getDataLayout() const=0;
    
    virtual int getDim() const{ return this->dim; }

    ///Returns index of example drawn
    virtual unsigned int drawExample() const=0 ;
    
    ///Retrieves pointer to position 
    virtual const double * getPosition(unsigned int) const=0;
    
    ///Retrieves pointer to metadata, with unsigned int as descriptor of meta
    virtual const double * getMetaPtr(unsigned int, unsigned int) const=0;
    
    virtual void addExample(const double * ex )=0;
    
    virtual void removeExample(unsigned int)=0;
    
    virtual int getSize() const=0;
    
    virtual ~GNGDatabase(){}
private:
    GNGDatabase(const GNGDatabase& orig){}
    unsigned int dim;
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
        //TODO: pause the algorithm here, because every vector can hypothetically get reallocated
        g_database->reserve(g_database->capacity()*2);
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
        do{ //rejection sampling TODO: improve
         ex=&(*g_database)[__int_rnd(0,g_database->size()-1)];
        }while(ex->position[this->getDim()]<__double_rnd(0,1.0));


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


//TODO: note as bagging dataset..

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
        
        DBG(1,"GNGDatabaseSimple::resizing started");
        
        g_database->reserve(g_database->capacity()*2);
        
        DBG(1,"GNGDatabaseSimple::resizing completed");
        
    }
public:
    int getSize() const{ 
        int ret; 
        database_mutex->lock(); 
        ret=(int)(g_database->size()); database_mutex->unlock(); return ret;}
    
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
















#endif

        /* GNGEXAMPLEMANAGER_H */