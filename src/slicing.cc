#include <math.h>
#include <stdlib.h>

#include "slicing.h"

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

slicing_string* initial_solution() {
	slicing_string *s;
	return s;
}

double cost(slicing_string *s) {
	return 0.0;
}

move random_move() {
	move new_move;
	return new_move;
}

void perform_move(move m, slicing_string *s) {
}

void reverse_move(move m, slicing_string *s) {
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
			move m = random_move();
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
		}
		if(current_value - start_value < 0.0) {
			temperature /= COOLING_FRACTION;
		}
	}
	return s;
}

void repeated_annealing(int num_runs) {
	double best_cost;
	best_cost = cost(initial_solution());
	for(int i = 1; i <= num_runs; i++) {
		slicing_string *current_string = anneal();
		double current_cost = cost(current_string);
		if(current_cost < best_cost) {
			best_cost = current_cost;
		}
	}
}
