#include "graph.h"

static void initial_partition() {
}

static int pass() {
	return 0;
}

void fm(int num_runs) {
	for(int i = 0; i < num_runs; i++) {
		initial_partition();
		while(pass() != 0) {
		}
	}
}
