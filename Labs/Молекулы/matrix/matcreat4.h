#include "matrix.h"


MATRIX mat_fill_R( MATRIX A, int c1, int c2,  double angle)
{
	int	i, j;
        if (c1==c2) return (mat_fill(A,UNIT_MATRIX));
	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				if ((i==c1)||(i==c2))
				A[i][j]=cos(angle);
				else
				A[i][j] = 1.0;
				continue;
				}
		A[i][j] = 0.0;
		}
		A[c1][c2] =  sin(angle);
		A[c2][c1] = -sin(angle);
	return (A);
}

MATRIX mat_fill_T( MATRIX A,  double *dx)
{
	int	i, j;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				A[i][j] = 1.0;
				continue;
				}
		A[i][j] = 0.0;
		}
	for (i=0; i<MatRow(A); i++)
		A[MatRow(A)-1][i] = dx[i];
	return (A);
}
MATRIX mat_fill_S( MATRIX A,  double s)
{
	int	i, j;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				A[i][j] = s;
				continue;
				}
		A[i][j] = 0.0;
		}
	A[MatRow(A)-1][MatCol(A)-1] = 1;
	return (A);
}
