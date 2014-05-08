#include <iostream>
#include <stdlib.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphml.hpp>

using namespace boost;
using namespace std;
using boost::get;



struct Vertex
{
    int index;
};

int main(int argc, char *argv[])
{

  typedef adjacency_list<vecS, vecS, bidirectionalS, 
    Vertex > Graph;

  const int num_vertices = 9;
  Graph G(num_vertices);

  boost::graph_traits<Graph>::vertex_descriptor u, v;
  typedef boost::property<boost::vertex_index_t, int> VID_prop;

  u = add_vertex(G);
  v = add_vertex(G);
  add_edge(u, v, G);
  
  G[u].index = 20;
 
  boost::property_map< Graph, int Vertex::* >::type v_id( boost::get(&Vertex::index, G) );
  // You can now use "handle_to_something" for whatever deed you are interested in.


  cout<<get(v_id, u)<<endl;
  
  dynamic_properties p;
  p.property("vertex_index", boost::get(&Vertex::index, G));
  
  cout<<get("vertex_index", p, u)<<endl;

  write_graphml(std::cout, G, p, true);
  
  return 0;
}



 