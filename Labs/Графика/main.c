/*
	Remember to compile try:
		1) gcc X.c -o X -lX11
*/


#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int init();
void close();
void rasterCircle(int x0, int y0, int radius);
void DrawFlower();
void deleteFlower(int i);
void drawLepestok(float x0, float y0, float R, float n);

#define WINDOW_K	1.2
#define WINDOW_HEIGHT	(int)(1080/WINDOW_K)
#define WINDOW_WIDTH	(int)(1920/WINDOW_K)
#define SIGN(x)		((x >= 0) ? (1) : (-1))

SDL_Window* window;
SDL_Surface* surface;
SDL_Renderer* ren;
SDL_Event e;

typedef struct Flower {
	int x, y;
	int nl;
	int R;
	int r;
} Flower;

Flower* flowers[10000];
int N = 0;

int MAX_N = 200;

#define WHITE	255, 255, 255, 255 //0xFFFFFF
#define BLUE	0, 0, 255, 255 //0x0000FF
#define PINK	255, 192, 203, 255 //0xFFC0CB
#define PURPLE	255, 0, 255, 255 //0x800080
#define ORANGE	255, 165, 0, 255 //0xFFA500
#define GREEN	0, 255, 0, 255 //0x008000
#define RED		255, 0, 0, 255
#define YELLOW	255, 255, 0, 255 //0xFFFF00

#define COLOR_COUNT	7
int pal[COLOR_COUNT][4] = { {BLUE}, {PINK}, {RED}, {PURPLE}, {ORANGE}, {WHITE}, {GREEN} };


int main(int argc, char* argv[])
{
	if (!init())
	{
		printf("Fail to init window");
		return -1;
	}


	SDL_SetRenderDrawColor(ren, GREEN);
	SDL_RenderClear(ren);

	int i = 0;

	while (1)
	{
		if (N == MAX_N)
		{
			for (i = 0; i < 2; i++)
			{				
				deleteFlower(i);
			}

			N = 0;
		}


		SDL_PollEvent(&e);
		if (e.key.key == SDLK_ESCAPE)
		{
			break;
		}

		DrawFlower();

		SDL_RenderPresent(ren);
		SDL_UpdateWindowSurface(window);
	}


	for (i = 0; i < N; i++)
		free(flowers[i]);

	while (SDL_PollEvent(&e) == 0 || e.key.key != SDLK_ESCAPE);

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

	//ren = SDL_CreateRenderer(window, (const char*)0);
	ren = SDL_CreateSoftwareRenderer(surface);


	if (!ren)
	{
		printf("Fail to get render: %s.\n", SDL_GetError());
		return 0;
	}

	//SDL_SetRenderVSync(ren, SDL_RENDERER_VSYNC_DISABLED);


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
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
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

void DrawFlower()
{
	Flower* f = malloc(sizeof(Flower));

	int R, x, y;

	while (1)
	{
		R = (rand() % 40) + 20;
		x = (rand() % (WINDOW_WIDTH - R * 2)) + R;
		y = (rand() % (WINDOW_HEIGHT - R * 2)) + R;

		int i = 0;
		int flag = 1;
		for (; i < MAX_N; i++)
		{
			if (flowers[i] == NULL) continue;
			if (!(sqrt(pow(flowers[i]->x - x, 2) + pow(flowers[i]->y - y, 2)) >= R + flowers[i]->R))
			{
				flag = 0;
				break;
			}
		}

		if (flag)
			break;
	}

	int r = R * 0.25;
	int ic = rand() % (COLOR_COUNT - 1);
	int color[4] = { pal[ic][0],pal[ic][1],pal[ic][2],pal[ic][3] };
	int nl = (rand() % 11) + 4;

	SDL_SetRenderDrawColor(ren, color[0], color[1], color[2], color[3]);
	drawLepestok(x, y, R, nl);


	f->x = x;
	f->y = y;
	f->R = R;
	f->r = r;
	f->nl = nl;

	if (flowers[N] != NULL)
		deleteFlower(N);

	flowers[N] = f;
	N++;

	SDL_SetRenderDrawColor(ren, YELLOW);
	rasterCircle(x, y, r);
}

void drawLepestok(float x0, float y0, float R, float n)
{
	float fi_1 = 0, fi_2 = SDL_PI_F * 2;
	for (; fi_1 < SDL_PI_F * 2; fi_1 += 0.0001f, fi_2 -= 0.0001f)
	{
		float r = R * fabs(sin(fi_1 * (n / 2)));
		float x1 = r * cos(fi_1) + x0;
		float y1 = r * sin(fi_1) + y0;
		SDL_RenderPoint(ren, x1, y1);

		//r = R * fabs(sin(fi_2 * (n / 2)));
		//float x2 = r * cos(fi_2) + x0;
		//float y2 = r * sin(fi_2) + y0;
		//SDL_RenderPoint(ren ,x2, y2);

		SDL_RenderLine(ren, x0, y0, x1, y1);
	}


}

void deleteFlower(int i)
{
	SDL_SetRenderDrawColor(ren, GREEN);
	drawLepestok(flowers[i]->x, flowers[i]->y, flowers[i]->R, flowers[i]->nl);
	rasterCircle(flowers[i]->x, flowers[i]->y, flowers[i]->r);
	free(flowers[i]);
	flowers[i] = NULL;
}


