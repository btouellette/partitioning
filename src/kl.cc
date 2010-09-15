#include "graph.h"

void initial_partition();
int pass();

void kl(int num_runs) {
	for(int i = 0; i < num_runs; i++) {
		initial_partition();
		while(pass() != 0) {
		}
	}
}

void initial_partition() {
}

int pass() {
	int max_gain = 0;
	return 0;
}
