#include <iostream>

#include <limits.h>
#include <string.h>
#include <stdlib.h>

#include "import.h"
#include "kl.h"
#include "fm.h"

using namespace std;

int main(int argc, char *argv[]) {
	// Correctly formatted input will have exactly 7 inputs
	if (argc != 7) {
		cout << "Usage is -in <infile> -alg <kl/fm> -runs <num runs>" << endl;
		return 1;
	}

	int num_runs;
	char *algorithm;
	// Run through all the inputs on the command line
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-in")) {
			// Import the entire file to the graph data structure
			import_graph(argv[++i]);
		} else if (strcmp(argv[i], "-alg")) {
			// Store the algorithm type
			algorithm = argv[++i];
		} else if (strcmp(argv[i], "-runs")) {
			// Convert the number of runs to do to an integer and store
			num_runs = atoi(argv[++i]);
			if(num_runs <= 0 || num_runs == INT_MAX) {
				// Either the number was invalid, negative, or outside our representable range 
				cout << "Usage is -in <infile> -alg <kl/fm> -runs <num runs>" << endl;
				return 1;
			}
		} else {
			// Unknown input arguments detected. Report usage and exit abnormally
			cout << "Usage is -in <infile> -alg <kl/fm> -runs <num runs>" << endl;
			return 1;
		}
	}

	// Call the appropriate starter function for the algorithm specified
	if (strcmp(algorithm, "kl")) {
		kl(num_runs);
	} else if (strcmp(algorithm, "fm")) {
		fm(num_runs);
	} else {
		cout << "Algorithms supported: kl fm" << endl;
	}

	return 0;
}
