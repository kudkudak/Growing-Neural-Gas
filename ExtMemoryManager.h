/* 
 * File:   ExtMemoryManager.h
 * Author: staszek
 *
 * Created on 10 sierpień 2012, 15:58
 */

#ifndef EXTMEMORYMANAGER_H
#define	EXTMEMORYMANAGER_H
#include <cstddef>
#include <string>


class ExtMemoryManager{
public:
    

   
    virtual void * allocate(std::size_t, int index=0)=0;
    
    //default constructor will be used

    virtual std::string get_name(int) const=0;
    
    
    virtual bool deallocate(void * ptr,int index=0)=0;
    
    virtual ~ExtMemoryManager(){}
};


#endif	/* EXTMEMORYMANAGER_H */

