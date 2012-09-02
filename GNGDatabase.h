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

//why not in GNGGlobals
#include "GNGGlobals.h"

extern DebugCollector dbg;
extern int GNG_DIM;

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
    
   
    void operator= (const GNGExample & rhs){
        memcpy(
                &position[0],rhs.position,sizeof(double)*N);
    }
    bool operator== (const GNGExample & rhs){
        double eps=0.00000000001;
        REP(i,N){
            if(position[i]-rhs.position[i]<-eps || position[i]-rhs.position[i]>eps ) return false;
        }
        return true;
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

class GNGDatabaseMeshes: public GNGDatabase{
public:
    std::vector<GNGDatabase*> meshes;    
    
    GNGDatabaseMeshes(): GNGDatabase(){
        __init_rnd();
    }
    
    void removeExample(GNGExample const * ex){
    
        
        throw 1; //not implemented
        

    }
    
    void addMesh(GNGDatabase * mesh){
        meshes.push_back(mesh);
    }
    
    GNGExample drawExample() const{
        
        int i = __int_rnd(0,SIZE(meshes)-1);
        return meshes[i]->drawExample();
    }
    
    void addExample(GNGExample const * ex){
     
        throw 1;
    }
    int getSize() const{ return 100000000; }
    ~GNGDatabaseMeshes(){
        
    }    
};

typedef boost::interprocess::allocator<GNGExample, boost::interprocess::managed_shared_memory::segment_manager>  SHGNGExampleDatabaseAllocator;
typedef boost::interprocess::vector<GNGExample, SHGNGExampleDatabaseAllocator> SHGNGExampleDatabase;

 class GNGDatabaseSphere : public GNGDatabase{
public:
    double m_center[GNG_MAX_DIM];
    double m_r;
     int getSize() const{ return 100000000; }
    
    GNGDatabaseSphere(double *center, double r): GNGDatabase(){
        memcpy(m_center,center,sizeof(double)*GNG_DIM);
        m_r=r;
        __init_rnd();
    }
    
    void removeExample(GNGExample const * ex){
    
        
        throw 1; //not implemented
        

    }
    
    GNGExample drawExample() const{
        GNGExample ret;
         
     
    
        double alfa=6.18*((double)rand() / RAND_MAX);
        double beta=3.14*((double)rand() / RAND_MAX);

        
       ret.position[0] = m_r*cos(beta) + m_center[0];
        ret.position[1] = m_r*sin(beta)*cos(alfa)+ m_center[1];
        ret.position[2] = m_r*sin(beta)*sin(alfa)+m_center[2];
      
        
        return ret;
    }
    
    void addExample(GNGExample const * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseSphere(){
        
    }
};
class GNGDatabaseRec : public GNGDatabase{
public:
     int getSize() const{ return 100000000; }
    
    GNGDatabaseRec(): GNGDatabase(){
   
        __init_rnd();
    }
    
    void removeExample(GNGExample const * ex){
    
        
        throw 1; //not implemented
        

    }
    
    GNGExample drawExample() const{
        GNGExample ret;
         
     
   
        
       ret.position[0] = ((double)rand() / RAND_MAX);
        ret.position[1] = ((double)rand() / RAND_MAX);
        ret.position[2] = ((double)rand() / RAND_MAX);
      
        
        return ret;
    }
    
    void addExample(GNGExample const * ex){
     
        throw 1;
    }
    
    ~GNGDatabaseRec(){
        
    }
};
class GNGDatabasePlane : public GNGDatabase{
public:
     
     int getSize() const{ return 100000000; }
     double m_a;
     double m_center[GNG_MAX_DIM];
     
    GNGDatabasePlane(double * center, double a): GNGDatabase(){
        memcpy(m_center,center,sizeof(double)*GNG_DIM);
        m_a=a;
        __init_rnd();
    }
    
    void removeExample(GNGExample const * ex){
        throw 1; //not implemented
    }
    
    GNGExample drawExample() const{
        GNGExample ret;
         
     
        ret.position[1]=m_center[1];
        ret.position[0]=m_a*((double)rand() / RAND_MAX)+m_center[0];
        ret.position[2]=m_a*((double)rand() / RAND_MAX)+m_center[2];
      
        
        return ret;
    }
    
    void addExample(GNGExample const * ex){
     
        throw 1;
    }
    
    ~GNGDatabasePlane(){
        
    }
};

class GNGDatabaseSimple: public GNGDatabase
{
    mutable boost::interprocess::interprocess_mutex * database_mutex; //wazne zeby stworzone przed kreacja watkow
    
    int index;
    SHGNGExampleDatabase *g_database;
   //vector<GNGExample> *g_database;
    void grow_database(){
        //dbg.push_back(1,"GNGDatabaseSmain.cpp:273:40: error: no matching function for call to ‘GNGDatabaseSimple::GNGDatabaseSimple()’imple::resizing");
        g_database->reserve(g_database->capacity()*2);
        //dbg.push_back(1,"GNGDatabaseSimple::resizing completed");
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
        
        //dbg.push_back(-200,"GNGDatabaseSimple:: size = "+to_string(index)+" drawing!");
        
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

