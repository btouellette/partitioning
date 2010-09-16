#include <stdio.h>
#include <stdlib.h>

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

Hypergraph hypergraph;

void import_graph(char *file_name) {
	// Read in entire file line by line
	in = fopen(file_name, "r");
	line = (char *) malloc(num_bytes+1);
	parse_header();
	parse_body();
	free(line);
	fclose(in);
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
	hypergraph.num_pins = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph.num_nets = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph.num_modules = atoi(line);

	getline(&line, &num_bytes, in);
	hypergraph.pad_offset = atoi(line);
}

void parse_body() {
	// Store all vertices in a single array so we don't create anything twice
	Vertex *vertices[hypergraph.num_modules]; 
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
		if (name[0] == 'p') {
			// This is a pad and is labeled from (0,num_modules-pad_offset-1)
		} else if (name[0] == 'a') {
			// This is a cell and is labeled from (0,pad_offset)
			int num = atoi(name+1);
			if(vertices[num] == NULL) {
				vertices[num] = new Vertex();
			}
		} else {
			printf("** Parse error on netd file");
			throw;
		}
		read_bytes = getline(&line, &num_bytes, in);
	}
	free(name);
}
