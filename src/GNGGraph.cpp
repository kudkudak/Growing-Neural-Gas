
#include "GNGGraph.h"

#include <iostream>
#include <fstream>

using namespace std;
std::string writeToGraphML(GNGGraph &g, string filename){
  DBG(4, "writeToGraphML::locking");
  g.lock();

  DBG(4, "writeToGraphML::locked");

  typedef adjacency_list<vecS, vecS, undirectedS, 
    boost_vertex_desc , boost_edge_desc> Graph;


  DBG(4, "writeToGraphML::boost_g defining");

  Graph boost_g(0);

  DBG(4, "writeToGraphML::defined boost_g");

  boost::graph_traits<Graph>::vertex_descriptor u, v;
  typedef boost::property<boost::vertex_index_t, int> VID_prop;

  map<int, boost::graph_traits<Graph>::vertex_descriptor> map_desc;
  
  DBG(4, "writeToGraphML::adding verticeS");
  //Add vertexes
  for(int i=0;i<=g.getMaximumIndex();++i)
    if(g.existsNode(i))
    {
    	DBG(1, to_string(i));
        map_desc[g[i].nr] = add_vertex(boost_g);
        boost_g[map_desc[g[i].nr]].index = g[i].nr;
        boost_g[map_desc[g[i].nr]].error = g[i].error;
        std::string dump = "";
        //TODO: string builder
        for(int j=0;j<g.getDim()-1;++j){
        	dump+=to_string(g[i].position[j])+ " ";

        }
        dump+=to_string(g[i].position[g.getDim()-1]);
        boost_g[map_desc[g[i].nr]].position_dump = dump;
        if(g.getDim() >= 3){
            boost_g[map_desc[g[i].nr]].v0 = g[i].position[0];
            boost_g[map_desc[g[i].nr]].v1 = g[i].position[1];
            boost_g[map_desc[g[i].nr]].v2 = g[i].position[2];
        }
    }
  
  boost::property_map< Graph, double boost_edge_desc::* >::type edge_dist( boost::get(&boost_edge_desc::dist, boost_g) );
  DBG(4, "writeToGraphML::adding edges");
  //Add edges
  for (unsigned int i = 0; i <= g.getMaximumIndex(); ++i)
      if(g.existsNode(i))
      {
            typedef boost::graph_traits<Graph>::edge_descriptor desc;
            desc e;
            bool b;
            FOREACH(edg, g[i]){
            	if(g[i].nr > (*edg)->nr){ //directed!
					add_edge(map_desc[g[i].nr], map_desc[(*edg)->nr],  boost_g);
					boost::tie(e, b) = edge(map_desc[g[i].nr], map_desc[(*edg)->nr],  boost_g);
					edge_dist[e] = g.getDist(i, (*edg)->nr);
            	}
            }
            
      }
  
  

  //Construct properties getters
  dynamic_properties p;
  p.property("gng_index", boost::get(&boost_vertex_desc::index, boost_g));
  p.property("v0", boost::get(&boost_vertex_desc::v0, boost_g));
  p.property("v1", boost::get(&boost_vertex_desc::v1, boost_g));
  p.property("v2", boost::get(&boost_vertex_desc::v2, boost_g));
  p.property("error", boost::get(&boost_vertex_desc::error, boost_g));
  p.property("position_dump", boost::get(&boost_vertex_desc::position_dump, boost_g));
  p.property("dist", boost::get(&boost_edge_desc::dist, boost_g));
  
  
  g.unlock();
  
  if(filename==""){
  
    std::stringstream ss;
    write_graphml(ss, boost_g, p, true);

    return ss.str();
  }
  else{
	 DBG(4, "writeToGraphML::writing to file");
    ofstream myfile;
    myfile.open (filename.c_str());
    std::stringstream ss;
    write_graphml(ss, boost_g, p, true) ;   
    
    myfile << ss.str();
    myfile.close(); 
    
      return "";
  }

}
