/*
*-----------------------------------------------------------------------------
*	internal matrix structure
*-----------------------------------------------------------------------------
*/
#include <math.h>
typedef struct {
	int	row;
	int	col;
	}	MATHEAD;

typedef struct {
	MATHEAD	head;
	double	*matrix;
	}	MATBODY;

typedef	double	**MATRIX;

#define	Mathead(a)	((MATHEAD *)((MATHEAD *)(a) - 1))
#define MatRow(a)	(Mathead(a)->row)
#define	MatCol(a)	(Mathead(a)->col)

/*
*----------------------------------------------------------------------------
*	mat_errors definitions
*----------------------------------------------------------------------------
*/
#define	MAT_MALLOC	1
#define MAT_FNOTOPEN	2
#define	MAT_FNOTGETMAT	3

/*
*----------------------------------------------------------------------------
*	matrice types
*----------------------------------------------------------------------------
*/
#define UNDEFINED	-1
#define ZERO_MATRIX	0
#define	UNIT_MATRIX	1
#define	ROTX_MATRIX	10
#define	ROTY_MATRIX	20
#define	ROTZ_MATRIX	30



MATRIX mat_error	(int);
MATRIX _mat_creat	(int, int);
MATRIX mat_creat	( int, int, int);
MATRIX mat_fill		(MATRIX, int);
MATRIX mat_fill_R	(MATRIX, int,int,double);
MATRIX mat_fill_T	(MATRIX, double*);
MATRIX mat_fill_S	(MATRIX, double);
MATRIX mat_fill_x	(MATRIX, double);
MATRIX mat_fill_y	(MATRIX, double);
MATRIX mat_fill_z	(MATRIX, double);
MATRIX mat_fill_s	(MATRIX, double);
MATRIX mat_fill_t	(MATRIX, double, double, double);
MATRIX mat_fill_p	(MATRIX, double, double, double);
int mat_free		(MATRIX);
MATRIX mat_copy		(MATRIX);
MATRIX mat_colcopy1	(MATRIX,MATRIX,int,int);
int fgetmat		(MATRIX, FILE *);
MATRIX mat_dumpf	(MATRIX, char *);
MATRIX mat_dump		(MATRIX);
MATRIX mat_fdump	(MATRIX, FILE *);
MATRIX mat_fdumpf       (MATRIX, char *, FILE *);

MATRIX mat_add		(MATRIX, MATRIX);
MATRIX mat_sub		(MATRIX, MATRIX);
MATRIX mat_mul		(MATRIX, MATRIX);
MATRIX mat_tran		(MATRIX);

