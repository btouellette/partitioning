#ifndef PART_GRAPH_H_
#define PART_GRAPH_H_

#include <list>
#include <iostream>
#include <string>

class Edge;
class Hyperedge;

class Vertex {
private:
	std::string label;
	std::list<Edge*> edges;
	std::list<Hyperedge*> nets;
	float weight;
public:
	Vertex(char *name);

	void addEdge(Edge*);
	void addHyperedge(Hyperedge*);

	friend std::ostream& operator<< (std::ostream&, const Vertex&);
};

class Edge {
private:
	Vertex *source;
	Vertex *sink;
public:
	Edge() {}

	void setSource(Vertex*);
	void setSink(Vertex*);
};

class Graph {
private:
	std::list<Edge*> edges;
	std::list<float> weights;
public:
	Graph() {}

	void addEdge(Edge*);
	void addEdge(Edge*, float);
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
	Graph* convertToGraph();

	friend std::ostream& operator<< (std::ostream&, const Hypergraph&);
};


#endif // PART_GRAPH_H_
