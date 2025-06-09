#include <math.h>
typedef struct {
	int	row;
	int	col;
}	MATHEAD;

typedef struct {
	MATHEAD	head;
	double* matrix;
}	MATBODY;

typedef	double** MATRIX;

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



MATRIX mat_fill_R(MATRIX A, int c1, int c2, double angle);
MATRIX mat_fill_T(MATRIX A, double* dx);
MATRIX mat_fill_S(MATRIX A, double s);
MATRIX mat_add(MATRIX A, MATRIX B);
MATRIX _mat_creat(int row, int col);
MATRIX mat_creat(int row, int col, int type);
MATRIX mat_fill(MATRIX A, int type);
MATRIX mat_fill_x(MATRIX A, double angle);
MATRIX mat_fill_y(MATRIX A, double angle);
MATRIX mat_fill_z(MATRIX A, double angle);
MATRIX mat_fill_t(MATRIX A, double dx, double dy, double dz);
MATRIX mat_fill_p(MATRIX A, double vx, double vy, double vz);
MATRIX mat_fill_s(MATRIX A, double S);
int mat_free(MATRIX A);
MATRIX mat_copy(MATRIX A);
MATRIX mat_fill_R(MATRIX A, int c1, int c2, double angle);
MATRIX mat_fill_T(MATRIX A, double* dx);
MATRIX mat_fill_S(MATRIX A, double s);
double mat_minor(MATRIX A, int i, int  j);
double mat_cofact(MATRIX A, int i, int j);
double mat_det(MATRIX a);
MATRIX mat_dump(MATRIX A);
MATRIX mat_dumpf(MATRIX A, char* s);
MATRIX mat_fdump(MATRIX A, FILE* fp);
MATRIX mat_fdumpf(MATRIX A, char* s, FILE* fp);
MATRIX mat_error(int errno);
MATRIX mat_inv(MATRIX a);
MATRIX mat_mul(MATRIX A, MATRIX B);
MATRIX mat_sub(MATRIX A, MATRIX B);
MATRIX mat_tran(MATRIX A);
