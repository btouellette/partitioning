#ifndef PART_GRAPH_H_
#define PART_GRAPH_H_

#include <list>
#include <string>

class Vertex {
private:
	std::string label;
};

class Graph {
private:
	std::list< std::pair<Vertex*, Vertex*> > edges;
public:
	Graph() {}
};

class Hyperedge {
private:
	std::list<Vertex*> vertices;
};

class Hypergraph {
private:
	std::list<Hyperedge> nets;
public:
	Hypergraph() {}
	int num_pins;
	int num_nets;
	int num_modules;
	int pad_offset;
};

#endif // PART_GRAPH_H_
