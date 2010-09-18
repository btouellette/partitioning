#ifndef PART_GRAPH_H_
#define PART_GRAPH_H_

#include <list>
#include <iostream>
#include <string>

struct Vertex {
	std::string label;
	float weight;
};

Vertex* newVertex(char *name);

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
	std::list<Vertex*> vertices;
};

void addEdge(Graph*, Edge*);
void printGraph(Graph*);

struct Hyperedge {
	std::list<Vertex*> vertices;
	float weight;
};

Hyperedge* newHyperedge(Vertex*);
void addVertex(Hyperedge*, Vertex*);

struct Hypergraph {
	std::list<Hyperedge*> nets;
	std::list<Vertex*> vertices;
	int num_pins;
	int num_nets;
	int num_modules;
	int pad_offset;
};

void addVertex(Hypergraph*, Vertex*);
void addNet(Hypergraph*, Hyperedge*);
Graph* convertToGraph(Hypergraph*);
void printHypergraph(Hypergraph*);

bool compare_edges_undirected(Edge*, Edge*);
bool compare_edges_directed(Edge*, Edge*);

#endif // PART_GRAPH_H_
