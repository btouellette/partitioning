#ifndef PART_SLICING_H_
#define PART_SLICING_H_

typedef unsigned char slicing_string;
typedef slicing_string slicing_index;

#define MAX_BLOCKS 151

#define V 255
#define H 254

// Relative probabilities (integers)
#define M1_PROB 1
#define M2_PROB 1
#define M3_PROB 1
#define M4_PROB 1

typedef struct {
	slicing_index index;
	char move_type;
} move;

typedef struct {
	int width, height;
} block;

#define INITIAL_TEMPERATURE 1
#define COOLING_STEPS 800 /* higher improves quality, lower speeds up */
#define COOLING_FRACTION 0.98 /* same */
#define STEPS_PER_TEMP 1200 /* same */
#define E 2.718
#define K 0.01

#define swap(x,y, type) \
	do { \
		type tmp = (x); \
		(x) = (y); \
		(y) = tmp; \
		} while(0)
#define TRUE  1
#define FALSE 0

#endif // PART_SLICING_H_
