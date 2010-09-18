#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "import.h"
#include "graph.h"

void parse_header();
void parse_body();

// Stream for reading from file
FILE *in;
// Last read line
char *line;
// Number of bytes malloced for line (see GNU getline)
size_t num_bytes = 20;

Hypergraph *hypergraph;

Hypergraph* import_graph(char *file_name) {
	// Read in entire file line by line
	in = fopen(file_name, "r+");
	if(in == NULL) {
		printf("** Couldn't open netd file\n");
		throw;
	}
	line = (char *) malloc(num_bytes+1);
	hypergraph = new Hypergraph();
	parse_header();
	parse_body();
	free(line);
	fclose(in);
	return hypergraph;
}

void parse_header() {
	/* Each netlist header has five entries which are 
	 * -ignored 
	 * -num pins
	 * -num nets
	 * -num modules
	 * -pad offset
	 */

	// Ignore first line
	getline(&line, &num_bytes, in);

	getline(&line, &num_bytes, in);
	hypergraph->num_pins = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph->num_nets = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph->num_modules = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph->pad_offset = atoi(line);
}

void parse_body() {
	// Store all vertices in a single array so we don't create anything twice
	Vertex *vertices[hypergraph->num_modules]; 
	// Initialize them to NULL so they aren't random memory values
	memset(vertices, 0, hypergraph->num_modules*sizeof(Vertex*));
	// This will store the net we're currently constructing
	Hyperedge *hyperedge = NULL;
	// Read in the first line, this should be the start of the first net
	ssize_t read_bytes = getline(&line, &num_bytes, in);
	char start, direction;
	char *name = (char *) malloc(20);
	// Stop if we reach EOF (read_bytes == -1)
   	while (read_bytes >= 0) {
		/* netD net details:
		 * module_name  start(s?) out/in/bi
		 * (p#|a#)      (s|l)     (O/I/B)
		 */
		sscanf(line, "%s %1c %1c", name, &start, &direction);
		
		// Pull the number off the tail end of the name
		int num = atoi(name+1);
		if (name[0] == 'p') {
			// This is a pad and is labeled from (0,num_modules-pad_offset-1)
			// or is a cell and is labeled from (0,pad_offset) inclusive
			num += hypergraph->pad_offset;
		} else if (name[0] != 'a') {
			printf("** Parse error on netd file\n");
			throw;
		}

		if (vertices[num] == NULL) {
			vertices[num] = newVertex(name);
		}

		// Check if we're starting a new net
		if (start == 's') {
			if(hyperedge != NULL) {
				// Finish the previous net by adding it to the graph and then start a new one
				addNet(hypergraph, hyperedge);
			}
			hyperedge = newHyperedge(vertices[num]);
		} else {
			// We need to add the current node to the current net
			addVertex(hyperedge, vertices[num]);
		}
		// Read the next line
		read_bytes = getline(&line, &num_bytes, in);
	}
	// We have one final net left to add
	addNet(hypergraph, hyperedge);
	free(name);
}
