#include <stdio.h>
#include "matrix.h"

MATRIX mat_mul(MATRIX A, MATRIX B )
{
	int	i, j, k;
	MATRIX	C;

	if ((C = mat_creat( MatRow(A), MatCol(B), UNDEFINED )) == NULL)
		return (NULL);

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(B); j++)
	for (k=0, C[i][j]=0.0; k<MatCol(A); k++)
		{
		C[i][j] += A[i][k] * B[k][j];
		}
	return (C);
}



