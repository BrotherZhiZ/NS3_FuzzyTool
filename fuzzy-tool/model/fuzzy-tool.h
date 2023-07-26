/*
 * Stand-alone C codes for fuzzy inference systems.
 * (This file is included in fis project)
 * J.-S. Roger Jang, 1994.
 * Copyright 1994-2001 The MathWorks, Inc.
 */

#ifndef __FIS__
#define __FIS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// #pragma warning(disable: 4244)

/***********************************************************************
 Macros and definitions
 **********************************************************************/
/* Define portable printf and double */
#if defined(MATLAB_MEX_FILE)
#define PRINTF mexPrintf
#define DOUBLE real_T
#elif defined(__SIMSTRUC__)
#define PRINTF ssPrintf
#define DOUBLE real_T
#elif defined(_MSC_VER)

#define PRINTF odprintf

#include <stdarg.h>
#include <Windows.h>

int _cdecl odprintf(const char *fmt, ...)
{
	char buf[4096], *p = buf;
	va_list args;
	int ret = -1;

	va_start(args, fmt);
	ret = vsnprintf_s(p, sizeof(buf), _TRUNCATE, fmt, args);
	p += ret;
	va_end(args);

	while (p > buf && isspace(*p))
	{
		*--p = '\0';
		*p++ = '\r';
		*p++ = '\n';
		*p = '\0';
	}

	OutputDebugStringA(buf);

	return (ret);
}
#else
#define PRINTF printf
#define DOUBLE double
#endif

#ifndef ABS
#define ABS(x) ((x) > (0) ? (x) : (-(x)))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#define MF_PARA_N 4
#define STR_LEN 500
#define MF_POINT_N 101

/* debugging macros */
/*
#define PRINT(expr) printf(#expr " = %g\n", (double)expr)
#define PRINTMAT(mat,m,n) printf(#mat " = \n"); fisPrintMatrix(mat,m,n)
#define FREEMAT(mat,m) printf("Free " #mat " ...\n"); fisFreeMatrix(mat,m)
#define FREEARRAY(array) printf("Free " #array " ...\n"); free(array)
*/

#if (defined(MATLAB_MEX_FILE) && !defined(__SIMSTRUC__))
#define FREE mxFree
#else
#define FREE free
#endif

#define FREEMAT(mat, m) fisFreeMatrix(mat, m)
#define FREEARRAY(array) FREE(array)

/***********************************************************************
 Data types FIS�Ľṹ��
 **********************************************************************/
typedef struct fis_node FIS;

typedef struct fis_node
{
	int handle;
	int load_param;
	char name[STR_LEN];
	char type[STR_LEN];
	char andMethod[STR_LEN];
	char orMethod[STR_LEN];
	char impMethod[STR_LEN];
	char aggMethod[STR_LEN];
	char defuzzMethod[STR_LEN];
	int userDefinedAnd;
	int userDefinedOr;
	int userDefinedImp;
	int userDefinedAgg;
	int userDefinedDefuzz;
	int in_n;
	int out_n;
	int rule_n;
	int **rule_list;
	DOUBLE *rule_weight;
	int *and_or; /* AND-OR indicator */
	DOUBLE *firing_strength;
	DOUBLE *rule_output;
	/* Sugeno: output for each rules */
	/* Mamdani: constrained output MF values of rules */
	struct io_node **input;
	struct io_node **output;
	DOUBLE(*andFcn)
	(DOUBLE, DOUBLE);
	DOUBLE(*orFcn)
	(DOUBLE, DOUBLE);
	DOUBLE(*impFcn)
	(DOUBLE, DOUBLE);
	DOUBLE(*aggFcn)
	(DOUBLE, DOUBLE);

	// DOUBLE (*defuzzFcn)();  hemmingway <hemmingway@163.com>  2013/01/02
	DOUBLE(*defuzzFcn)
	(FIS *fis, int m, DOUBLE *mf, int numofpoints);

	DOUBLE *BigOutMfMatrix;	 /* used for Mamdani system only */
	DOUBLE *BigWeightMatrix; /* used for Mamdani system only */
	DOUBLE *mfs_of_rule;	 /* MF values in a rule */

	DOUBLE *bias; /*bias, to be tuned when no rules are fired*/
	int isbias;

	struct fis_node *next;
} FIS;

typedef struct io_node
{
	char name[STR_LEN];
	int mf_n;
	DOUBLE bound[2];
	DOUBLE value;
	struct mf_node **mf;
} IO;

typedef struct mf_node
{
	char label[STR_LEN]; /* MF name */
	char type[STR_LEN];	 /* MF type */
	int nparams;		 /* length of params field */
	DOUBLE *params;		 /* MF parameters */
	int userDefined;	 /* 1 if the MF is user-defined */
	DOUBLE(*mfFcn)
	(DOUBLE, DOUBLE *);	 /* pointer to a mem. fcn */
	DOUBLE value;		 /* for Sugeno only */
	DOUBLE *value_array; /* for Mamdani only, array of MF values */
} MF;

/* display error message and exit */
__attribute__((unused)) static void fisError(char *msg)
{
#ifdef MATLAB_MEX_FILE
	mexErrMsgTxt(msg);
#else
	PRINTF("%s\n", msg);

#ifndef _MSC_VER
	exit(1);
#endif

#endif
};

__attribute__((unused)) static DOUBLE fisMin(DOUBLE x, DOUBLE y)
{
	return ((x) < (y) ? (x) : (y));
};

__attribute__((unused)) static DOUBLE fisMax(DOUBLE x, DOUBLE y)
{
	return ((x) > (y) ? (x) : (y));
};

__attribute__((unused)) static DOUBLE fisProduct(DOUBLE x, DOUBLE y)
{
	return (x * y);
};

__attribute__((unused)) static DOUBLE fisProbOr(DOUBLE x, DOUBLE y)
{
	return (x + y - x * y);
};

__attribute__((unused)) static DOUBLE fisSum(DOUBLE x, DOUBLE y)
{
	return (x + y);
};

/***********************************************************************
 Parameterized membership functions
 **********************************************************************/

#ifndef UNUSED_PARAMETER
#if defined(__LCC__)
#define UNUSED_PARAMETER(x) /* do nothing */
#else
/*
 * This is the semi-ANSI standard way of indicating that a
 * unused function parameter is required.
 */
#define UNUSED_PARAMETER(x) (void)(x)
#endif
#endif

/* all membership function */
__attribute__((unused)) static DOUBLE fisAllMf(DOUBLE x, DOUBLE *params)
{
	UNUSED_PARAMETER(x);
	UNUSED_PARAMETER(params);
	return (1);
}

/* Triangular membership function */
__attribute__((unused)) static DOUBLE fisTriangleMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], b = params[1], c = params[2];

	if (a > b)
		fisError((char *)"Illegal parameters in fisTriangleMf() --> a > b");
	if (b > c)
		fisError((char *)"Illegal parameters in fisTriangleMf() --> b > c");

	if (a == b && b == c)
		return (x == a);
	if (a == b)
		return ((c - x) / (c - b) * (b <= x) * (x <= c));
	if (b == c)
		return ((x - a) / (b - a) * (a <= x) * (x <= b));
	return (MAX(MIN((x - a) / (b - a), (c - x) / (c - b)), 0));
};

/* Trapezpoidal membership function */
__attribute__((unused)) static DOUBLE fisTrapezoidMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], b = params[1], c = params[2], d = params[3];
	DOUBLE y1 = 0, y2 = 0;

	if (a > b)
	{
		PRINTF("a = %f, b = %f, c = %f, d = %f\n", a, b, c, d);
		fisError((char *)"Illegal parameters in fisTrapezoidMf() --> a > b");
	}
	if (b > c)
	{
		PRINTF("a = %f, b = %f, c = %f, d = %f\n", a, b, c, d);
		fisError((char *)"Illegal parameters in fisTrapezoidMf() --> b > c");
	}
	if (c > d)
	{
		PRINTF("a = %f, b = %f, c = %f, d = %f\n", a, b, c, d);
		fisError((char *)"Illegal parameters in fisTrapezoidMf() --> c > d");
	}

	if (b <= x)
		y1 = 1;
	else if (x < a)
		y1 = 0;
	else if (a != b)
		y1 = (x - a) / (b - a);

	if (x <= c)
		y2 = 1;
	else if (d < x)
		y2 = 0;
	else if (c != d)
		y2 = (d - x) / (d - c);

	return (MIN(y1, y2));
	/*
	if (a == b && c == d)
		return((b<=x)*(x<=c));
	if (a == b)
		return(MIN(1, (d-x)/(d-c))*(b<=x)*(x<=d));
	if (c == d)
		return(MIN((x-a)/(b-a), 1)*(a<=x)*(x<=c));
	return(MAX(MIN(MIN((x-a)/(b-a), 1), (d-x)/(d-c)), 0));
	*/
};

/* Gaussian membership function */
__attribute__((unused)) static DOUBLE fisGaussianMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE sigma = params[0], c = params[1];
	DOUBLE tmp;

	if (sigma == 0)
		fisError((char *)"Illegal parameters in fisGaussianMF() --> sigma = 0");
	tmp = (x - c) / sigma;
	return (exp(-tmp * tmp / 2));
};

/* Extended Gaussian membership function */
__attribute__((unused)) static DOUBLE fisGaussian2Mf(DOUBLE x, DOUBLE *params)
{
	DOUBLE sigma1 = params[0], c1 = params[1];
	DOUBLE sigma2 = params[2], c2 = params[3];
	DOUBLE tmp1, tmp2;

	if ((sigma1 == 0) || (sigma2 == 0))
		fisError((char *)"Illegal parameters in fisGaussian2MF() --> sigma1 or sigma2 is zero");

	tmp1 = x >= c1 ? 1 : exp(-pow((x - c1) / sigma1, 2.0) / 2);
	tmp2 = x <= c2 ? 1 : exp(-pow((x - c2) / sigma2, 2.0) / 2);
	return (tmp1 * tmp2);
};

/* Sigmoidal membership function */
__attribute__((unused)) static DOUBLE fisSigmoidMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], c = params[1];
	return (1 / (1 + exp(-a * (x - c))));
};

/* Product of two sigmoidal functions */
__attribute__((unused)) static DOUBLE fisProductSigmoidMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a1 = params[0], c1 = params[1], a2 = params[2], c2 = params[3];
	DOUBLE tmp1 = 1 / (1 + exp(-a1 * (x - c1)));
	DOUBLE tmp2 = 1 / (1 + exp(-a2 * (x - c2)));
	return (tmp1 * tmp2);
};
/* Absolute difference of two sigmoidal functions */
__attribute__((unused)) static DOUBLE fisDifferenceSigmoidMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a1 = params[0], c1 = params[1], a2 = params[2], c2 = params[3];
	DOUBLE tmp1 = 1 / (1 + exp(-a1 * (x - c1)));
	DOUBLE tmp2 = 1 / (1 + exp(-a2 * (x - c2)));
	return (fabs(tmp1 - tmp2));
};

/* Generalized bell membership function */
__attribute__((unused)) static DOUBLE fisGeneralizedBellMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], b = params[1], c = params[2];
	DOUBLE tmp;
	if (a == 0)
		fisError((char *)"Illegal parameters in fisGeneralizedBellMf() --> a = 0");
	tmp = pow((x - c) / a, 2.0);
	if (tmp == 0 && b == 0)
		return (0.5);
	else if (tmp == 0 && b < 0)
		return (0.0);
	else
		return (1 / (1 + pow(tmp, b)));
};

/* S membership function */
__attribute__((unused)) static DOUBLE fisSMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], b = params[1];
	DOUBLE out;

	if (a >= b)
		return (x >= (a + b) / 2);

	if (x <= a)
		out = 0;
	else if (x <= (a + b) / 2)
		out = 2 * pow((x - a) / (b - a), 2.0);
	else if (x <= b)
		out = 1 - 2 * pow((b - x) / (b - a), 2.0);
	else
		out = 1;
	return (out);
};

/* Z membership function */
__attribute__((unused)) static DOUBLE fisZMf(DOUBLE x, DOUBLE *params)
{
	DOUBLE a = params[0], b = params[1];
	DOUBLE out;

	if (a >= b)
		return (x <= (a + b) / 2);

	if (x <= a)
		out = 1;
	else if (x <= (a + b) / 2)
		out = 1 - 2 * pow((x - a) / (b - a), 2.0);
	else if (x <= b)
		out = 2 * pow((b - x) / (b - a), 2.0);
	else
		out = 0;
	return (out);
};

/* pi membership function */
__attribute__((unused)) static DOUBLE fisPiMf(DOUBLE x, DOUBLE *params)
{
	return (fisSMf(x, params) * fisZMf(x, params + 2));
};

/* apply given function to an array */
__attribute__((unused)) static DOUBLE fisArrayOperation(DOUBLE *array, int size, DOUBLE (*fcn)(DOUBLE, DOUBLE))
{
	int i;
	DOUBLE out;

	if (size == 0)
		fisError((char *)"Given size is zero!");

	out = array[0];
	for (i = 1; i < size; i++)
		out = (*fcn)(out, array[i]);
	return (out);
}

/***********************************************************************
 Defuzzification methods
 **********************************************************************/

/* return the center of area of combined output MF (specified by mf)
   of output m */
/* numofpoints is the number of partition for integration */
__attribute__((unused)) static DOUBLE defuzzCentroid(FIS *fis, int m, DOUBLE *mf, int numofpoints)
{
	DOUBLE min = fis->output[m]->bound[0];
	DOUBLE max = fis->output[m]->bound[1];
	DOUBLE step = (max - min) / (numofpoints - 1);
	DOUBLE total_mf = 0;
	DOUBLE sum = 0;
	int i;

	for (i = 0; i < numofpoints; i++)
	{
		total_mf += mf[i];
		sum += mf[i] * (min + step * i);
	}
	if (total_mf == 0)
	{
		PRINTF("Total area is zero in defuzzCentroid() for output %d.\n", m + 1);
		PRINTF("Average of the range of this output variable is used as the output value.\n\n");
		return ((fis->output[m]->bound[0] + fis->output[m]->bound[1]) / 2);
	}
	return (sum / total_mf);
};

/* return the bisector of area of mf */
__attribute__((unused)) static DOUBLE defuzzBisector(FIS *fis, int m, DOUBLE *mf, int numofpoints)
{
	DOUBLE min = fis->output[m]->bound[0];
	DOUBLE max = fis->output[m]->bound[1];
	DOUBLE step = (max - min) / (numofpoints - 1);
	DOUBLE area, sub_area;
	int i;

	/* find the total area */
	area = 0;
	for (i = 0; i < numofpoints; i++)
		area += mf[i];

	if (area == 0)
	{
		PRINTF("Total area is zero in defuzzBisector() for output %d.\n", m + 1);
		PRINTF("Average of the range of this output variable is used as the output value.\n");
		return ((fis->output[m]->bound[0] + fis->output[m]->bound[1]) / 2);
	}

	sub_area = 0;
	for (i = 0; i < numofpoints; i++)
	{
		sub_area += mf[i];
		if (sub_area >= area / 2)
			break;
	}
	return (min + step * i);
};

/* Returns the mean of maximizing x of mf */
__attribute__((unused)) static DOUBLE defuzzMeanOfMax(FIS *fis, int m, DOUBLE *mf, int numofpoints)
{
	DOUBLE min = fis->output[m]->bound[0];
	DOUBLE max = fis->output[m]->bound[1];
	DOUBLE step = (max - min) / (numofpoints - 1);
	DOUBLE mf_max;
	DOUBLE sum;
	int count;
	int i;

	mf_max = fisArrayOperation(mf, numofpoints, fisMax);

	sum = 0;
	count = 0;
	for (i = 0; i < numofpoints; i++)
		if (mf[i] == mf_max)
		{
			count++;
			sum += i;
		}
	return (min + step * sum / count);
};

/* Returns the smallest (in magnitude) maximizing x of mf */
__attribute__((unused)) static DOUBLE defuzzSmallestOfMax(FIS *fis, int m, DOUBLE *mf, int numofpoints)
{
	DOUBLE min = fis->output[m]->bound[0];
	DOUBLE max = fis->output[m]->bound[1];
	DOUBLE step = (max - min) / (numofpoints - 1);
	DOUBLE mf_max;
	int i, min_index = 0;
	DOUBLE min_distance = pow(2.0, 31.0) - 1;
	DOUBLE distance; /* distance to the origin */

	mf_max = fisArrayOperation(mf, numofpoints, fisMax);
	for (i = 0; i < numofpoints; i++)
		if (mf[i] == mf_max)
		{
			distance = ABS(min + step * i);
			if (min_distance > distance)
			{
				min_distance = distance;
				min_index = i;
			}
		}
	return (min + step * min_index);
};

/* Returns the largest (in magnitude) maximizing x of mf */
__attribute__((unused)) static DOUBLE defuzzLargestOfMax(FIS *fis, int m, DOUBLE *mf, int numofpoints)
{
	DOUBLE min = fis->output[m]->bound[0];
	DOUBLE max = fis->output[m]->bound[1];
	DOUBLE step = (max - min) / (numofpoints - 1);
	DOUBLE mf_max;
	int i, max_index = 0;
	DOUBLE max_distance = -(pow(2.0, 31.0) - 1);
	DOUBLE distance; /* distance to the origin */

	mf_max = fisArrayOperation(mf, numofpoints, fisMax);
	for (i = 0; i < numofpoints; i++)
		if (mf[i] == mf_max)
		{
			distance = ABS(min + step * i);
			if (max_distance < distance)
			{
				max_distance = distance;
				max_index = i;
			}
		}
	return (min + step * max_index);
};

class FuzzyTool
{
public:
	FuzzyTool(){};
	~FuzzyTool(){};

	FILE *fisOpenFile(char *file, char *mode);
	void *fisCalloc(int num_of_x, int size_of_x);
	char **fisCreateMatrix(int row_n, int col_n, int element_size);
	void fisFreeMatrix(void **matrix, int row_n);
	DOUBLE **fisCopyMatrix(DOUBLE **source, int row_n, int col_n);
	void fisPrintMatrix(DOUBLE **matrix, int row_n, int col_n);
	void fisPrintArray(DOUBLE *array, int size);
	void fisPause();
	int fisGetMfParaN(char *mfType);
	DOUBLE fisAllMf(DOUBLE x, DOUBLE *params);

	IO *fisBuildIoList(int node_n, int *mf_n);
	void fisAssignMfPointer(FIS *fis);
	void fisAssignFunctionPointer(FIS *fis);
	void fisPrintData(FIS *fis);
	void fisFreeMfList(MF *mf_list, int n);
	void fisFreeIoList(IO *io_list, int n);
	void fisFreeFisNode(FIS *fis);
	void fisComputeOutputMfValueArray(FIS *fis, int numofpoints);
	void fisGetString2(char *target, DOUBLE *array, int max_leng);
	void fisCheckDataStructure(FIS *fis);
	void fisBuildFisNode(FIS *fis, DOUBLE **fismatrix, int col_n, int numofpoints);
	void fisLoadParameter(FIS *fis, DOUBLE **fismatrix, int numofpoints);
	void fisLoadParameter1(FIS *fis, DOUBLE *para_array, int numofpoints);
	FIS *fisMatchHandle(FIS *head, int handle);
	FIS *fisMatchName(FIS *head, char *name);
	int fisFindMaxHandle(FIS *head);
	void fisComputeInputMfValue(FIS *fis);
	void fisEvaluate(FIS *fis, int numofpoints);
	void fisComputeTskRuleOutput(FIS *fis);
	void fisComputeFiringStrength(FIS *fis);
	DOUBLE fisFinalOutputMf(FIS *fis, int m, int n);
	void fisFinalOutputMf2(FIS *fis, int m, DOUBLE *aggMF, int numofpoints);
	void getFisOutput(DOUBLE *input, FIS *fis, DOUBLE *output);
	char *getNextLine(char *buf, FILE *fp);
	DOUBLE getNumber(char *buf, FILE *fp);
	void getString(char *buf, FILE *fp, DOUBLE *array);
	int getArray(char *string, DOUBLE *array);
	void getMfN(char *filename, int in_n, DOUBLE *in_mf_n, int out_n, DOUBLE *out_mf_n);
	DOUBLE **returnEmptyFismatrix(char *filename, int *row_n_p, int *col_n_p);
	DOUBLE **returnFismatrix(char *fis_file, int *row_n_p, int *col_n_p);
	DOUBLE **returnDataMatrix(char *filename, int *row_n_p, int *col_n_p);
};

#endif /* __FIS__ */
