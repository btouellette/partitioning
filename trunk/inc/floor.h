#ifndef PART_SLICING_H_
#define PART_SLICING_H_

typedef unsigned char slicing_string;
typedef slicing_string slicing_index;

#define V 255
#define H 254

typedef struct {
	slicing_index ind;
	char move_type;
} move;

typedef struct {
	int width, height;
} block;

#define INITIAL_TEMPERATURE 1
#define COOLING_STEPS 500
#define COOLING_FRACTION 0.97
#define STEPS_PER_TEMP 1000
#define E 2.718
#define K 0.01

#endif // PART_SLICING_H_
