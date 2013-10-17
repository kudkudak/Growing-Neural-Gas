/* 
 * File:   GNGInclude.h
 * Author: staszek
 *
 * Created on 12 sierpień 2012, 11:56
 */

#ifndef GNGINCLUDE_H
#define	GNGINCLUDE_H

#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>




#include "GNGGlobals.h"

#include "GNGAlgorithmControl.h"

#include "UniformGrid.h"

#include "SHMemoryManager.h"
#include "SHGraphDefs.h"

#include "GNGGraph.h"
#include "GNGDatabase.h"
#include "GNGAlgorithm.h"

#include "ExtGraphNodeManager.h"
#include "ExtMemoryManager.h"

#include "DebugCollector.h"

#include "GNGDefines.h"



#endif	/* GNGINCLUDE_H */

