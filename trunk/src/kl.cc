#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include <limits.h>

#include "graph.h"

using namespace std;

void initial_partition();
int pass();
int calc_gain(Vertex*, Vertex*);

set<Vertex*> part_1, part_2;
set<Vertex*> unlocked_1, unlocked_2;
Graph* graph;

void kl(Graph* in_graph, int num_runs) {
	graph = in_graph;
	for (int i = 0; i < num_runs; i++) {
		part_1.empty();
		part_2.empty();
		initial_partition();
		while (pass() > 0) {
		}
	}
}

void initial_partition() {
	list<Vertex*>::iterator it;
	// Seed the random number generator
	srand(time(NULL));
	// Randomly partition the vertices into sets
	for (it = graph->vertices.begin(); it != graph->vertices.end(); it++) {
		if (rand()%2) {
			part_1.insert(*it);
			(*it)->partition = 1;
		} else {
			part_2.insert(*it);
			(*it)->partition = 2;
		}
	}
	// Sets are randomly assigned but not initially balanced
	// Swap a random element from the larger set to the smaller set until they are
	int size_1 = part_1.size();
	int size_2 = part_2.size();
	vector<Vertex*> vec;
	if (size_1 > size_2) {
		// Copy the relevant set into a vector 
		copy(part_1.begin(), part_1.end(), inserter(vec, vec.begin())); 
		// Se we can randomly shuffle it in O(n)
		random_shuffle(vec.begin(), vec.end());
		vector<Vertex*>::iterator vec_it = vec.begin();
		// And pull off elements from the random shuffle until the sets are balanced
		while (size_1 > size_2) {
			part_1.erase(*vec_it);
			part_2.insert(*vec_it);
			(*vec_it)->partition = 2;
			vec_it++;
			size_1--;
			size_2++;
		}
	} else if (size_2 > size_1) {
		copy(part_2.begin(), part_2.end(), inserter(vec, vec.begin())); 
		random_shuffle(vec.begin(), vec.end());
		vector<Vertex*>::iterator vec_it = vec.begin();
		while (size_2 > size_1) {
			part_2.erase(*vec_it);
			part_1.insert(*vec_it);
			(*vec_it)->partition = 1;
			vec_it++;
			size_2--;
			size_1++;
		}
	}
}

int pass() {
	int best_gain = 0, total_gain = 0;
	list< pair<Vertex*, Vertex*> > swaps_since_best, swaps_till_best;
	Vertex *vertex1 = NULL, *vertex2 = NULL;
	// Unlock all vertices
	unlocked_1.clear();
	unlocked_2.clear();
	unlocked_1 = part_1;
	unlocked_2 = part_2;
	// While we still have an unlocked pair to match
	while (unlocked_1.size() + unlocked_2.size() >= 2) {
		int max_gain = INT_MIN;
		set<Vertex*>::iterator it_1;
		for (it_1 = unlocked_1.begin(); it_1 != unlocked_1.end(); it_1++) {
			set<Vertex*>::iterator it_2;
			for (it_2 = unlocked_2.begin(); it_2 != unlocked_2.end(); it_2++) {
				int gain = calc_gain(*it_1, *it_2);
				if (gain > max_gain) {
					vertex1 = *it_1;
					vertex2 = *it_2;
					max_gain = gain;
				}
			}
		}
		unlocked_1.erase(vertex1);
		vertex1->partition = 2;
		unlocked_2.erase(vertex2);
		vertex2->partition = 1;
		total_gain += max_gain;
		// Check if after this pass we have a new best
		if (total_gain > best_gain) {
			// Record a new state
			best_gain = total_gain;
			// Splice the since list onto the end of the till list
			swaps_till_best.splice(swaps_till_best.end(), swaps_since_best);
		} else {
			// Or add this pair to the end of the since list
			swaps_since_best.push_back(make_pair(vertex1, vertex2));
		}
	}
	// Apply all the swaps to the stored partitions
	list< pair<Vertex*, Vertex*> >::iterator it;
	for (it = swaps_till_best.begin(); it != swaps_till_best.end(); it++) {
		pair<Vertex*, Vertex*> v_pair = *it;
		part_1.erase(v_pair.first);
		part_2.insert(v_pair.first);
		part_2.erase(v_pair.second);
		part_1.insert(v_pair.second);
	}
	// Undo the partition changes we wrote into the vertices swapped since the best gain
	for (it = swaps_since_best.begin(); it != swaps_since_best.end(); it++) {
		pair<Vertex*, Vertex*> v_pair = *it;
		v_pair.first->partition = 1;
		v_pair.second->partition = 2;
	}
	return best_gain;
}

int calc_gain(Vertex *vertex1, Vertex *vertex2) {
	int gain = 0;
	int vertex1_part = vertex1->partition;
	list<Edge*>::iterator it;
	// Check all the edges out of the first vertex
	for (it = vertex1->edges.begin(); it != vertex1->edges.end(); it++) {
		Edge *edge = *it;
		// Find the vertex in the edge that isn't this one
		Vertex *neighbor = (edge->source == vertex1) ? edge->sink : edge->source;
		if (neighbor == vertex2) {
			// If the neighbor is the other potential swap then we gain nothing from this edge
		} else if (vertex1_part == neighbor->partition) {
			// Moving an edge across the partition that wasn't there before loses
			gain -= edge->weight;
		} else {
			// Moving an edge from the partition into a partition wins
			gain += edge->weight;
		}
	}
	int vertex2_part = vertex1->partition;
	// Check all the edges out of the second vertex
	for (it = vertex2->edges.begin(); it != vertex2->edges.end(); it++) {
		Edge *edge = *it;
		Vertex *neighbor = (edge->source == vertex2) ? edge->sink : edge->source;
		if (neighbor == vertex1) {
			// If the neighbor is the other potential swap then we gain nothing from this edge
		} else if (vertex2_part == neighbor->partition) {
			// Moving an edge across the partition that wasn't there before loses
			gain -= edge->weight;
		} else {
			// Moving an edge from the partition into a partition wins
			gain += edge->weight;
		}
	}
	return gain;
}
