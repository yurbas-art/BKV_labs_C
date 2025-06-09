#include <stdio.h>
#include <malloc.h>
#include "matcreat4.h"
#include "matrix.h"

MATRIX _mat_creat( int row, int col )
{
	MATBODY	*mat;
	int 	i, j;

	if ((mat = (MATBODY *)malloc( sizeof(MATHEAD) + sizeof(double *) * row)) == NULL)
		return (mat_error( MAT_MALLOC ));

	for (i=0; i<row; i++)
	{
	if ((*((double **)(&mat->matrix) + i) = (double *)malloc(sizeof(double) * col)) == NULL)
		return (mat_error( MAT_MALLOC ));
	}

	mat->head.row = row;
	mat->head.col = col;

	return (&(mat->matrix));
}

MATRIX mat_creat( int row, int col, int type )
{
	MATRIX	A;

	if ((A =_mat_creat( row, col )) != NULL)
		{
		return (mat_fill(A, type));
		}
	else
		return (NULL);
}

MATRIX mat_fill( MATRIX A, int type )
{
	int	i, j;

	switch (type)
		{
		case UNDEFINED:
			break;
		case ZERO_MATRIX:
		case UNIT_MATRIX:
			for (i=0; i<MatRow(A); i++)
			for (j=0; j<MatCol(A); j++)
				{
				if (type == UNIT_MATRIX)
					{
					if (i==j)
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

MATRIX mat_fill_x( MATRIX A,  double angle)
{
	int	i, j;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				if ((i==1)||(i==2))
				A[i][j]=cos(angle);
				else
				A[i][j] = 1.0;
				continue;
				}
		A[i][j] = 0.0;
		}
		A[1][2] =  sin(angle);
		A[2][1] = -sin(angle);
	return (A);
}
MATRIX mat_fill_y( MATRIX A,  double angle)
{
	int	i, j;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				if ((i==0)||(i==2))
				A[i][j]=cos(angle);
				else
				A[i][j] = 1.0;
				continue;
				}
		A[i][j] = 0.0;
		}
		A[0][2] =  sin(angle);
		A[2][0] = -sin(angle);
	return (A);
}
MATRIX mat_fill_z( MATRIX A,  double angle)
{
	int	i, j;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
			if (i==j)
				{
				if ((i==1)||(i==0))
				A[i][j]=cos(angle);
				else
				A[i][j] = 1.0;
				continue;
				}
		A[i][j] = 0.0;
		}
		A[1][0] = -sin(angle);
		A[0][1] =  sin(angle);
	return (A);
}
MATRIX mat_fill_t( MATRIX A,  double dx, double dy, double dz)
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
		A[3][0] = dx;
		A[3][1] = dy;
		A[3][2] = dz;
	return (A);
}
MATRIX mat_fill_p( MATRIX A,  double vx, double vy, double vz)
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
//        if (vx>0.001)
//		A[0][3] = -1.0/vx; // A[2][2]=0;
//        if (vy>0.001)
//		A[1][3] = -1.0/vy; // A[2][2]=0;
//        if (vz>0.001)
		A[2][3] = -1.0/vz; // A[2][2]=0;
	return (A);
}

MATRIX mat_fill_s( MATRIX A,  double S)
{
	int	i, j, N;
        N=MatRow(A);
	for (i=0; i<N; i++)
	for (j=0; j<N; j++)
		{
			if (i==j)
				{
				A[i][j] = S;
				continue;
				}
		A[i][j] = 0.0;
		}
		A[N-1][N-1] = 1.0;
	return (A);
}


int mat_free(MATRIX A )
{
	int i;

	if (A == NULL)
		return (0);
	for (i=0; i<MatRow(A); i++)
		{
		free( A[i] );
		}
	free( Mathead(A) );
	return (1);
}

MATRIX mat_copy(MATRIX A )
{
	int	i, j;
	MATRIX	C;

	if ((C = mat_creat( MatRow(A), MatCol(A), UNDEFINED )) == NULL)
		return (NULL);

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
		C[i][j] = A[i][j];
		}
	return (C);
}


MATRIX mat_colcopy1(MATRIX A, MATRIX B, int cola,int  colb )
{
	int	i, n;

	n = MatRow(A);
	for (i=0; i<n; i++)
		{
		A[i][cola] = B[i][colb];
		}
	return (A);
}

int fgetmat(MATRIX A, FILE *fp )
{
	int 	i, j, k=0;

	for (i=0; i<MatRow(A); i++)
	for (j=0; j<MatCol(A); j++)
		{
		k += fscanf( fp, "%lf", &A[i][j] );
		}
	return (k);
}
