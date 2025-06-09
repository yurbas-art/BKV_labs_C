#include <stdio.h>
#include <malloc.h>
#include "fullMatrix.h"


MATRIX mat_add(MATRIX A, MATRIX B)
{
	int	i, j;
	MATRIX	C;

	if ((C = mat_creat(MatRow(A), MatCol(A), UNDEFINED)) == NULL)
		return (NULL);

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			C[i][j] = A[i][j] + B[i][j];
		}
	return (C);
}
MATRIX _mat_creat(int row, int col)
{
	MATBODY* mat;
	int 	i, j;

	if ((mat = (MATBODY*)malloc(sizeof(MATHEAD) + sizeof(double*) * row)) == NULL)
		return (mat_error(MAT_MALLOC));

	for (i = 0; i < row; i++)
	{
		if ((*((double**)(&mat->matrix) + i) = (double*)malloc(sizeof(double) * col)) == NULL)
			return (mat_error(MAT_MALLOC));
	}

	mat->head.row = row;
	mat->head.col = col;

	return (&(mat->matrix));
}
MATRIX mat_creat(int row, int col, int type)
{
	MATRIX	A;

	if ((A = _mat_creat(row, col)) != NULL)
	{
		return (mat_fill(A, type));
	}
	else
		return (NULL);
}
MATRIX mat_fill(MATRIX A, int type)
{
	int	i, j;

	switch (type)
	{
	case UNDEFINED:
		break;
	case ZERO_MATRIX:
	case UNIT_MATRIX:
		for (i = 0; i < MatRow(A); i++)
			for (j = 0; j < MatCol(A); j++)
			{
				if (type == UNIT_MATRIX)
				{
					if (i == j)
					{
						A[i][j] = 1.0;
						continue;
					}
				}
				A[i][j] = 0.0;
			}
		break;
	}
	return (A);
}
MATRIX mat_fill_x(MATRIX A, double angle)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				if ((i == 1) || (i == 2))
					A[i][j] = cos(angle);
				else
					A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[1][2] = sin(angle);
	A[2][1] = -sin(angle);
	return (A);
}
MATRIX mat_fill_y(MATRIX A, double angle)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				if ((i == 0) || (i == 2))
					A[i][j] = cos(angle);
				else
					A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[0][2] = sin(angle);
	A[2][0] = -sin(angle);
	return (A);
}
MATRIX mat_fill_z(MATRIX A, double angle)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				if ((i == 1) || (i == 0))
					A[i][j] = cos(angle);
				else
					A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[1][0] = -sin(angle);
	A[0][1] = sin(angle);
	return (A);
}
MATRIX mat_fill_t(MATRIX A, double dx, double dy, double dz)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[0][3] = dx;
	A[1][3] = dy;
	A[2][3] = dz;
	return (A);
}
MATRIX mat_fill_p(MATRIX A, double vx, double vy, double vz)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	//        if (vx>0.001)
	//		A[0][3] = -1.0/vx; // A[2][2]=0;
	//        if (vy>0.001)
	//		A[1][3] = -1.0/vy; // A[2][2]=0;
	//        if (vz>0.001)
	A[2][3] = -1.0 / vz; // A[2][2]=0;
	return (A);
}
MATRIX mat_fill_s(MATRIX A, double S)
{
	int	i, j, N;
	N = MatRow(A);
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			if (i == j)
			{
				A[i][j] = S;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[N - 1][N - 1] = 1.0;
	return (A);
}
int mat_free(MATRIX A)
{
	int i;

	if (A == NULL)
		return (0);
	for (i = 0; i < MatRow(A); i++)
	{
		free(A[i]);
	}
	free(Mathead(A));
	return (1);
}
MATRIX mat_copy(MATRIX A)
{
	int	i, j;
	MATRIX	C;

	if ((C = mat_creat(MatRow(A), MatCol(A), UNDEFINED)) == NULL)
		return (NULL);

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			C[i][j] = A[i][j];
		}
	return (C);
}
MATRIX mat_fill_R(MATRIX A, int c1, int c2, double angle)
{
	int	i, j;
	if (c1 == c2) return (mat_fill(A, UNIT_MATRIX));
	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				if ((i == c1) || (i == c2))
					A[i][j] = cos(angle);
				else
					A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[c1][c2] = sin(angle);
	A[c2][c1] = -sin(angle);
	return (A);
}
MATRIX mat_fill_T(MATRIX A, double* dx)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				A[i][j] = 1.0;
				continue;
			}
			A[i][j] = 0.0;
		}
	for (i = 0; i < MatRow(A); i++)
		A[MatRow(A) - 1][i] = dx[i];
	return (A);
}
MATRIX mat_fill_S(MATRIX A, double s)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			if (i == j)
			{
				A[i][j] = s;
				continue;
			}
			A[i][j] = 0.0;
		}
	A[MatRow(A) - 1][MatCol(A) - 1] = 1;
	return (A);
}
static double signa[2] = { 1.0, -1.0 };
MATRIX mat_dump(MATRIX A)
{
	return(mat_fdumpf(A, "%f ", stdout));
}
/*
*-----------------------------------------------------------------------------
*	funct:	mat_dumpf
*   desct:  dump a matrix with format string to standard output
*	given:	A = matrice to dumped
*	retrn:	nothing
*	comen:	matrix a dumped to standard output
*-----------------------------------------------------------------------------
*/
MATRIX mat_dumpf(MATRIX A, char* s)
{
	return (mat_fdumpf(A, s, stdout));
}
MATRIX mat_fdump(MATRIX A, FILE* fp)
{
	return (mat_fdumpf(A, "%f ", fp));
}
MATRIX mat_fdumpf(MATRIX A, char* s, FILE* fp)
{
	int	i, j;

	for (i = 0; i < MatRow(A); i++)
	{
		for (j = 0; j < MatCol(A); j++)
		{
			fprintf(fp, s, A[i][j]);
		}
		fprintf(fp, "\n");
	}

	return (A);
}
MATRIX mat_error(int errno)
{
	switch (errno)
	{
	case MAT_MALLOC:
		fprintf(stderr, "mat: malloc error\n");
		break;
	case MAT_FNOTOPEN:
		fprintf(stderr, "mat: fileopen error\n");
		break;
	case MAT_FNOTGETMAT:
		fprintf(stderr, "fgetmat: matrix read error\n");
		break;
	}

	return (NULL);
}
MATRIX mat_mul(MATRIX A, MATRIX B)
{
	int	i, j, k;
	MATRIX	C;

	if ((C = mat_creat(MatRow(A), MatCol(B), UNDEFINED)) == NULL)
		return (NULL);

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(B); j++)
			for (k = 0, C[i][j] = 0.0; k < MatCol(A); k++)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
	return (C);
}
MATRIX mat_sub(MATRIX A, MATRIX B)
{
	int	i, j;
	MATRIX	C;

	if ((C = mat_creat(MatRow(A), MatCol(A), UNDEFINED)) == NULL)
		return (NULL);

	for (i = 0; i < MatRow(A); i++)
		for (j = 0; j < MatCol(A); j++)
		{
			C[i][j] = A[i][j] - B[i][j];
		}
	return (C);
}
MATRIX mat_tran(MATRIX A)
{
	int	i, j;
	MATRIX	At;

	if ((At = mat_creat(MatCol(A), MatRow(A), UNDEFINED)) == NULL)
		return (NULL);

	/*
	*	Transposing ...
	*/
	for (i = 0; i < MatCol(A); i++)
		for (j = 0; j < MatRow(A); j++)
		{
			At[i][j] = A[j][i];
		}
	return (At);
}
