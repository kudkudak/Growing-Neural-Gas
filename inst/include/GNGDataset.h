/*
* File: GNGExampleManager.h
* Author: Stanisław "kudkudak" Jastrzebski
*
* Created on 11 sierpień 2012, 10:47
*/
#ifndef GNGDATABASE_H
#define GNGDATABASE_H

#include <algorithm>
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include "SHMemoryManager.h"
#include "Utils.h"
#include "DebugCollector.h"
#include "GNGGlobals.h"
#include "GNGConfiguration.h"

/** Database for growing neural gas interface
*
 * Example is its interiors just an double array with relevant data at different positions.
 * What specifies 
 * 
* @note Drawing example is not very time-expensive comapred to other problems (like
* neighbour search). Therefore it is locking on addExample and drawExample
*/
class GNGDataset
{
public:

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
    
    virtual int getDataDim() const=0;
    
    virtual int getGNGDim() const =0;

    ///Returns index of example drawn
    virtual unsigned int drawExample() const=0 ;
    
    ///Retrieves pointer to position 
    virtual const double * getPosition(unsigned int) const=0;
    
    ///Retrieves pointer to metadata, with unsigned int
    virtual const double * getMetaData(unsigned int) const=0;

    ///Retrieves pointer to vertex data, with unsigned int as descriptor of meta
    virtual const double * getVertexData(unsigned int) const=0;
    
    virtual void addExamples(const void *, unsigned int count)=0;
    
    virtual void removeExample(unsigned int)=0;
    
    virtual int getSize() const=0;
    
    virtual ~GNGDataset(){}
private:
    GNGDataset(const GNGDataset& orig){}

    GNGDataset(){}
};



/*
 * Abstracts away storage from database double*
 * Possible storages:
 * 
 * * arma matrix
 * * raw memory array
 * * bigmemory backed matrix
 * * csv file (!!)
 * 
 * @note Not supposed to take care of synchronization problems
 */
template<class BaseType, class StorageType>
class GNGDatasetStorage{
    unsigned int dim_;
public:
    typedef typename  BaseType baseType;
    typedef typename  StorageType storageType;
    
    GNGDatasetStorage(unsigned int dim): dim_(dim)
    {
    }

    


    int dim() const{
        return dim_;
    }    
    
    
    virtual const BaseType * getData(unsigned int) const=0;
 
    virtual unsigned int getSize() const=0;
    ///Add examples to memory
    virtual void addData(StorageType *,  unsigned int) =0;
    ///Will take ownership of x, no copy involved
    virtual void take(StorageType *) =0;

    virtual ~GNGDatasetStorage(){

    }
};


///Most basic RAM storage
class GNGDatasetStorageRAM: GNGDatasetStorage<double, std::vector<double> >{
    std::vector<double> * storage_;
    unsigned int num_examples_;
    bool init_;
public:
    ///Note that dimensionality is not equal to dimensionality of position pointer
    GNGDatasetStorageRAM(unsigned int dim): 
    	GNGDatasetStorage<double, std::vector<double> >(dim), init_(false), num_examples_(0){
        storage_ = new std::vector<double>();
        storage_->resize(10);
    }
    virtual const double * getData(unsigned int idx) const{
        if(idx > storage_->size()*dim()) return 0;
        return &(*storage_)[idx*dim()];
    }
    virtual void take(std::vector<double> * mem){
        delete storage_; //no auto_ptr because assigment handling is not automatically deleting
        storage_ = mem;
        num_examples_ = storage_->size();
    }
    virtual unsigned int getSize() const{
        return storage_->size()/dim();
    }
    
    virtual void addData(std::vector<double> * x, unsigned int count){
        //Always take first batch, do not copy
        if(!init_){
            init_ = true;
            return take(x);
        }
        
        
        if(x->size() + num_examples_ > storage_->size() ){
            if(storage_->size() < 2e6)
                storage_->resize(2*storage_->size());   
            else
                storage_->resize((unsigned int)(1.2*storage_->size()));      
         }
        std::copy(x->begin(), x->end(), storage_->begin() + num_examples_*dim());
        num_examples_ += x->size()/dim();
    }
    ~GNGDatasetStorageRAM(){
        delete storage_;
    }
};


///Storage :< GNGDatabaseStorage
template<class Storage>
class GNGDatasetSampling: public GNGDataset
{
    const unsigned int pos_dim_, meta_dim_, vertex_dim_;
    const unsigned int prob_location_;
    
    boost::mutex * mutex_;
    Storage storage_;
    std::vector<int> data_layout_;
    
   
public:

    
    /*
    * @param prob_location If prob location is -1 (by default) means there 
    * is no probability data in meta data. If there is then 
    */
    GNGDatasetSampling(boost::mutex *mutex, unsigned int pos_dim, 
            unsigned int vertex_dim, unsigned int meta_dim, unsigned int prob_location=-1): 
    mutex_(mutex), pos_dim_(pos_dim), vertex_dim_(meta_dim_), meta_dim_(prob_location),
    prob_location_(prob_location)
    {
        //Data layout 
        data_layout_.push_back(pos_dim);
        data_layout_.push_back(meta_dim_);
        data_layout_.push_back(vertex_dim_)
        
        __init_rnd();
    }
    

    
     ///Retrieves pointer to position 
    const Storage::baseType * getPosition(unsigned int i) const{
        boost::mutex::scoped_lock(*mutex_);
        
        return &storage_.getData()[0];
    }
    
    ///Retrieves pointer to metadata, with unsigned int
    const Storage::baseType * getMetaData(unsigned int i) const{
        boost::mutex::scoped_lock(*mutex_);
        
        if(meta_dim_==0) return 0;
        
        return &storage_.getData()[pos_dim_+vertex_dim_];
    }

    ///Retrieves pointer to vertex data, with unsigned int as descriptor of meta
    const Storage::baseType * getVertexData(unsigned int i) const{
        boost::mutex::scoped_lock(*mutex_);
        
        if(vertex_dim_==0) return 0;
        
        return &storage_.getData()[pos_dim_];
    }
    
    
    unsigned int drawExample() const{
        boost::mutex::scoped_lock(*mutex_);
        if(prob_location_==-1){
            return index = __int_rnd(0,storage_.getSize()-1);
        }else{
            const double * ex;
            unsigned int index;
            
            //Sample
            do{ 
             index = __int_rnd(0,storage_.getSize()-1);   
             ex=storage_.getData(index);
            }while(ex[pos_dim_+meta_dim_+vertex_dim_+prob_location_]<__double_rnd(0,1.0));
            
            return index;    
        }
    }
    
    void addExamples(const void * memptr, unsigned int count){
    	const Storage::storageType * examples =
    			reinterpret_cast<Storage::storageType *>(memptr);
        boost::mutex::scoped_lock(*mutex_);
        storage_.addData(examples, count);
    }
    
    virtual std::vector<int> getDataLayout() const{
    	return data_layout_;
    }
    
    void removeExample(unsigned int index){
        throw 1; //not implemented
    }
    int getSize() const{ 
        boost::mutex::scoped_lock(*mutex_);
        return storage_.getSize();
    } 
    
    virtual int getDataDim() const{
        return pos_dim_+meta_dim_+vertex_dim_;
    }
    
    virtual int getGNGDim() const{
        return pos_dim_;
    }   
private:
};


#endif
/* GNGEXAMPLEMANAGER_H */
