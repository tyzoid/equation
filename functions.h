#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#define EQUATION_ADD_STR "add"
#define EQUATION_SUB_STR "sub"
#define EQUATION_MUL_STR "mul"
#define EQUATION_DIV_STR "div"
#define EQUATION_EXP_STR "exp"
#define EQUATION_POW_STR "pow"

#define EQUATION_ABS_STR "abs"

#define EQUATION_LT_STR  "lt"
#define EQUATION_LTE_STR "lte"
#define EQUATION_GT_STR  "gt"
#define EQUATION_GTE_STR "gte"
#define EQUATION_EQ_STR  "eq"

#define EQUATION_AND_STR "and"
#define EQUATION_OR_STR  "or"
#define EQUATION_NOT_STR "not"
#define EQUATION_XOR_STR "xor"
#define EQUATION_IF_STR  "if"

double eqfunc_add(size_t, double*);
double eqfunc_sub(size_t, double*);
double eqfunc_mul(size_t, double*);
double eqfunc_div(size_t, double*);
double eqfunc_exp(size_t, double*);
double eqfunc_pow(size_t, double*);

double eqfunc_abs(size_t, double*);

double eqfunc_lt(size_t, double*);
double eqfunc_lte(size_t, double*);
double eqfunc_gt(size_t, double*);
double eqfunc_gte(size_t, double*);
double eqfunc_eq(size_t, double*);

double eqfunc_and(size_t, double*);
double eqfunc_or(size_t, double*);
double eqfunc_not(size_t, double*);
double eqfunc_xor(size_t, double*);
double eqfunc_if(size_t, double*);

#endif
