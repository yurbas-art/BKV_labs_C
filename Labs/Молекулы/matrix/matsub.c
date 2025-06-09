#include <stdio.h>
#include "matrix.h"

MATRIX mat_sub(MATRIX A, MATRIX B )
{
	int	i, j;
	MATRIX	C;

	if ((C = mat_creat( MatRow(A), MatCol(A), UNDEFINED )) == NULL)
		return (NULL);

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
		C[i][j] = A[i][j] - B[i][j];
		}
	return (C);
}
