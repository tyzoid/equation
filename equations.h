#ifndef _EQUATIONS_H_
#define _EQUATIONS_H_

#include <inttypes.h>
#include <stdio.h>
#include <ctype.h>

#define PARAMETER_TYPE_VARIABLE 1
#define PARAMETER_TYPE_CONSTANT 2
#define PARAMETER_TYPE_FUNCTION 3

typedef double (*eqfunc)(size_t, double *);

typedef union _equation_parameter_t {
	size_t variable_index;
	size_t step;
	double constant;
} EquationParameter;

typedef struct _equation_step_t {
	eqfunc function;
	EquationParameter *params;
	uint8_t *parameter_types;
	double *parameter_values;
	size_t parameter_count;
} EquationStep;

typedef struct _equation_t {
	EquationStep *steps;
	double *variables;
	size_t variable_count;
	size_t step_count;
} Equation;

Equation *parseEquation(const char*);
void freeEquation(Equation*);
double executeEquation(Equation*, double*, size_t);

#endif
