#include <stdio.h>
#include <stdlib.h>

#include "import.h"
#include "graph.h"

void parse_header();
void parse_body();
void parse_net();

// Stream for reading from file
FILE *in;
// Last read line
char *line;
// Number of bytes malloced for line (see GNU getline)
size_t num_bytes = 20;

void import_graph(char *file_name) {
	// Read in entire file line by line
	in = fopen(file_name, "r");
	line = (char *) malloc(num_bytes+1);
	parse_header();
	parse_body();
	free(line);
	fclose(in);
}

int num_pins, num_nets, num_modules, pad_offset;

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
	num_pins = atoi(line);

	getline(&line, &num_bytes, in);
	num_nets = atoi(line);

	getline(&line, &num_bytes, in);
	num_modules = atoi(line);

	getline(&line, &num_bytes, in);
	pad_offset = atoi(line);
}

void parse_body() {
}

void parse_net() {
}
