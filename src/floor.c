#define _GNU_SOURCE
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "floor.h"

block *blocks[MAX_BLOCKS];
block *best_blocks[MAX_BLOCKS];
FILE *input_ckt, *output_file;
int debug_level;
int accepted_moves, rejected_moves;

slicing_string* initial_solution() {
	if(debug_level > 1) {
		printf("Generating initial solution...\n");
	}
	// Grab enough space for our biggest possible slicing tree
	slicing_string *s = (slicing_string*)malloc((2*MAX_BLOCKS-1)*sizeof(slicing_string));
	int operands = 0, operators = 0;
	int num_blocks = 0;
	int block_used[MAX_BLOCKS];
	// Mark all blocks as used or unused (for the string)
	for(int i = 0; i < MAX_BLOCKS; i++) {
		if(blocks[i] != NULL) {
			num_blocks++;
			block_used[i] = 0;
		} else {
			// Mark nonexistent blocks as used
			block_used[i] = 1;
		}
	}
	if(debug_level > 2) {
		printf(" %d total blocks\n", num_blocks);
	}
	int i = 0;
	while(operands < num_blocks) {
		// Choose whether to try and add an operator or an operand
		if(operands < (operators+2) || rand()%2 == 0) {
			// Add operand selected randomly from remaining operands
			int select = rand()%MAX_BLOCKS;
			while(block_used[select] == 1) {
				select = rand()%MAX_BLOCKS;
			}
			s[i] = select;
			block_used[select] = 1;
			if(debug_level > 2) {
				printf("  Added %d at position %d\n", select, i);
			}
			operands++;
		} else {
			// Add operator
			// Must satisfy slicing requirement of not having same cuts next to each other
			if(s[i-1] == H) {
				s[i] = V;
				if(debug_level > 2) {
					printf("  Added V at position %d\n", i);
				}
			} else if(s[i-1] == V) {
				s[i] = H;
				if(debug_level > 2) {
					printf("  Added H at position %d\n", i);
				}
			} else if(rand()%2 == 0) {
				s[i] = V;
				if(debug_level > 2) {
					printf("  Added V at position %d\n", i);
				}
			} else {
				s[i] = H;
				if(debug_level > 2) {
					printf("  Added H at position %d\n", i);
				}
			}
			operators++;
		}
		i++;
	}
	// Finished assigning operands
	// Add operators until the tree is valid
	while(operators + operands < 2*num_blocks - 1) {
		if(s[i-1] == H) {
			s[i] = V;
			if(debug_level > 2) {
				printf("  Added V at position %d\n", i);
			}
		} else if(s[i-1] == V) {
			s[i] = H;
			if(debug_level > 2) {
				printf("  Added H at position %d\n", i);
			}
		} else if(rand()%2 == 0) {
			s[i] = V;
			if(debug_level > 2) {
				printf("  Added V at position %d\n", i);
			}
		} else {
			s[i] = H;
			if(debug_level > 2) {
				printf("  Added H at position %d\n", i);
			}
		}
		operators++;
		i++;
	}
	// Add null terminator to string
	s[i] = 0;
	if(debug_level > 1) {
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

double cost_width(slicing_string *s) {
	slicing_string node = *s;
	// If we are at a terminal return the width
	if(node != V && node != H) {
		return blocks[node]->width;
	}
	double width1, width2;
	width1 = cost_width(s-1);
	// Size of the subtree to the right/top
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
	width2 = cost_width(s);
	// For a vertical cut sum the widths
	if(node == V) {
		return width1 + width2;
	}
	// For a horizontal cut take the max
	return (width1 > width2) ? width1 : width2;
}

double cost_height(slicing_string *s) {
	slicing_string node = *s;
	// If we are at a terminal return the height
	if(node != V && node != H) {
		return blocks[node]->height;
	}
	double height1, height2;
	height1 = cost_height(s-1);
	// Size of the subtree to the right/top
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
	height2 = cost_height(s);
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
	return cost_width(s) * cost_height(s);
}

move* random_move(slicing_string *s) {
	if(debug_level > 2) {
		printf(" Making random move...\n");
	}

	move *new_move = (move*)malloc(sizeof(move));
	
	// Select move randomly
	int selection = rand()%(M1_PROB+M2_PROB+M3_PROB+M4_PROB);
	if(selection < M1_PROB) {
		new_move->move_type = 1;
	} else if(selection < M1_PROB + M2_PROB) {
		new_move->move_type = 2;
	} else if(selection < M1_PROB + M2_PROB + M3_PROB) {
		new_move->move_type = 3;
	} else {
		new_move->move_type = 4;
	}

	if(debug_level > 2) {
		printf("  Selected M%d\n", new_move->move_type);
	}

	// Select index
	switch(new_move->move_type) {
		case 1:
		{
			// M1 (Operand Swap): Swap two adjacent operands.
			int choices = 0;
			int i = 0;
			while(s[i] != 0) {
				// Verify this and the next are both operands
				if(s[i] != V && s[i] != H && s[i+1] != V && s[i+1] != H && s[i+1] != 0) {
					choices++;
				}
				i++;
			}
			// Choose a random pair
			int choice = rand()%choices;
			// Find index of first element of pair
			i = -1;
			while(choice >= 0) {
				i++;
				if(s[i] != V && s[i] != H && s[i+1] != V && s[i+1] != H) {
					choice--;
				}
			}
			new_move->index = i;
			break;
		}
		case 2:
		{
			// M2 (Chain Invert): Complement some chain (V = H, H = V ).
			int choices = 0;
			int i = 1;
			while(s[i] != 0) {
				// Identify the start of a chain
				if((s[i] == V || s[i] == H) && s[i-1] != V && s[i-1] != H) {
					choices++;
				}
				i++;
			}
			// Pick random chain start
			int choice = rand()%choices;
			// Find index of first element in chain
			i = 0;
			while(choice >= 0) {
				i++;
				if((s[i] == V || s[i] == H) && s[i-1] != V && s[i-1] != H) {
					choice--;
				}
			}
			new_move->index = i;
			break;
		}
		case 3:
		{
			// M3 (Operator/Operand Swap): Swap two adjacent operand and operator.
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
			// Pick random pair to swap
			int choice = rand()%choices;
			// Find the index of the first elmement in the pair
			i = 0;
			while(choice >= 0) {
				i++;
				if(((s[i] != V && s[i] != H) && (s[i+1] == V || s[i+1] == H)) ||
				   ((s[i] == V || s[i] == H) && (s[i+1] != V && s[i+1] != H))) {
					choice--;
				}
			}
			new_move->index = i;
			break;
		}
		case 4:
		{
			// M4 (Rotation of Operand)
			int choices = 0;
			int i = 0;
			while(s[i] != 0) {
				// Verify this is an operand
				if(s[i] != V && s[i] != H) {
					choices++;
				}
				i++;
			}
			// Pick random operand
			int choice = rand()%choices;
			// Retrieve string index of operand
			i = -1;
			while(choice >= 0) {
				i++;
				if(s[i] != V && s[i] != H) {
					choice--;
				}
			}
			new_move->index = i;
			break;
		}
		default:
		{
			printf("Illegal situation @%s:%d", __FILE__, __LINE__);
			break;
		}
	}
	if(debug_level > 2) {
		printf("  Selected index %d\n", new_move->index);
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
	switch(m->move_type) {
		case 1:
		{
			// M1 (Operand Swap): Swap two adjacent operands.
			swap(s[m->index], s[m->index+1], slicing_string);
			break;
		}
		case 2:
		{
			// M2 (Chain Invert): Complement some chain (V = H, H = V ).
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
		{
			// M3 (Operator/Operand Swap): Swap two adjacent operand and operator.
			// Undo the move if we violated balloting
			swap(s[m->index], s[m->index+1], slicing_string);
			if(satisfies_balloting(s) == FALSE) {
				swap(s[m->index], s[m->index+1], slicing_string);
			}
			break;
		}
		case 4:
		{
			// M4 (Rotation of Operand)
			swap(blocks[s[m->index]]->width, blocks[s[m->index]]->height, int);
			break;
		}
		default:
		{
			printf("Illegal situation @%s:%d", __FILE__, __LINE__);
			break;
		}
	}
}

void reverse_move(move *m, slicing_string *s) {
	// Moves are all symmetric
	perform_move(m, s);
}

slicing_string* anneal() {
	/* Simulated-Annealing()
	 * Create initial solution S
	 * Initialize temperature t
	 * repeat:
	 *     for i = 1 to iteration-length do
	 *         Generate a random transition from S to Si
	 *         If (C(S) ≥ C(Si )) then S = Si
	 *         else if (e(C(S)−C(Si ))/(k·t) > random[0, 1)) then S = Si
	 *     Reduce temperature t
	 *     until (no change in C(S))
	 * Return S
	 */
	if(debug_level > 1) {
		printf("Annealing...\n");
	}
	double current_value;
	double start_value;
	double temperature;

	slicing_string *s;

	accepted_moves = 0;
	rejected_moves = 0;
	
	s = initial_solution();
	current_value = cost(s);
	temperature = INITIAL_TEMPERATURE;	

	for(int i = 1; i <= COOLING_STEPS; i++) {
		// Drop the temp
		temperature *= COOLING_FRACTION;
		start_value = current_value;
		for(int j = 1; j <= STEPS_PER_TEMP; j++) {
			// Pick move randomly and get cost change
			move *m = random_move(s);
			double current_area = cost(s);
			perform_move(m, s);
			double delta = cost(s) - current_area;
			// Get random double [0,1) to compare to
			double flip = (double)RAND_MAX / (double)rand(); 
			double exponent = (-delta/current_value)/(K*temperature);
			double merit = pow(E, exponent);
			if(delta < 0 || merit > flip) { 
				current_value += delta;
				accepted_moves++;
			} else {
				reverse_move(m, s);
				rejected_moves++;
			}
			free(m);
		}
		if(current_value - start_value < 0.0) {
			// Undo temp drop if we improved the cost
			temperature /= COOLING_FRACTION;
		}
	}
	if(debug_level > 1) {
		printf("Final Solution:\n");
		int i = 0;
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
		printf("Accepted Moves: %d\n", accepted_moves);
		printf("Rejected Moves: %d\n", rejected_moves);
	}
	return s;
}

void repeated_annealing(int num_runs) {
	if(debug_level > 0) {
		printf("Repeated annealing...\n");
	}
	// Generate a single random solution to compare annealing results to
	slicing_string *best_string = initial_solution();
	double best_cost = cost(best_string);
	if(debug_level > 0) {
		printf("Base cost: %d\n", (int)best_cost);
	}
	// Anneal repeatedly
	for(int i = 1; i <= num_runs; i++) {
		slicing_string *current_string = anneal();
		double current_cost = cost(current_string);
		if(debug_level > 0) {
			printf("Run cost: %d\n", (int)current_cost);
		}
		if(current_cost < best_cost) {
			// Found new best solution
			best_cost = current_cost;
			free(best_string);
			best_string = current_string;
			// Copy the blocks over to preserve orientation against future rotations
			int k = 1;
			while(k < MAX_BLOCKS && blocks[k] != NULL) {
				if(best_blocks[k] == NULL) {
					best_blocks[k] = (block*)malloc(sizeof(block));
				}
				best_blocks[k]->width = blocks[k]->width;
				best_blocks[k]->height = blocks[k]->height;
				k++;
			}
		} else {
			free(current_string);
		}
	}
	/* File format:
	 * slicing string (0-1-2-V-H)
	 * Blocks:
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
	if(debug_level > 0) {
		printf("Annealing best cost: %d from %d runs\n", (int)best_cost, num_runs);
	}
	fprintf(output_file, "Blocks:\n");
	// Print out the blocks to file (with correct orientation)
	int i = 1;
	while(i < MAX_BLOCKS && best_blocks[i] != NULL) {
		fprintf(output_file, "%d %d\n", best_blocks[i]->width, best_blocks[i]->height);
		i++;
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
		best_blocks[i] = NULL;
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
	char *line = (char*)malloc(num_bytes+1);
	// Ignore #_of_blocks
	getline(&line, &num_bytes, in);
	int i = 1;
	// Stop if we reach EOF (read_bytes == -1)
	while(getline(&line, &num_bytes, in) >= 0) {
		int width, height;
		if(sscanf(line, "%d %d", &width, &height) != 2) {
			printf("** Invalid file");
			exit(1);
		}
		blocks[i] = (block*)malloc(sizeof(block));
		blocks[i]->width = width;
		blocks[i]->height = height;
		if(debug_level > 2) {
			printf("  Added block %d with w: %d and h: %d\n", i, width, height);
		}
		i++;
	}
	free(line);
	fclose(in);
	return;
}

void print_usage() {
	printf("** Usage is: -r #_of_runs -i input_ckt -o output_file -d debug_level\n");
	printf("** All parameters but input_ckt are optional\n");
	printf("** debug_level can be 0-3\n");
}

int main(int argc, char *argv[]) {
	int r = 1;
	input_ckt = NULL;
	output_file = stdout;
	debug_level = 0;

	// Parse input parameters
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

	// Init random generator with system time
	srand(time(NULL));
	// Read in the input file
	import_blocks(input_ckt);
	
	// Compare annealing with Monte Carlo
	// Disable debug outputs temporarily
	int tmp = debug_level;
	debug_level = 0;
	slicing_string *s = initial_solution();
	double best_cost = cost(s);
	free(s);
	for(int i = 0; i < r*20; i++) {
		s = initial_solution();
		double new_cost = cost(s);
		free(s);
		best_cost = new_cost < best_cost ? new_cost : best_cost;
	}
	debug_level = tmp;
	if(debug_level > 0) {
		printf("Monte Carlo best cost: %d from %d runs\n", (int)best_cost,  20*r);
	}

	repeated_annealing(r);

	// Clean up
	for(int i = 0; i < MAX_BLOCKS; i++) {
		if(blocks[i] != NULL) {
			free(blocks[i]);
		}
		if(best_blocks[i] != NULL) {
			free(best_blocks[i]);
		}
	}

	return 0;
}
