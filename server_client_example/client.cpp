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


/*
 * 
 */
int main(int argc, char** argv) { 
    managed_shared_memory *  mshm=new managed_shared_memory(open_only,"SH");
    
    offset_ptr< SHNode > * ptr = mshm->find_or_construct< offset_ptr<SHNode> >("SHPoolPtr")();
    
   // SHNode::mm = &mshm;
   // SHVector::mm = &mshm;
   // SHVector::alloc_inst = new ShmemAllocator(mshm->get_segment_manager());
    
    ExtGraphNodeManager<SHNode, SHEdge, SHVector> egnm(&(*(*ptr)),2,1000,2);
    
    while(1) cout<<egnm.reportPool()<<endl;
    
    
    
    return 0;
}


