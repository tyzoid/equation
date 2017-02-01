#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include "functions.h"

// Condition Checking
static int eq_conditions(int cond, char *error)
{
	int exit_status = 1;

	if (cond)
	{
		fprintf(stderr, "%s\n", error);
		if (exit_status) exit(exit_status);
	}

	return cond;
}

// Mathematical Functions
double eqfunc_add(size_t count, double *args)
{
	double sum = 0;
	for (size_t i = 0; i < count; i++)
	{
		sum += args[i];
	}

	return sum;
}

double eqfunc_sub(size_t count, double *args)
{
	eq_conditions(count < 1, "Subtract requires at least one argument");

	double difference = args[0];
	for (size_t i = 1; i < count; i++)
	{
		difference -= args[i];
	}

	return difference;
}

double eqfunc_mul(size_t count, double *args)
{
	double product = 1;
	for (size_t i = 0; i < count; i++)
	{
		product *= args[i];
	}

	return product;
}

double eqfunc_div(size_t count, double *args)
{
	eq_conditions(count < 1, "Divide requires at least one argument");

	double dividand = args[0];
	for (size_t i = 1; i < count; i++)
	{
		dividand /= args[i];
	}

	return dividand;
}

double eqfunc_exp(size_t count, double *args)
{
	eq_conditions(count != 1, "Exponentiate requires one argument");

	return exp(args[0]);
}

double eqfunc_pow(size_t count, double *args)
{
	eq_conditions(count != 2, "Power requires two arguments");

	return pow(args[0], args[1]);
}

// Unary Functions
double eqfunc_abs(size_t count, double *args)
{
	eq_conditions(count != 1, "Absolute value than requires exactly one argument");

	return fabs(args[0]);
}

// Comparison Functions
double eqfunc_lt(size_t count, double *args)
{
	eq_conditions(count < 2, "Less than requires at least two arguments");

	for (size_t i = 1; i < count; i++)
	{
		if (args[i] <= args[i-1]) return 0;
	}

	return 1;
}

double eqfunc_lte(size_t count, double *args)
{
	eq_conditions(count < 2, "Less or equal than requires at least two arguments");

	for (size_t i = 1; i < count; i++)
	{
		if (args[i] < args[i-1]) return 0;
	}

	return 1;
}

double eqfunc_gt(size_t count, double *args)
{
	eq_conditions(count < 2, "Greater than requires at least two arguments");

	for (size_t i = 1; i < count; i++)
	{
		if (args[i] >= args[i-1]) return 0;
	}

	return 1;
}

double eqfunc_gte(size_t count, double *args)
{
	eq_conditions(count < 2, "Greater or equal than requires at least two arguments");

	for (size_t i = 1; i < count; i++)
	{
		if (args[i] > args[i-1]) return 0;
	}

	return 1;
}

double eqfunc_eq(size_t count, double *args)
{
	eq_conditions(count < 2, "Equals requires at least two arguments");
	eq_conditions(count > 3, "Equals accepts at most three arguments");

	double tolerance = 0.0000001;

	if (count == 3)
	{
		tolerance = args[2];
	}

	if (fabs(args[0] - args[1]) < tolerance) return 1;

	return 0;
}

// Logical Functions
double eqfunc_and(size_t count, double *args)
{
	for (size_t i = 0; i < count; i++)
	{
		if (fabs(args[i]) < .5) return 0;
	}

	return 1;
}

double eqfunc_or(size_t count, double *args)
{
	for (size_t i = 0; i < count; i++)
	{
		if (fabs(args[i]) >= .5) return 1;
	}

	return 0;
}

double eqfunc_not(size_t count, double *args)
{
	eq_conditions(count != 1, "Not requires one argument");

	return exp(args[0]);
}

double eqfunc_xor(size_t count, double *args)
{
	eq_conditions(count != 2, "XOR requires one argument");

	double or = eqfunc_or(count, args);
	double and = eqfunc_and(count, args);

	return (or > .5 && and < .5);
}

double eqfunc_if(size_t count, double *args)
{
	eq_conditions(count != 3, "If requires three arguments");

	if (fabs(args[0]) < .5) return args[2];
	return args[1];
}

