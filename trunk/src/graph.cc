#include "graph.h"

using namespace std;

Vertex* newVertex(char *name) {
	Vertex *vertex = new Vertex();
	if(name != NULL) {
		vertex->label.assign(name);
	}
	return vertex;
}

void addEdgeToVertices(Edge *edge) {
	edge->source->edges.push_front(edge);
	edge->sink->edges.push_front(edge);
}

Edge* newEdge(Vertex* vertex1, Vertex* vertex2) {
	Edge *edge = new Edge();
	edge->source = vertex1;
	edge->sink = vertex2;
	return edge;
}

Edge* newEdge(Vertex* vertex1, Vertex* vertex2, float edge_weight) {
	Edge *edge = newEdge(vertex1, vertex2); 
	edge->weight = edge_weight;
	return edge;
}

void addEdge(Graph *graph, Edge *edge) {
	graph->edges.push_front(edge);
}

void addVertex(Graph *graph, Vertex *vertex) {
	graph->vertices.push_front(vertex);
}

Hyperedge* newHyperedge(Vertex *vertex) {
	Hyperedge *hyperedge = new Hyperedge();
	hyperedge->vertices.push_front(vertex);
	return hyperedge;
}

void addVertex(Hyperedge *hyperedge, Vertex *vertex) {
	hyperedge->vertices.push_front(vertex);
}

void addVertex(Hypergraph *hypergraph, Vertex *vertex) {
	hypergraph->vertices.push_front(vertex);
}

void addNet(Hypergraph *hypergraph, Hyperedge *hyperedge) {
	hypergraph->nets.push_front(hyperedge);
}

bool compare_edges_undirected(Edge *first, Edge *second) {
	// Compares the min vertex pointer from each edge
	Vertex *vertex1 = (first->source < first->sink) ? first->source : first->sink;
	Vertex *vertex2 = (second->source < second->sink) ? second->source : second->sink;
	// If the min vertex is the same for each edge sort by the other
	if (vertex1 == vertex2) {
		vertex1 = (vertex1 == first->sink) ? first->source : first->sink;
		vertex2 = (vertex2 == second->sink) ? second->source : second->sink;
	}
	return vertex1 < vertex2;
}

bool compare_edges_directed(Edge *first, Edge *second) {
	// Compares the source from each edge
	if (first->source != second->source) {
		return first->source < second->source;
	}
	return first->sink < second->sink;
}

bool merge_same_edges(Edge *first, Edge *second) {
	if ((first->source == second->source && first->sink == second->sink) ||
		(first->source == second->sink   && first->sink == second->source)) {
		first->weight += second->weight;
		return true;
	}
	return false;
}

Graph* convertToGraph(Hypergraph *hypergraph) {
	// Create a new graph to insert all our edges into
	// We'll take each net and form a clique out of it
	Graph *graph = new Graph();
	// Copy in all the vertices
	graph->vertices = hypergraph->vertices;

	list<Hyperedge*>::iterator h_it;
	// For every net in the hypergraph
	for (h_it = hypergraph->nets.begin(); h_it != hypergraph->nets.end(); h_it++) {
		Hyperedge *net = *h_it;
		list<Vertex*> vertices = net->vertices;
		// Our weight is defined as 1/(k-1)
		float weight = 1.0/(vertices.size()-1);
		list<Vertex*>::iterator v_it;
		// For every vertex in the net
		for (v_it = vertices.begin(); v_it != vertices.end(); v_it++) {
			Vertex *vertex1 = *v_it;
			list<Vertex*>::iterator v_it2;
			// For every vertex in the net (again)
			for (v_it2 = vertices.begin(); v_it2 != vertices.end(); v_it2++) {
				// Make an edge between this vertex and every other vertex in the net
				// To avoid making edges both ways restrict the edges to point from the lower vertex to the higher one
				Vertex *vertex2 = *v_it2;
				if (vertex1 != vertex2 && vertex1 < vertex2) {
					addEdge(graph, newEdge(vertex1, vertex2, weight));
				}
			}
		}
	}
	// Now merge duplicate edges
	// We do this by sorting the list of edges and then merging consecutive elements
	graph->edges.sort(compare_edges_undirected);
	graph->edges.unique(merge_same_edges);
	// Now populate the vertex lists with the appropriate edges after merging
	list<Edge*>::iterator e_it;
	for (e_it = graph->edges.begin(); e_it != graph->edges.end(); e_it++) {	
		addEdgeToVertices(*e_it);
	}
	return graph;
}

void printGraph(Graph *graph) {
	list<Edge*> edges = graph->edges;
	list<Edge*>::iterator e_it;
	// For every edge
	for (e_it = edges.begin(); e_it != edges.end(); e_it++) {
		cout << "Edge: " << (*e_it)->source->label << "-" << (*e_it)->sink->label << " " << (*e_it)->weight << endl;
	}
	list<Vertex*>::iterator v_it;
	for (v_it = graph->vertices.begin(); v_it != graph->vertices.end(); v_it++) {
		cout << "Vertex: " << (*v_it)->label << " " << (*v_it)->edges.size() << endl;
		edges = (*v_it)->edges;
		for (e_it = edges.begin(); e_it != edges.end(); e_it++) {
			cout << "  Edge: " << (*e_it)->source->label << "-" << (*e_it)->sink->label << " " << (*e_it)->weight << endl;
		}
	}
}

void printHypergraph(Hypergraph *hypergraph) {
	list<Hyperedge*> nets = hypergraph->nets;
	list<Hyperedge*>::iterator n_it;
	// For every net
	for (n_it = nets.begin(); n_it != nets.end(); n_it++) {
		cout << "Net:" << endl;
		list<Vertex*> vertices = (*n_it)->vertices;
		list<Vertex*>::iterator v_it;
		// Print out every vertex's name
		for (v_it = vertices.begin(); v_it != vertices.end(); v_it++) {
			cout << "  " << (*v_it)->label << endl; 
		}
	}
}
