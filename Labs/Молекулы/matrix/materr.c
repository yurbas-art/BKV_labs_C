#include <stdio.h>

#include <malloc.h>

#include "matrix.h"

MATRIX mat_error(int errno )
{
	switch( errno )
		{
		case MAT_MALLOC:
			fprintf(stderr, "mat: malloc error\n" );
			break;
		case MAT_FNOTOPEN:
			fprintf(stderr, "mat: fileopen error\n" );
			break;
		case MAT_FNOTGETMAT:
			fprintf(stderr, "fgetmat: matrix read error\n");
			break;
		}

	return (NULL);
}

