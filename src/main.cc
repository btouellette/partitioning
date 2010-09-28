#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "import.h"
#include "kl.h"
#include "fm.h"

void printUsage() {
		printf("** Usage is -in <infile> {-adj <outfile>} {-alg <kl/fm> -runs <num runs>}\n");
		printf("** adj - write the adjacency matrix for this hypergraph out to a file\n");
		printf("** kl - Kernighan-Lin partitioning\n");
		printf("** fm - Fiduccia-Mattheyses partitioning\n");
}

int main(int argc, char *argv[]) {
	if (argc != 9 && argc != 7 && argc != 5) {
		printUsage();
		return 1;
	}

	int num_runs = 0;
	char *algorithm = NULL, *in_file = NULL, *out_file = NULL;
	// Run through all the inputs on the command line
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-adj") == 0) {
			out_file = argv[++i];
		} else if (strcmp(argv[i], "-in") == 0) {
			// Import the entire file to the graph data structure
			in_file = argv[++i];
		} else if (strcmp(argv[i], "-alg") == 0) {
			// Store the algorithm type
			algorithm = argv[++i];
		} else if (strcmp(argv[i], "-runs") == 0) {
			// Convert the number of runs to do to an integer and store
			num_runs = atoi(argv[++i]);
			if(num_runs <= 0 || num_runs == INT_MAX) {
				// Either the number was invalid, negative, or outside our representable range 
				printf("** Number of runs must be >0\n");
				return 1;
			}
		} else {
			// Unknown input arguments detected. Report usage and exit abnormally
			printUsage();
			return 1;
		}
	}

	// Import the entire file to the graph data structure
	Hypergraph *hypergraph = import_graph(in_file);

	if (out_file != NULL) {
		Graph *graph = convertToGraph(hypergraph);
		writeAdjacencyMatrixToDisk(graph, out_file);
	}

	// Call the appropriate starter function for the algorithm specified
	if (algorithm != NULL) {
		if (strcmp(algorithm, "kl") == 0) {
			Graph *graph = convertToGraph(hypergraph);
			kl(graph, num_runs);
		} else if (strcmp(algorithm, "fm") == 0) {
			fm(num_runs);
		} else {
			printf("** Invalid algorithm.\n");
			printUsage();
		}
	}

	return 0;
}
