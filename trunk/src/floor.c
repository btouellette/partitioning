#define _GNU_SOURCE
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "floor.h"

/*
 Simulated-Annealing()
	Create initial solution S
	Initialize temperature t
	repeat
	for i = 1 to iteration-length do
	Generate a random transition from S to Si
	If (C(S) ≥ C(Si )) then S = Si
	else if (e(C(S)−C(Si ))/(k·t) > random[0, 1)) then S = Si
	Reduce temperature t
	until (no change in C(S))
	Return S
*/

block *blocks[MAX_BLOCKS];
FILE *input_ckt, *output_file;
int debug_level;

slicing_string* initial_solution() {
	slicing_string *s = (slicing_string*)malloc(2*MAX_BLOCKS-1);
	int operands = 0, operators = 0;
	int num_blocks = 0;
	int block_used[MAX_BLOCKS];
	// Init all blocks to unused
	for(int i = 0; i < MAX_BLOCKS; i++) {
		if(blocks[i] != NULL) {
			num_blocks++;
			block_used[i] = 0;
		} else {
			// Mark nonexistent blocks as used
			block_used[i] = 1;
		}
	}
	int i = 0;
	while(operands < num_blocks) {
		// Choose whether to try and add an operator or an operand
		if(operands < (operators+2) || rand()%2 == 0) {
			// Add operand
			int select = rand()%MAX_BLOCKS;
			while(block_used[select] == 1) {
				select = rand()%MAX_BLOCKS;
			}
			s[i] = select;
			block_used[select] = 1;
			operands++;
		} else {
			// Add operator
			if(rand()%2 == 0) {
				s[i] = V;
			} else {
				s[i] = H;
			}
			operators++;
		}
		i++;
	}
	// Add null terminator to string
	s[i] = 0;
	if(debug_level > 0) {
		printf("Initial Solution:\n");
		i = 0;
		while(s[i] != 0) {
			if(s[i] == V) {
				printf("V");
			} else if(s[i] == H) {
				printf("H");
			} else {
				printf("%d", s[i]);
			}
			if(s[i+1] != 0) {
				printf("-");
			}
			i++;
		}
		printf("\n");
	}
	return s;
}

double cost_recursive_width(slicing_string *s) {
	slicing_string node = *s;
	// If we are at a terminal return the width
	if(node != V && node != H) {
		return blocks[node]->width;
	}
	double width1, width2;
	width1 = cost_recursive_width(s-1);
	// Size of the subtree to the left/top
	int size = 1;
	while(size > 0) {
		// Move left
		s--;
		if(*s != V && *s != H) {
			// Subtract for every terminal
			size--;
		} else {
			// Add two for every cut (and subtract one for the cut value)
			size += 1;
		}
	}
	// Now at the last element in the first subtree
	s--;
	width2 = cost_recursive_width(s);
	// For a vertical cut sum the widths
	if(node == V) {
		return width1 + width2;
	}
	// For a horizontal cut take the max
	return (width1 > width2) ? width1 : width2;
}

double cost_recursive_height(slicing_string *s) {
	slicing_string node = *s;
	// If we are at a terminal return the height
	if(node != V && node != H) {
		return blocks[node]->height;
	}
	double height1, height2;
	height1 = cost_recursive_height(s-1);
	// Size of the subtree to the left/top
	int size = 1;
	while(size > 0) {
		// Move left
		s--;
		if(*s != V && *s != H) {
			// Subtract for every terminal
			size--;
		} else {
			// Add two for every cut (and subtract one for the cut value)
			size += 1;
		}
	}
	// Now at the last element in the first subtree
	s--;
	height2 = cost_recursive_height(s);
	// For a horizontal cut sum the heights
	if(node == H) {
		return height1 + height2;
	}
	// For a vertical cut take the max
	return (height1 > height2) ? height1 : height2;
}

double cost(slicing_string *s) {
	// Move the pointer to the terminator of the string
	while(*(++s) != 0) ;
	// Move the string back to the final element
	s--;
	return cost_recursive_width(s) * cost_recursive_height(s);
}

move* random_move(slicing_string *s) {
	// M1 (Operand Swap): Swap two adjacent operands.
	// M2 (Chain Invert): Complement some chain (V = H, H = V ).
	// M3 (Operator/Operand Swap): Swap two adjacent operand and operator.
	// M4 (Rotation of Operand)

	move *new_move = (move *)malloc(sizeof(move));
	
	// Select move
	int selection = rand()%(M1_PROB+M2_PROB+M3_PROB+M4_PROB);
	if(selection < M1_PROB) {
		new_move->move_type = 1;
	} else if(selection < M2_PROB) {
		new_move->move_type = 2;
	} else if(selection < M3_PROB) {
		new_move->move_type = 3;
	} else {
		new_move->move_type = 4;
	}

	// Select index
	if(new_move->move_type == 1) {
		int choices = 0;
		int i = 0;
		while(s[i] != 0) {
			// Verify this and the next are both operands
			if(s[i] != V && s[i] != H && s[i+1] != V && s[i+1] != H && s[i+1] != 0) {
				choices++;
			}
			i++;
		}
		int choice = rand()%choices;
		i = -1;
		while(choice >= 0) {
			i++;
			if(s[i] != V && s[i] != H && s[i+1] != V && s[i+1] != H && s[i+1] != 0) {
				choice--;
			}
		}
		new_move->index = i;
	} else if(new_move->move_type == 2) {
		int choices = 0;
		int i = 1;
		while(s[i] != 0) {
			// Identify the start of a chain
			if((s[i] == V || s[i] == H) && s[i-1] != V && s[i-1] != H) {
				choices++;
			}
			i++;
		}
		int choice = rand()%choices;
		i = 0;
		while(choice >= 0) {
			i++;
			if((s[i] == V || s[i] == H) && s[i-1] != V && s[i-1] != H) {
				choice--;
			}
		}
		new_move->index = i;
	} else if(new_move->move_type == 3) {
		int choices = 0;
		int i = 1;
		while(s[i] != 0) {
			// Verify this and the next are opposite types
			if((((s[i] != V && s[i] != H) && (s[i+1] == V || s[i+1] == H)) ||
			    ((s[i] == V || s[i] == H) && (s[i+1] != V && s[i+1] != H))) &&
			   s[i+1] != 0) {
				choices++;
			}
			i++;
		}
		int choice = rand()%choices;
		i = 0;
		while(choice >= 0) {
			i++;
			if((((s[i] != V && s[i] != H) && (s[i+1] == V || s[i+1] == H)) ||
			    ((s[i] == V || s[i] == H) && (s[i+1] != V && s[i+1] != H))) &&
			   s[i+1] != 0) {
				choice--;
			}
		}
		new_move->index = i;
	} else {
		int choices = 0;
		int i = 0;
		while(s[i] != 0) {
			// Verifty this is an operand
			if(s[i] != V && s[i] != H && s[i] != 0) {
				choices++;
			}
			i++;
		}
		int choice = rand()%choices;
		i = 0;
		while(choice >= 0) {
			i++;
			if(s[i] != V && s[i] != H && s[i] != 0) {
				choices--;
			}
		}
		new_move->index = i;
	}
	return new_move;
}

int satisfies_balloting(slicing_string *s) {
	// (the balloting property) for every subexpression Ei = e1 . . . ei, 
	// 1 ≤ i ≤ 2n − 1, #operands > #operators.
	// Must check after M3 in case this is broken
	int operands = 0, operators = 0;
	while(*s != 0) {
		if(*s == V || *s == H) {
		   operators++;
		} else {
			operands++;
		}	 
		if(operands <= operators) {
			return FALSE;
		}
		s++;
	}
	return TRUE;
}

void perform_move(move *m, slicing_string *s) {
	// M1 (Operand Swap): Swap two adjacent operands.
	// M2 (Chain Invert): Complement some chain (V = H, H = V ).
	// M3 (Operator/Operand Swap): Swap two adjacent operand and operator.
	// M4 (Rotation of Operand)
	switch(m->move_type) {
		case 1:
			swap(s[m->index], s[m->index+1], slicing_string);
			break;
		case 2:
		{
			int index = m->index;
			while(s[index] == V || s[index] == H) {
				if(s[index] == V) {
					s[index] = H;
				} else {
					s[index] = V;
				}
				index++;
			}
			break;
		}
		case 3:
			swap(s[m->index], s[m->index+1], slicing_string);
			if(satisfies_balloting(s) == FALSE) {
				swap(s[m->index], s[m->index+1], slicing_string);
			}
			break;
		case 4:
			swap(blocks[s[m->index]]->width, blocks[s[m->index]]->height, int);
			break;
		default:
			printf("Illegal situation @%s:%d", __FILE__, __LINE__);
			break;
	}
}

void reverse_move(move *m, slicing_string *s) {
	// Moves are all symmetric
	perform_move(m, s);
}

slicing_string* anneal() {
	double current_value;
	double start_value;
	double temperature;

	slicing_string *s;
	
	s = initial_solution();
	current_value = cost(s);
	temperature = INITIAL_TEMPERATURE;	

	for(int i = 1; i <= COOLING_STEPS; i++) {
		temperature *= COOLING_FRACTION;
		start_value = current_value;
		for(int j = 1; j <= STEPS_PER_TEMP; j++) {
			/* Pick move */
			move *m = random_move(s);
			double current_area = cost(s);
			perform_move(m, s);
			double delta = cost(s) - current_area;
			double flip = (double)RAND_MAX / (double)rand(); 
			double exponent = (-delta/current_value)/(K*temperature);
			double merit = pow(E, exponent);
			if(delta < 0 || merit > flip) { 
				current_value += delta;
			} else {
				reverse_move(m, s);
			}
			free(m);
		}
		if(current_value - start_value < 0.0) {
			temperature /= COOLING_FRACTION;
		}
	}
	return s;
}

void repeated_annealing(int num_runs) {
	slicing_string *best_string = initial_solution();
	double best_cost = cost(best_string);
	for(int i = 1; i <= num_runs; i++) {
		slicing_string *current_string = anneal();
		double current_cost = cost(current_string);
		if(current_cost < best_cost) {
			best_cost = current_cost;
			free(best_string);
			best_string = current_string;
		} else {
			free(current_string);
		}
	}
	/* File format:
	 * slicing string (0-1-2-V-H)
	 * width height (block 0)
	 * width height (block 1)
	 * ....
	 */
	slicing_string *s = best_string;
	while(*s != 0) {
		if(*s == V) {
			fprintf(output_file, "V");
		} else if(*s == H) {
			fprintf(output_file, "H");
		} else {
			fprintf(output_file, "%d", *s);
		}
		s++;
		if(*s != 0) {
			fprintf(output_file, "-");
		}
	}
	fprintf(output_file, "\n");
	int i = 0;
	while(i < MAX_BLOCKS && blocks[i] != NULL) {
		fprintf(output_file, "%d %d\n", blocks[i]->width, blocks[i]->height);
	}
	fclose(output_file);
	free(best_string);
}

void import_blocks(FILE *in) {
	if(debug_level > 0) {
		printf("Importing...\n");
	}
	// NULL initialize the blocks array
	for(int i = 0; i < MAX_BLOCKS; i++) {
		blocks[i] = NULL;
	}
	/* File format:
	 * #_of_blocks
	 * block1_width block1_height
	 * block2_width block2_height
	 * etc...
	 */
	if(in == NULL) {
		printf("** Couldn't open block file\n");
		exit(1);
	}
	size_t num_bytes = 20;
	char *line = (char *) malloc(num_bytes+1);
	// Ignore #_of_blocks
	getline(&line, &num_bytes, in);
	int num_operands = 0;
	// Stop if we reach EOF (read_bytes == -1)
	while(getline(&line, &num_bytes, in) >= 0) {
		int width, height;
		if(sscanf(line, "%d %d", &width, &height) != 2) {
			printf("** Invalid file");
			exit(1);
		}
		blocks[num_operands] = (block *)malloc(sizeof(block));
		blocks[num_operands]->width = width;
		blocks[num_operands]->height = height;
		if(debug_level > 1) {
			printf("Added block %d with w: %d and h: %d\n", num_operands, width, height);
		}
		num_operands++;
	}
	free(line);
	fclose(in);
	return;
}

void print_usage() {
	printf("** Usage is: -r #_of_runs -i input_ckt -o output_file -d debug_level\n");
	printf("** All parameters but input_ckt are optional\n");
	printf("** debug_level can be 0, 1, 2\n");
}

int main(int argc, char *argv[]) {
	int r = 1;
	input_ckt = NULL;
	output_file = stdout;
	debug_level = 0;

	if(argc < 3 || argc%2 == 0) {
		print_usage();
		return 1;
	}

	for(int i = 1; i < argc; i++) {
		if(strcmp(argv[i], "-r") == 0) {
			r = atoi(argv[++i]);
		} else if(strcmp(argv[i], "-i") == 0) {
			input_ckt = fopen(argv[++i], "r");
		} else if(strcmp(argv[i], "-o") == 0) {
			output_file = fopen(argv[++i], "w");
		} else if(strcmp(argv[i], "-d") == 0) {
			debug_level = atoi(argv[++i]);
		}
	}

	srand(time(NULL));
	import_blocks(input_ckt);
	repeated_annealing(r);

	for(int i = 0; i < MAX_BLOCKS; i++) {
		if(blocks[i] != NULL) {
			free(blocks[i]);
		}
	}

	return 0;
}
