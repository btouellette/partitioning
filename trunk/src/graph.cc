#include "graph.h"

using namespace std;

Vertex::Vertex(char *name) {
	if(name != NULL) {
		label.assign(name);
	}
}

void Vertex::addEdge(Edge *edge){
	edges.push_front(edge);
}

void Vertex::addHyperedge(Hyperedge *hyperedge) {
	nets.push_front(hyperedge);
}

void Edge::setSource(Vertex *vertex) {
	source = vertex;
	vertex->addEdge(this);
}

void Edge::setSink(Vertex *vertex) {
	sink = vertex;
	vertex->addEdge(this);
}

void Graph::addEdge(Edge *edge) {
	edges.push_front(edge);
}

void Graph::addEdge(Edge *edge, float weight) {
	edges.push_front(edge);
	weights.push_front(weight);
}

Hyperedge::Hyperedge(Vertex *vertex) {
	vertices.push_front(vertex);
	vertex->addHyperedge(this);
}

void Hyperedge::addVertex(Vertex *vertex) {
	vertices.push_front(vertex);
	vertex->addHyperedge(this);
}

void Hypergraph::addNet(Hyperedge *hyperedge) {
	nets.push_front(hyperedge);
}

Graph* Hypergraph::convertToGraph() {
	Graph *graph = new Graph();
	list<Hyperedge*>::iterator it;
	for (it = nets.begin(); it != nets.end(); it++) {
	}
	return graph;
}

ostream& operator<< (ostream &out, const Vertex &vertex) {
	out << vertex.label << endl; 
	return out;
}
ostream& operator<< (ostream &out, const Hyperedge &hyperedge) {
	list<Vertex*> vertices = hyperedge.vertices;
	list<Vertex*>::iterator it;
	for (it = vertices.begin(); it != vertices.end(); it++) {
		out << "  " << *(*it);
	}
	return out;
}
ostream& operator<< (ostream &out, const Hypergraph &hypergraph) {
	list<Hyperedge*> nets = hypergraph.nets;
	list<Hyperedge*>::iterator it;
	for (it = nets.begin(); it != nets.end(); it++) {
		out << "Net:" << endl;
		out << *(*it);
	}
	return out;
}
