#include "graph.h"

using namespace std;

Vertex::Vertex(char *name) {
	if(name != NULL) {
		label.assign(name);
	}
}

Hyperedge::Hyperedge(Vertex *vertex) {
	vertices.push_front(vertex);
}

void Hyperedge::addVertex(Vertex *vertex) {
	vertices.push_front(vertex);
}

void Hypergraph::addNet(Hyperedge *hyperedge) {
	nets.push_front(hyperedge);
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
