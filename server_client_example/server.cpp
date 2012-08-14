/* 
 * File:   main.cpp
 * Author: staszek
 *
 * Created on 7 sierpień 2012, 18:27
 */

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include "SHMemoryManager.h"
#include "SHGraphDefs.h"
#include "ExtGraphNodeManager.h"
#include "ExtMemoryManager.h"
#include "DebugCollector.h"

#include<new>

DebugCollector dbg;

using namespace std;
using namespace boost::interprocess;

int main(int argc, char** argv) {
        
    SHMemoryManager shm(100000);
    cout<<shm.get_name()<<endl;
    
    SHNode::mm = &shm;
    SHVector::mm = &shm;
    SHVector::alloc_inst = new ShmemAllocator(shm.get_segment()->get_segment_manager());
    
    ExtGraphNodeManager<SHNode, SHEdge, SHVector> egnm(1000);
    
    egnm.newNode();
    egnm.newNode();
    egnm.addDEdge(0,1);
    egnm.addDEdge(0,2);
    
    offset_ptr< SHNode > * ptr = shm.get_segment()->construct< offset_ptr<SHNode> >("SHPoolPtr")();
    
    *ptr = egnm.getPool();
    
    cout<<egnm.reportPool(false)<<endl;
    
    int a,b;
    while(1) { cin>>a>>b; egnm.addDEdge(a,b); if(a==-1) break; }
    
    cout<<egnm.reportPool(false)<<endl;
    
    return 0;
}





