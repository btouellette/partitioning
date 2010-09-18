#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "import.h"
#include "kl.h"
#include "fm.h"

int main(int argc, char *argv[]) {
	// Correctly formatted input will have exactly 7 inputs
	if (argc != 7) {
		printf("** Usage is -in <infile> -alg <kl/fm> -runs <num runs>\n");
		return 1;
	}

	int num_runs = 0;
	char *algorithm = NULL, *file = NULL;
	// Run through all the inputs on the command line
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-in") == 0) {
			// Import the entire file to the graph data structure
			file = argv[++i];
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
			printf("** Usage is -in <infile> -alg <kl/fm> -runs <num runs>\n");
			return 1;
		}
	}

	// Import the entire file to the graph data structure
	Hypergraph *hypergraph = import_graph(file);
	//std::cout << *hypergraph << std::endl;

	// Call the appropriate starter function for the algorithm specified
	if (strcmp(algorithm, "kl") == 0) {
		kl(num_runs);
	} else if (strcmp(algorithm, "fm") == 0) {
		fm(num_runs);
	} else {
		printf("** Invalid algorithm. Algorithms supported: kl fm\n");
	}

	return 0;
}
