/* 
 * File:   GNGDatabaseExtra.h
 * Author: staszek
 *
 * Created on May 9, 2014, 9:35 PM
 */

#ifndef GNGDATABASEEXTRA_H
#define	GNGDATABASEEXTRA_H

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
#include "GNGDatabase.h"

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



#endif	/* GNGDATABASEEXTRA_H */

