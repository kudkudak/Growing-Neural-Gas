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
extern DebugCollector dbg;

#define GNG_MAX_DIM 100

//struct-like : zeby latwo bylo przekazywac przez shm

class GNGExample{
public:      
    static int N;
    
    
    double position[GNG_MAX_DIM];
    GNGExample(double *_position){
       
        memcpy(&position[0],_position,sizeof(double)*N);
    }
    GNGExample(){
       
    }
    
    void operator = (const GNGExample & rhs){
        memcpy(&position[0],rhs.position,sizeof(double)*N);
    }
};


//TODO: mutex dodac


class GNGDatabase
{
public:
    GNGDatabase(){}
    
    virtual GNGExample drawExample() const=0 ;
    virtual void addExample(GNGExample const * ex )=0;
    virtual void removeExample(GNGExample const * ex)=0;
    virtual int getSize() const=0;
    
    virtual ~GNGDatabase(){}
private:
    GNGDatabase(const GNGDatabase& orig){}
};

typedef boost::interprocess::allocator<GNGExample, boost::interprocess::managed_shared_memory::segment_manager>  SHGNGExampleDatabaseAllocator;
typedef boost::interprocess::vector<GNGExample, SHGNGExampleDatabaseAllocator> SHGNGExampleDatabase;

class GNGDatabaseSimple: public GNGDatabase
{
    mutable boost::interprocess::interprocess_mutex * database_mutex; //wazne zeby stworzone przed kreacja watkow
    
    int index;
    SHGNGExampleDatabase *g_database;
   //vector<GNGExample> *g_database;
    void grow_database(){
        dbg.push_back(1,"GNGDatabaseSimple::resizing");
        g_database->reserve(g_database->capacity()*2);
        dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
    }
public:
    int getSize() const{ int ret; database_mutex->lock();     ret=(int)(g_database->size());  database_mutex->unlock(); return ret;}
    
    GNGDatabaseSimple( boost::interprocess::interprocess_mutex * database_mutex, SHGNGExampleDatabase * alc): database_mutex(database_mutex),g_database(alc), index(0),GNGDatabase(){
      
        g_database->reserve(100);
        __init_rnd();
    }
    
    void removeExample(GNGExample const * ex){
        database_mutex->lock();    
        
        
        throw 1; //not implemented
        
        database_mutex->unlock();
    }
    
    GNGExample drawExample() const{
        GNGExample ret;
         
        database_mutex->lock();  
        
        dbg.push_back(-200,"GNGDatabaseSimple:: size = "+to_string(index)+" drawing!");
        
        int draw = __int_rnd(0,g_database->size()-1);
        
        ret =  (*g_database)[draw];
      
        database_mutex->unlock();
        
        return ret;
    }
    
    void addExample(GNGExample const * ex){
     
        database_mutex->lock();    
       
       
        if(g_database->size() == g_database->capacity()) grow_database();
        g_database->push_back(*ex); //operator= kopiowania, ale jest to struct wiec nie trzeba nic pisac
        
        database_mutex->unlock();
    }
    
    ~GNGDatabaseSimple(){
        
    }
private:

};

#endif	/* GNGEXAMPLEMANAGER_H */

