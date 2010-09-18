#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include "graph.h"

using namespace std;

void initial_partition();
int pass();

set<Vertex*> part_1, part_2;
set<Vertex*> uncalculated_1, uncalculated_2, unlocked_1, unlocked_2;
map<Vertex*, bool> locked;
Graph* graph;

void kl(Graph* in_graph, int num_runs) {
	graph = in_graph;
	for (int i = 0; i < num_runs; i++) {
		initial_partition();
		while (pass() != 0) {
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
		} else {
			part_2.insert(*it);
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
			vec_it++;
			size_2--;
			size_1++;
		}
	}
}

int pass() {
	int max_gain = 0;
	return 0;
}
