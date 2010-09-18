#ifndef PART_GRAPH_H_
#define PART_GRAPH_H_

#include <list>
#include <iostream>
#include <string>

struct Edge;
struct Hyperedge;

struct Vertex {
	std::string label;
	std::list<Edge*> edges;
	std::list<Hyperedge*> nets;
	float weight;
};

Vertex* newVertex(char *name);
void addEdge(Vertex*, Edge*);
void addHyperedge(Vertex*, Hyperedge*);

struct Edge {
	Vertex *source;
	Vertex *sink;
	float weight;
};

Edge* newEdge(Vertex*, Vertex*);
Edge* newEdge(Vertex*, Vertex*, float);
void setSource(Edge*, Vertex*);
void setSink(Edge*, Vertex*);

struct Graph {
	std::list<Edge*> edges;
};

void addEdge(Graph*, Edge*);

struct Hyperedge {
	std::list<Vertex*> vertices;
};

Hyperedge* newHyperedge(Vertex*);
void addVertex(Hyperedge*, Vertex*);

struct Hypergraph {
	std::list<Hyperedge*> nets;
	int num_pins;
	int num_nets;
	int num_modules;
	int pad_offset;
};

void addNet(Hypergraph*, Hyperedge*);
Graph* convertToGraph(Hypergraph*);
void printHypergraph(Hypergraph*);

#endif // PART_GRAPH_H_
