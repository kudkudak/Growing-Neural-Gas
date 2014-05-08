/* 
 * File:   GNGLazyErrorHeap.h
 * Author: staszek
 *
 * Created on August 21, 2012, 4:00 AM
 */

#ifndef GNGLAZYERRORHEAP_H
#define	GNGLAZYERRORHEAP_H

#include "Heap.h"
#include "GNGGraph.h"

struct ErrorNode{
    double error;
    int i;
    ErrorNode(const ErrorNode & orig){ error = orig.error; i = orig.i; }
    ErrorNode(double error, int i):error(error), i(i){}
    ErrorNode(){}
    bool operator>(const ErrorNode& rhs){ return error>rhs.error;}
    bool operator<(const ErrorNode& rhs){ return error<rhs.error;}
    bool operator>=(const ErrorNode& rhs){ return error>=rhs.error;}
    bool operator<=(const ErrorNode& rhs){ return error<=rhs.error;}
    friend std::ostream & operator<<(std::ostream & out, const ErrorNode & rhs){ out<<"("<<rhs.error<<","<<rhs.i<<")"; return out;}
};

class GNGLazyErrorHeap: protected Heap<ErrorNode>{
    typedef Heap<ErrorNode> super;
    
protected:   
    std::vector<void*> m_buffer; // <HeapNode*, GNGNode->nr>
    std::vector<bool> m_isonlist;

    
    int m_buffer_size;
    void checkBufferSize(){
        if(m_buffer_size >= SIZE(m_buffer)){ m_buffer.resize(3*m_buffer_size); m_isonlist.resize(3*m_buffer_size); } //domyslnie bool ma false (jako T())
    }
    
    std::list<int > L; //list of nodes to be inserted on lazy top operation
public:
     std::list<int> & getLazyList()  { return L; }
    
    GNGLazyErrorHeap():m_buffer_size(0),super(){}
    
    void insertLazy(int nr){
       m_buffer_size = std::max(m_buffer_size, nr + 1);
       checkBufferSize();        
        
        if(!m_isonlist[nr]){
            L.push_back(nr);
            m_isonlist[nr]=true;
        }
    }
    
    void updateLazy(int nr){
        m_buffer_size = std::max(m_buffer_size, nr + 1);
        checkBufferSize();
        if(m_buffer[nr]) super::remove(m_buffer[nr]);
        m_buffer[nr]=0;
        
        
        if(!m_isonlist[nr]){
            L.push_back(nr);
           m_isonlist[nr]=true;
        }
    }
    
    void update(int nr,double new_error){
        m_buffer_size = std::max(m_buffer_size, nr + 1);
        checkBufferSize();
        
        if(m_buffer[nr]) super::remove(m_buffer[nr]);
        m_buffer[nr] = super::insert(ErrorNode(new_error,nr));
    }    
    
    void insert(int nr, double error){
        m_buffer_size = std::max(m_buffer_size,nr+1);
        checkBufferSize();
        
        if(m_buffer[nr]==0)
                m_buffer[nr]=reinterpret_cast<void*>(
                super::insert(ErrorNode(error,nr))
         );
        m_isonlist[nr]=false;
    }
    
    ErrorNode extractMax(){
        
        ErrorNode max = super::extractMax();
        
        m_buffer[max.i]=0; //mark that it is removed from the heap
        return max;
    }
    
    using super::print;
    using super::getSize;
    
};

#endif	/* GNGLAZYERRORHEAP_H */

