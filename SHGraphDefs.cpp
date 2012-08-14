/* 
 * File:   SHGraphDefs.cpp
 * Author: staszek
 * 
 * Created on 11 sierpień 2012, 08:19
 */

#include "SHGraphDefs.h"

ShmemAllocator* SHVector::alloc_inst = 0;
ExtMemoryManager* SHVector::mm = 0;

ExtMemoryManager * SHNode::mm = 0;


ShmemAllocator* GNGVector::alloc_inst = 0;
ExtMemoryManager* GNGVector::mm = 0;

 ExtMemoryManager * GNGNode::mm = 0;

