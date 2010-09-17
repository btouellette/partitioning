#ifndef PART_GRAPH_H_
#define PART_GRAPH_H_

#include <list>
#include <iostream>
#include <string>

class Vertex {
private:
	std::string label;
public:
	Vertex(char *name);

	friend std::ostream& operator<< (std::ostream&, const Vertex&);
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
public:
	Hyperedge() {}
	Hyperedge(Vertex*);

	void addVertex(Vertex*);

	friend std::ostream& operator<< (std::ostream&, const Hyperedge&);
};

class Hypergraph {
private:
	std::list<Hyperedge*> nets;
public:
	int num_pins;
	int num_nets;
	int num_modules;
	int pad_offset;

	Hypergraph() {}

	void addNet(Hyperedge*);

	friend std::ostream& operator<< (std::ostream&, const Hypergraph&);
};


#endif // PART_GRAPH_H_
