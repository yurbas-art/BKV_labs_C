/*
	Remember to compile try:
		1) gcc X.c -o X -lX11
*/

#define _CRT_SECURE_NO_WARNINGS

//#define DEBUG

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "fullMatrix.h"


typedef struct Atom {
	double x, y, z;
	double nx, ny, nz;
	double R;
	double nR;
	long color;
	char name[10];
} Atom;

typedef struct Connection {
	Atom* m1;
	Atom* m2;
	double z;
} Connection;



int init();
void close();
void rasterCircle(int x0, int y0, int radius);
int keyHandle();
int readData(FILE* file);
void setDataByName(Atom* mol);
void setPosirionAndRotation();
void drawAtoms();
int sortByZ(const void* a, const void* b);
int sortConnectionsByZ(const void* a, const void* b);
void createMatrixes();
void calcCentroid();
void deleteMatrixes();

#define WINDOW_K	1.2
#define WINDOW_HEIGHT	(int)(1080/WINDOW_K)
#define WINDOW_WIDTH	(int)(1920/WINDOW_K)

SDL_Window* window;
SDL_Surface* surface;
SDL_Renderer* ren;
SDL_Event e;


int N = 0;
Atom* atoms[10000];

int G = 0;
Connection* connections[10000];

MATRIX Rx, Ry, Rz, S, T;
double Tx = WINDOW_WIDTH / 2, Ty = WINDOW_HEIGHT / 2, Tz = 1;
MATRIX centroid;

/*#define WHITE	255,	255,	255,	255
#define BLUE	0,		0,		255,	255
#define PINK	255,	192,	203,	255
#define PURPLE	255,	0,		255,	255
#define ORANGE	255,	255,	0,		255
#define GREEN	0,		255,	0,		255
#define RED		255,	0,		0,		255
#define YELLOW	255,	255,	0,		255
#define BLACK	0,		0,		0,		255*/

#define WHITE   0xFFFFFFFF  // R=255, G=255, B=255, A=255
#define BLUE    0x0000FFFF  // R=0,   G=0,   B=255, A=255
#define PINK    0xFFC0CBFF  // R=255, G=192, B=203, A=255
#define PURPLE  0xFF00FFFF  // R=255, G=0,   B=255, A=255
#define ORANGE  0xFFFF00FF  // R=255, G=255, B=0,   A=255
#define GREEN   0x00FF00FF  // R=0,   G=255, B=0,   A=255
#define RED     0xFF0000FF  // R=255, G=0,   B=0,   A=255
#define YELLOW  0xFFFF00FF  // R=255, G=255, B=0,   A=255
#define BLACK   0x000000FF  // R=0,   G=0,   B=0,   A=255
#define GREY 0x808080FF	

#define convColor(a) ((a >> 24) & 0xFF), ((a >> 16) & 0xFF), ((a >> 8) & 0xFF), (a & 0xFF)


double Alpha = 0;
double Betta = 0;
double Gamma = 0;
double Scale = 100;

int main(int argc, char* argv[])
{
#ifdef DEBUG
	argc = 2;
	argv[1] = "C:\\Users\\Пользователь\\Desktop\\adaman.txt";
#endif

	if (argc == 1)
	{
		printf("Too little arguments.");
		return -1;
	}
	else if (argc > 2)
	{
		printf("Too many arguments.");
		return -1;
	}

	FILE* file = fopen(argv[1], "r");
	if (readData(file) == -1)
	{
		printf("Fail to read file.");
		fclose(file);
		return -1;
	}

	fclose(file);



	if (!init())
	{
		printf("Fail to init window");
		return -1;
	}

	createMatrixes();


	SDL_SetRenderDrawColor(ren, convColor(RED));
	SDL_RenderClear(ren);

	int i = 0;

	printf("buttons: up, down, left, right, W, S,\n- (- scale), + (+ scale)\n3 (- X), 4 (+ X)\n5 (- Y), 6 (+ Y)\n\n[ ESC to exit ]");

	calcCentroid();
	while (1)
	{
		if (keyHandle() == -1)
		{
			break;
		}

		setPosirionAndRotation();
		SDL_SetRenderDrawColor(ren, convColor(BLACK));
		SDL_RenderClear(ren);
		drawAtoms();

#ifdef DEBUG
		for (i = 0; i < N; i++)
		{
			printf("%lf %lf %lf %d\n", atoms[i]->nx, atoms[i]->ny, atoms[i]->nz, atoms[i]->R);
		}
#endif


		SDL_RenderPresent(ren);
		SDL_UpdateWindowSurface(window);
	}



	while (SDL_PollEvent(&e) == 0 || e.key.key != SDLK_ESCAPE);


	for (i = 0; i < N; i++)
		free(atoms[i]);

	for (i = 0; i < G; i++)
		free(connections[i]);

	deleteMatrixes();

	close();

	return 0;
}

int init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Fail to init SDL.\n");
		return 0;
	}

	window = SDL_CreateWindow("Flowers", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

	if (!window) {
		printf("Fail to create window: %s.\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	surface = SDL_GetWindowSurface(window);

	if (!surface)
	{
		printf("Fail to get window surface: %s.\n", SDL_GetError());
		return 0;
	}

	ren = SDL_CreateSoftwareRenderer(surface);

	if (!ren)
	{
		printf("Fail to get render: %s.\n", SDL_GetError());
		return 0;
	}

	return 1;
}

void close()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(ren);
	SDL_Quit();
}

void rasterCircle(int x0, int y0, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;


	SDL_RenderLine(ren, x0, y0 - radius, x0, y0 + radius);
	SDL_RenderLine(ren, x0 + radius, y0, x0 - radius, y0);

	/*
	SDL_RenderPoint(ren, x0, y0 + radius);
	SDL_RenderPoint(ren, x0, y0 - radius);
	SDL_RenderPoint(ren, x0 + radius, y0);
	SDL_RenderPoint(ren, x0 - radius, y0);
	*/

	while (x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		SDL_RenderLine(ren, x0 - x, y0 + y, x0 + x, y0 + y);
		SDL_RenderLine(ren, x0 + x, y0 - y, x0 - x, y0 - y);
		SDL_RenderLine(ren, x0 - y, y0 + x, x0 + y, y0 + x);
		SDL_RenderLine(ren, x0 + y, y0 - x, x0 - y, y0 - x);


		/*
		SDL_RenderPoint(ren, x0 + x, y0 + y);
		SDL_RenderPoint(ren, x0 - x, y0 + y);
		SDL_RenderPoint(ren, x0 + x, y0 - y);
		SDL_RenderPoint(ren, x0 - x, y0 - y);

		SDL_RenderPoint(ren, x0 + y, y0 + x);
		SDL_RenderPoint(ren, x0 - y, y0 + x);
		SDL_RenderPoint(ren, x0 + y, y0 - x);
		SDL_RenderPoint(ren, x0 - y, y0 - x);
		*/
	}
}

int keyHandle()
{
	SDL_PollEvent(&e);
	switch (e.key.key)
	{
	case SDLK_ESCAPE:
		return -1;
	case SDLK_1:
		Gamma += 0.05;
		break;
	case SDLK_2:
		Gamma -= 0.05;
		break;
	case SDLK_D:
		Betta += 0.05;
		break;
	case SDLK_A:
		Betta -= 0.05;
		break;
	case SDLK_W:
		Alpha += 0.05;
		break;
	case SDLK_S:
		Alpha -= 0.05;
		break;
	case SDLK_KP_MINUS:
		Scale -= 1;
		break;
	case SDLK_KP_PLUS:
		Scale += 1;
		break;
	case SDLK_3:
		Tx -= 3;
		break;
	case SDLK_4:
		Tx += 3;
		break;
	case SDLK_5:
		Ty -= 3;
		break;
	case SDLK_6:
		Ty += 3;
		break;
	}

	return 0;
}


int readData(FILE* file)
{
	int molCount = 0;
	int conCount = 0;
	int any = 0;
	float fany = 0;

	int m1 = 0, m2 = 0;
	char con[20];


	if (!fscanf(file, "%d %d", &molCount, &conCount) == 2) return -1;

	int i;
	for (i = 0; i < molCount; i++)
	{
		atoms[i] = (Atom*)malloc(sizeof(Atom));
		if (atoms[i] == NULL) return -1;

		atoms[i]->x = 0;
		atoms[i]->y = 0;
		atoms[i]->z = 0;


		if (fscanf(file, " %d %s %lf %lf %lf %f\n", &any, atoms[i]->name, &(atoms[i]->x), &(atoms[i]->y), &(atoms[i]->z), &fany) != 6) return -1;

		setDataByName(atoms[i]);
	}

	for (i = 0; i < conCount; i++)
	{
		connections[i] = (Connection*)malloc(sizeof(Connection));
		if (fscanf(file, " %d %d %d %s\n", &any, &m1, &m2, con) != 4) return -1;
		connections[i]->m1 = atoms[m1 - 1];
		connections[i]->m2 = atoms[m2 - 1];
	}

#ifdef DEBUG
	for (i = 0; i < molCount; i++)
	{
		printf(" %d %s %f %f %f %f\n", i, atoms[i]->name, atoms[i]->x, atoms[i]->y, atoms[i]->z, 0.0);
	}
#endif // DEBUG

	N = molCount;
	G = conCount;

	return 0;

}

void setDataByName(Atom* mol)
{
	char c;
	switch (tolower(mol->name[0]))
	{
	case 'h':
		mol->color = WHITE;
		mol->R = 5;
		break;
	case '~':
		mol->color = PURPLE;
		mol->R = 10;
		break;
	case 'c':

		if (strlen(mol->name) == 1 || tolower(mol->name[1]) == ' ')
		{
			mol->color = WHITE;
			mol->R = 12;
			break;
		}

		c = tolower(mol->name[1]);

		if (c == 'l')
		{
			mol->color = GREEN;
			mol->R = 32;
		}
		else if (c == 'a')
		{
			mol->color = WHITE;
			mol->R = 39;
		}
		else
		{
			mol->color = WHITE;
			mol->R = 32;
		}
		break;
	case 'n':
		mol->color = BLUE;
		mol->R = 14;
		break;
	case 'k':
		mol->color = PURPLE;
		mol->R = 35;
		break;
	case 'o':
		mol->color = RED;
		mol->R = 16;
		break;
	case 'p':
		mol->color = ORANGE;
		mol->R = 30;
		break;
	default:
		mol->color = GREY;
		mol->R = 15;
		break;
	}


}

void setPosirionAndRotation()
{
	int i;
	MATRIX PP;
	MATRIX PB = NULL;

	mat_fill_R(Rx, 2, 1, Alpha);
	mat_fill_R(Ry, 0, 2, Betta);
	mat_fill_R(Rz, 0, 1, Gamma);
	mat_fill_s(S, Scale);
	mat_fill_t(T, Tx, Ty, Tz);

	PP = mat_mul(Ry, Rx);
	PP = mat_mul(Rz, PP);
	PP = mat_mul(S, PP);
	PP = mat_mul(T, PP);
	

	MATRIX v = _mat_creat(4, 1);

	for (i = 0; i < N; i++)
	{
		v[0][0] = atoms[i]->x - centroid[0][0];
		v[1][0] = atoms[i]->y - centroid[1][0];
		v[2][0] = atoms[i]->z - centroid[2][0];
		v[3][0] = 1;

		PB = mat_mul(PP, v);

		atoms[i]->nx = PB[0][0];
		atoms[i]->ny = PB[1][0];
		atoms[i]->nz = PB[2][0];
		atoms[i]->nR = Scale * atoms[i]->R / 100;
	}

	mat_free(PP);
	mat_free(PB);
	mat_free(v);
}

void drawAtoms()
{
	int i;
	for (i = 0; i < G; i++)	
		connections[i]->z = (connections[i]->m1->nz + connections[i]->m2->nz) / 2;	

	qsort(connections, G, sizeof(Connection*), sortConnectionsByZ);
	for (i = 0; i < G; i++)
	{
		SDL_SetRenderDrawColor(ren, convColor(GREEN));
		SDL_RenderLine(ren, connections[i]->m1->nx, connections[i]->m1->ny, connections[i]->m2->nx, connections[i]->m2->ny);
	}

	qsort(atoms, N, sizeof(Atom*), sortByZ);
	for (i = 0; i < N; i++)
	{
		SDL_SetRenderDrawColor(ren, convColor(atoms[i]->color));
		rasterCircle(atoms[i]->nx, atoms[i]->ny, atoms[i]->nR);
	}

}

void createMatrixes()
{
	Rx = _mat_creat(4, 4);
	Ry = _mat_creat(4, 4);
	Rz = _mat_creat(4, 4);
	T = _mat_creat(4, 4);
	S = _mat_creat(4, 4);
	centroid = _mat_creat(4, 1);	
}

void calcCentroid()
{
	int i;
	double ax = 0, ay = 0, az = 0;
	for (i = 0; i < N; i++)
	{
		ax += atoms[i]->x;
		ay += atoms[i]->y;
		az += atoms[i]->z;
	}

	centroid[0][0] = ax / N;
	centroid[1][0] = ay / N;
	centroid[2][0] = az / N;
	centroid[3][0] = 0;
}

void deleteMatrixes()
{
	mat_free(Rx);
	mat_free(Ry);
	mat_free(Rz);
	mat_free(T);
	mat_free(S);
	mat_free(centroid);
}

int sortByZ(const void* a, const void *b)
{
	const Atom* atomA = *(const Atom**)a;
	const Atom* atomB = *(const Atom**)b;

	if (atomA->nz < atomB->nz) return -1;
	if (atomA->nz > atomB->nz) return 1;
	return 0;
}

int sortConnectionsByZ(const void* a, const void* b)
{
	const Connection* cA = *(const Connection**)a;
	const Connection* cB = *(const Connection**)b;

	return (cA->z - cB->z);
}