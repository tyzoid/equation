#include "equations.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "At least one parameter required\n");
		return 1;
	}

	Equation *eq = parseEquation(argv[1]);

	if (!eq)
	{
		fprintf(stderr, "Failed to compile equation.\n");
		return 1;
	}

	double *vars = calloc(argc-2, sizeof(double));
	for (int i = 0; i < argc-2; i++)
	{
		vars[i] = strtod(argv[i+2], 0);
	}

	printf("Result: %9.7lf\n", executeEquation(eq, vars, argc-2));

	freeEquation(eq);
	free(vars);
	eq = 0;

	return 0;
}
