#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "equations.h"
#include "functions.h"

static double executeEquationStep(Equation*, size_t);
static eqfunc parseFunctionName(const char*, const char**);
static int parseEquationStep(const char*, Equation*, size_t*, char**);

Equation *parseEquation(const char *str)
{
	size_t equation_steps = 0;
	size_t num_ending_parens = 0;

	// Simply count the number of parentheses to get the number of steps.
	for (const char *ptr = str; *ptr; ptr++)
	{
		if (*ptr == '(') equation_steps++;

		// Sanity Check
		if (*ptr == ')') num_ending_parens++;
	}

	if (num_ending_parens != equation_steps) return 0;

	// Allocate memory for the equation and steps.
	// Note, memory allocation for the parameter list is handled in parseEquationStep.
	Equation *eq = calloc(1, sizeof(Equation));
	eq->step_count = equation_steps;
	eq->steps = calloc(equation_steps, sizeof(EquationStep));


	size_t offset = 0;
	if (parseEquationStep(str, eq, &offset, 0))
	{
		return 0;
	}

	if (eq->variable_count > 0)
	{
		eq->variables = calloc(eq->variable_count, sizeof(double));
	}

	return eq;
}

static eqfunc parseFunctionName(const char *str, const char **end)
{
	*end = str;

	while (**end && **end != '(' && **end != ' ')
	{
		++*end;
	}

	size_t name_size = (size_t) (*end - str);

	if (strncmp(str, EQUATION_ADD_STR, name_size) == 0) return eqfunc_add;
	if (strncmp(str, EQUATION_SUB_STR, name_size) == 0) return eqfunc_sub;
	if (strncmp(str, EQUATION_MUL_STR, name_size) == 0) return eqfunc_mul;
	if (strncmp(str, EQUATION_DIV_STR, name_size) == 0) return eqfunc_div;
	if (strncmp(str, EQUATION_EXP_STR, name_size) == 0) return eqfunc_exp;
	if (strncmp(str, EQUATION_POW_STR, name_size) == 0) return eqfunc_pow;

	if (strncmp(str, EQUATION_ABS_STR, name_size) == 0) return eqfunc_abs;

	if (strncmp(str, EQUATION_LT_STR, name_size) == 0) return eqfunc_lt;
	if (strncmp(str, EQUATION_LTE_STR, name_size) == 0) return eqfunc_lte;
	if (strncmp(str, EQUATION_GT_STR, name_size) == 0) return eqfunc_gt;
	if (strncmp(str, EQUATION_GTE_STR, name_size) == 0) return eqfunc_gte;
	if (strncmp(str, EQUATION_EQ_STR, name_size) == 0) return eqfunc_eq;

	if (strncmp(str, EQUATION_AND_STR, name_size) == 0) return eqfunc_and;
	if (strncmp(str, EQUATION_OR_STR, name_size) == 0)  return eqfunc_or;
	if (strncmp(str, EQUATION_NOT_STR, name_size) == 0) return eqfunc_not;
	if (strncmp(str, EQUATION_XOR_STR, name_size) == 0) return eqfunc_xor;
	if (strncmp(str, EQUATION_IF_STR, name_size) == 0)  return eqfunc_if;

	return 0;
}

static int parseEquationStep(const char *str, Equation *eq, size_t *offset, char **endptr)
{
	EquationStep *steps = eq->steps;

	size_t self_offset = *offset;
	const char *next;

#ifdef _DEBUG_MODE
	printf("+ '%s'\n", str);
#endif

	steps[self_offset].function = parseFunctionName(str, &next);

	// Function not found
	if (!steps[self_offset].function)
	{
		fprintf(stderr, "Error: Specified function not found: '");
		for (const char *s = str; s != next+1; s++)
		{
			fputc(*s, stderr);
		}

		fprintf(stderr, "'\n");

		return 1;
	}

#ifdef _DEBUG_MODE
	printf("  - '%s'\n", next);
#endif

	// Skip spacing and parentheses.
	while (*next && (*next == '(' || *next == ' ')) next++;

	// Find the number of parameters. This is equal to the number of commas
	// within the outermost pair of parentheses.
	//
	// For example: "add(4, sub(3, 2), 3)"
	//    'add' has 3 parameters, since it has two commas contained exclusively
	//    outside all other pairs of parens.
	ssize_t paren_level = 0;
	size_t param_count = 1;

#ifdef _DEBUG_MODE
	printf("  - '%s'\n", next);
#endif

	for (const char *ptr = next; *ptr && paren_level >= 0; ptr++)
	{
		if (*ptr == '(') paren_level++;
		else if (*ptr == ')') paren_level--;
		else if (*ptr == ',' && !paren_level) param_count++;
	}

	// Allocate together to limit the number of calls to malloc made.
	// parameter_types MUST be assigned second, to follow strict aliasing rules.
	steps[self_offset].params = calloc(param_count, (sizeof(EquationParameter) + sizeof(uint8_t)));
	steps[self_offset].parameter_types = (uint8_t *) (steps[self_offset].params + param_count);
	steps[self_offset].parameter_values = calloc(param_count, sizeof(double));
	steps[self_offset].parameter_count = param_count;

#ifdef _DEBUG_MODE
	printf("  - '%s'\n", next);
#endif

	// Loop through all parameters
	for (size_t param = 0; *next != ')'; param++)
	{
		// Handle commas and spacing
		uint8_t comma_found = 0;
		while (*next && (*next == ',' || *next == ' '))
		{
			comma_found |= *next == ',';
			next++;
		}

		// Require a comma to exist between parameters
		if (!comma_found && param != 0)
		{
			fprintf(stderr, "Expected ',' between parameters.\n");
			return 1;
		}

#ifdef _DEBUG_MODE
		printf("  - '%s'\n", next);
#endif

		char *nptr = 0;
		// Handle different parameter types.
		if (*next == '%')
		{
#ifdef _DEBUG_MODE
			printf("  - '%s' (Processing Variable)\n", next);
#endif

			errno = 0;
			long variable_index = strtol(next + 1, &nptr, 10);

			if (errno || variable_index < 1 || (size_t) variable_index >= SIZE_MAX)
			{
				fprintf(stderr, "Error while parsing variable.\n");
				return 1;
			}

			if (eq->variable_count < (size_t) variable_index)
			{
				eq->variable_count = variable_index;
			}

			steps[self_offset].parameter_types[param] = PARAMETER_TYPE_VARIABLE;
			steps[self_offset].params[param].variable_index = variable_index;
		}
		else if (isdigit(*next) || *next == '-' || *next == '.')
		{
			double value = strtod(next, &nptr);

			if (errno)
			{
				fprintf(stderr, "Error while parsing constant.\n");
				return 1;
			}

			steps[self_offset].parameter_types[param] = PARAMETER_TYPE_CONSTANT;
			steps[self_offset].params[param].constant = value;
		}
		else
		{
			// Parse as function
			steps[self_offset].parameter_types[param] = PARAMETER_TYPE_FUNCTION;
			steps[self_offset].params[param].step = ++*offset;
			if (parseEquationStep(next, eq, offset, &nptr))
			{
				return 1;
			}
		}

#ifdef _DEBUG_MODE
			printf("  - '%s' (nptr)\n", nptr);
#endif

		next = nptr;

		// Handle spacing
		while (*next && (*next == ' ')) next++;
	}

	// Remove parenthesis from end of function parse
	if (*next && (*next == ')')) next++;

	if (endptr)
	{
		*endptr = (char *) next;
	}

	return 0;
}

void freeEquation(Equation* eq)
{
	printf ("%lu\n", eq->step_count);
	for (size_t i = 0; i < eq->step_count; i++) {
		free(eq->steps[i].params);
		free(eq->steps[i].parameter_values);
	}

	free(eq->variables);
	free(eq->steps);
	free(eq);
}

double executeEquation(Equation *eq, double *variables, size_t num_vars)
{
	// Assign variables
	size_t num_copy = (num_vars < eq->variable_count) ? num_vars : eq->variable_count;

	memcpy(eq->variables, variables, num_copy * sizeof(double));

	// Evaluate starting at the first function.
	return executeEquationStep(eq, 0);
}

double executeEquationStep(Equation *eq, size_t step_index)
{
	EquationStep *step = eq->steps + step_index;

	// Gather parameter values
	// TODO: Offload this to compile stage, with an array of double pointers.
	for (size_t param = 0; param < step->parameter_count; param++)
	{
		switch (step->parameter_types[param])
		{
			EquationParameter *p;
			size_t vindex;

			case PARAMETER_TYPE_VARIABLE:
				p = step->params+param;
				vindex = p->variable_index - 1;
				step->parameter_values[param] = eq->variables[vindex];
				break;
			case PARAMETER_TYPE_CONSTANT:
				step->parameter_values[param] = step->params[param].constant;
				break;
			case PARAMETER_TYPE_FUNCTION:
				step->parameter_values[param] = executeEquationStep(eq, step->params[param].step);
				break;
			default:
				printf("WARNING: Unknown parameter.\n");
		}
	}

	return step->function(step->parameter_count, step->parameter_values);
}
