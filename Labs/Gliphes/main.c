#define _CRT_SECURE_NO_WARNINGS

// Найти все области, статистику глифов:
// размеры глифа, количество пикселей, плотность, связность, диаметр, периметр
// найти похожие глифы путем сравнения глифов одинакового размера.
// Похожие глифы должны содержать не более 10% раличающихся пикселей.
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>
#include <dirent.h>
#include "list.h"
#include <locale.h>

#define WINDOWS


#ifdef WINDOWS



void connectStr(char** dest, char* source1, char* source2, char* source3)
{
	int i, j;
	for (i = 0; i < strlen(source1); i++)
	{
		(*dest)[i] = source1[i];
	}

	for (j = 0; j < strlen(source2); i++, j++)
	{
		(*dest)[i] = source2[j];
	}

	for (j = 0; j < strlen(source3); i++, j++)
	{
		(*dest)[i] = source3[j];
	}

}

int get_all_files(char* pathOnly, char*** filePathes)
{
	NODE* list = NULL;
	int count = 0;
	WIN32_FIND_DATAA FindFileData;
	HANDLE handle;

	int pathOnlyLen = strlen(pathOnly);
	char* path = (char*)malloc(sizeof(char) * (pathOnlyLen + 3));
	path = strcpy(path, pathOnly);
	path[pathOnlyLen] = '\\';
	path[pathOnlyLen + 1] = '*';
	path[pathOnlyLen + 2] = 0;

	handle = FindFirstFileA(path, &FindFileData);

	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("Cant find file:  [%d]\n", GetLastError());
		printf("Path: [%s]", path);
		return -1;
	}

	while (FindNextFileA(handle, &FindFileData)) {
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] == '.') continue;
			int dirNameLen = strlen(FindFileData.cFileName);
			char* pathToNewDir = (char*)malloc(sizeof(char) * (pathOnlyLen + dirNameLen + 2));
			pathToNewDir = strcpy(pathToNewDir, pathOnly);

			int i, j;
			pathToNewDir[pathOnlyLen] = '\\';
			for (i = pathOnlyLen + 1, j = 0; j < dirNameLen; i++, j++)
			{
				pathToNewDir[i] = FindFileData.cFileName[j];
			}
			pathToNewDir[pathOnlyLen + dirNameLen + 1] = 0;

			char** subFileNames = NULL;
			int fileCount = get_all_files(pathToNewDir, &subFileNames);

			for (i = 0; i < fileCount; i++)
			{
				NODE* newNode = (NODE*)malloc(sizeof(NODE));
				char* fileName;
				int namelen = strlen(subFileNames[i]);
				count++;

				if ((fileName = (char*)malloc(sizeof(char) * (namelen + 1))) == NULL)
					perror("Cant to allocate memory");

				fileName[namelen] = 0;
				fileName = strcpy(fileName, subFileNames[i]);

				newNode->data = (void*)fileName;
				add_node(&list, newNode);
				free(subFileNames[i]);
			}
			free(subFileNames);
			continue;
		}

		NODE* newNode = (NODE*)malloc(sizeof(NODE));
		char* fileName;
		int namelen = strlen(FindFileData.cFileName);
		count++;

		if ((fileName = (char*)malloc(sizeof(char) * (namelen + 1))) == NULL)
			perror("Cant to allocate memory");

		fileName[namelen] = 0;
		fileName = strcpy(fileName, FindFileData.cFileName);

		newNode->data = (void*)fileName;
		add_node(&list, newNode);
	}

	FindClose(handle);

	*filePathes = (char**)malloc(sizeof(char*) * count);

	int i = 0;
	for (; i < count; i++)
	{
		char* fileName = (char*)(get_node(&list, i)->data);
		int fileNameLen = strlen(fileName);
		int len = fileNameLen + pathOnlyLen;

		if (strncmp(fileName, pathOnly, pathOnlyLen) != 0)
		{
			char* fullPathFileName = (char*)malloc(sizeof(char) * (len + 2));
			(*filePathes)[i] = (char*)malloc(sizeof(char) * (len + 2));

			fullPathFileName[len + 1] = 0;
			connectStr(&fullPathFileName, pathOnly, "\\", fileName);
			(*filePathes)[i] = strcpy((*filePathes)[i], fullPathFileName);
			free(fullPathFileName);
		}
		else
		{
			(*filePathes)[i] = (char*)malloc(fileNameLen + 1);
			(*filePathes)[i][fileNameLen] = 0;
			(*filePathes)[i] = strcpy((*filePathes)[i], fileName);
		}
	}

	freeList(&list);
	free(path);
	return count;
}

#endif

#define ISBIT(n,x) (((01<<(n))&(x))?1:0)

void outbyte(char byte)
{          // Вывод байта в двоичном виде
	int i;
	for (i = 7; i > -1; i--)
		putchar(ISBIT((i), byte) ? '*' : ' ');
	fflush(stdout);
}

void outbytes(int n, char* byte)
{          // Вывод массива байтов в двоичном виде
	int i;
	for (i = 0; i < n; i++)
		outbyte(byte[i]);
	putchar('\n');
}



typedef struct img { //  структура глифа - картинки
	int w; int h;     // ширина и высота в пикслелях        form file
	int dx;      // расстояние до следующего глифа (если буква) --
	int count;   // черных пикселей всего
	int id;      // иднтификатор - "номер" глифа
	int bytes;   // /количество байтов в битовой карте
	double density; // плотность черных пикселей
	int diam;       // диаметр в манхеттенской метрике  // амые дальние чёрные пиксели
	int perim;      // периметр глифа
	int conn;       // связность
	unsigned char* data;  //  битовая карта (неупакованная)
} IMG;

int popcnt16(unsigned short int i)
{
	return popcnt16(i); //__builtin_popcount(i);
	//   i = i - ((i >> 1) & 0x5555);
	//   i = (i & 0x3333) + ((i >> 2) & 0x3333);
	//   return (((i + (i >> 4)) & 0x0F0F) * 0x0101) >> 8;
}

int popcnt8(unsigned char i)
{
	int count;
	count = 0;
	while (i)
	{
		++count;
		i = (i - 1) & i;
	}
	return count;
}

int popcnt64(unsigned long long w) {
	//  return __builtin_popcountll(w);
	w -= (w >> 1) & 0x5555555555555555ULL;
	w = (w & 0x3333333333333333ULL) + ((w >> 2) & 0x3333333333333333ULL);
	w = (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0fULL;
	return (int)((w * 0x0101010101010101ULL) >> 56);
}

int popcnt32(unsigned int i)
{
	//   return __builtin_popcountl(i);
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}


IMG* init_img(int id, int w, int h)
{             // создание пустого (чистого) глифа с заданными размерами
	IMG* t;
	t = (IMG*)malloc(sizeof(IMG));
	t->w = w; t->h = h; t->dx = 0;
	t->count = 0;
	t->id = id;
	t->bytes = (((w + 7) / 8) * h);
	t->data = (unsigned char*)calloc(t->bytes, 1);
	return t;
}



IMG* load_img(int id, char* File)          // TODO -  считывание из каталога
{
	FILE* F;          // считывание из файла глифа с номером id
	IMG* I;
	I = (IMG*)malloc(sizeof(IMG));
	F = fopen(File, "rb");
	fread(&(I->w), sizeof(int), 1, F);
	fread(&(I->h), sizeof(int), 1, F);
	fread(&(I->dx), sizeof(int), 1, F);
	fread(&(I->count), sizeof(int), 1, F);
	fread(&(I->id), sizeof(int), 1, F);
	fread(&(I->bytes), sizeof(int), 1, F);
	I->data = (unsigned char*)calloc(I->bytes, 1);
	fread(I->data, 1, I->bytes, F);
	fclose(F);
	return I;
}
int xcomp(IMG** a, IMG** b)
{
	return	-(*a)->w + (*b)->w;
}
int ycomp(IMG** a, IMG** b)
{
	return	-(*a)->h + (*b)->h;
}

IMG* G[5000];
int N;
int ISPIX(int argc, int i, int j)
{
	if (i < 0) return 0;
	if (j < 0) return 0;
	if (i >= G[argc]->w) return 0;
	if (j >= G[argc]->h) return 0;

	return
		ISBIT((7 - j % 8), G[argc]->data[i * (G[argc]->bytes / G[argc]->h) + j / 8]);
}


void show(int i);
int getConn(IMG* img, int height, int width);
int** convertImgToArray(IMG* img, int height, int width);
int getPerim(IMG* img, int height, int width);
int getDiam(IMG* img);
int getDensity(IMG* img);
int compareGliphes(const IMG* img1, const IMG* img2);
void sortGliphes(int N);
int sortGliphesByHeight(const IMG* img1, const IMG* img2);

int main(int argc, char* argv[])
{
	long int c;
	int len;
	unsigned char* s;
	unsigned long long* t;
	N = argc - 1;

#ifdef WINDOWS
	setlocale(LC_ALL, "rus");
	char path[1000];
	printf("Enter the directory path:  \n");
	scanf("%999s", path);

	OemToCharA(path, path);

	char** filePathes;
	N = argc = get_all_files(path, &filePathes);


	printf("SCAN FILES AT:  [%s]\n", path);
#endif

	while (argc--)
	{
#ifdef WINDOWS
		G[argc] = load_img(argc, filePathes[argc]);
		printf("Loaded %s\n", filePathes[argc]);
#else
		G[argc] = load_img(argc, argv[argc]);
		printf("Loaded %s\n", argv[argc]);
#endif
		c = 0;
		t = (unsigned long long*)G[argc]->data;
		s = (unsigned char*)G[argc]->data;

		len = G[argc]->bytes;

		for (int i = 0; i < len / 8; i++)
			c += popcnt64(t[i]);

		for (int i = (len / 8) * 8; i < len; i++)
			c += popcnt8(s[i]);

		G[argc]->count = c;


		G[argc]->conn = getConn(G[argc], G[argc]->h, G[argc]->w);
		G[argc]->perim = getPerim(G[argc], G[argc]->h, G[argc]->w);
		G[argc]->diam = getDiam(G[argc]);
		G[argc]->density = getDensity(G[argc]);

	}

	//qsort(G, N, sizeof(IMG*), sortGliphesByHeight);
	sortGliphes(N);



#ifdef WINDOWS
	int l = 0;
	for (l = 0; l < N; l++)
	{
		show(l);
	}
#endif

	printf("%d\n", N);
	getch();
}


void sortGliphes(int N)
{
	NODE* list = NULL;

	int i, j;
	for (i = 0; i < N; i++)
	{
		if (G[i] == NULL) continue;

		for (j = i + 1; j < N; j++)
		{
			if (G[j] == NULL) continue;

			if ((abs(G[i]->w - G[j]->w) <= 1) && (abs(G[i]->h - G[j]->h) <= 1) && (compareGliphes(G[i], G[j]) <= 10))
			{
				NODE* n = (NODE*)malloc(sizeof(NODE));
				n->next = NULL;
				n->data = (void*)(G[j]);
				add_node(&list, n);
				G[j] = NULL;
			}
		}

		NODE* l = (NODE*)malloc(sizeof(NODE));
		l->next = NULL;
		l->data = (void*)(G[i]);
		add_node(&list, l);
		G[i] = NULL;
	}

	for (i = 0; i < N; i++)
	{
		NODE* n = get_node(&list, i);
		G[i] = ((IMG*)(n->data));
	}
	freeListSaveData(&list);
}


int sortGliphesByHeight(const IMG* img1, const IMG* img2)
{
	return img1->h - img2->h;
}

int compareGliphes(const IMG* img1, const IMG* img2)
{
	int mw = min(ceilf((float)(img1->w) / 8), ceilf((float)(img2->w) / 8));
	int offset = abs(ceilf((float)(img1->w) / 8) - ceilf((float)(img2->w) / 8));

	int all = min(img1->bytes, img2->bytes);
	int cnt = 0;
	int i, j, k;
	for (i = 0, j = 0, k = 0; i < all; i++, j++, k++)
	{
		if ((i != 0) && (i % mw == 0))
		{
			if (img1->w < img2->w)
				j += offset;
			else k += offset;
		}

		cnt += popcnt8(img1->data[k] & img2->data[j]);
	}

	return ((cnt * 100) / (min(img1->w, img2->w) * min(img1->h, img2->h)));
}




int possibleWayCount = 4;
int possibleMoves[4][2] = { { -1, 0 },	{ 0, -1 },	{ 1, 0 },	{ 0, 1 } };
void setSection(int*** arr, int h, int w, int x, int y, int from, int to)
{
	if ((x >= 0) && (x < h) && (y >= 0) && (y < w) && ((*arr)[x][y] == from))
		(*arr)[x][y] = to;
	else return;

	int i;
	for (i = 0; i < possibleWayCount; i++)
		setSection(arr, h, w, possibleMoves[i][0] + x, possibleMoves[i][1] + y, from, to);
}

int** convertImgToArray(IMG* img, int height, int width, int x0, int y0)
{
	if (width < 8)
		width += 8;

	height += x0 * 2;
	width += y0 * 2;

	int i, j;
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++)
	{
		arr[i] = (int*)malloc(sizeof(int) * width);
		for (j = 0; j < width; j++)
			arr[i][j] = 0;
	}

	int x, y;
	i = 0;
	j = 0;
	unsigned char* s = (unsigned char*)img->data;
	int len = ((int)(ceil((float)(img->w) / (float)8)));

	for (x = 0; x < img->bytes; x++)
	{
		for (y = 7; y >= 0 && (((x + 1) % len != 0) || (len == 1)); y--)
		{
			int h = x / len + x0;
			int g = ((x * 8) % (len * 8) + 7 - y) + y0;
			arr[h][g] = ((s[x] >> y) & 1) == 1 ? -1 : 0;
		}
	}


	return arr;
}

int getDiam(IMG* img)
{
	int y1 = 0, y2 = 0;

	int** arr = convertImgToArray(img, img->h, img->w, 0, 0);

	int i = 0;
	int j = 0;
	for (j = 0; j < img->h; j++)
		for (i = 0; i < img->w; i++)
		{
			if (arr[0][i] == -1)
			{
				y1 = i;
				j = img->h;
				break;
			}
		}

	for (j = 0; j < img->h; j++)
		for (i = img->w - 1; i >= 0; i--)
		{
			if (arr[img->h - 1][i] == -1)
			{
				y2 = i;
				j = img->h;
				break;
			}
		}

	for (i = 0; i < img->h; i++)
		free(arr[i]);
	free(arr);

	return img->h + (y2 - y1);
}

int getDensity(IMG* img)
{
	int i = 0;
	int n = 0;
	for (i = 0; i < img->bytes; i++)
	{
		n += popcnt8(img->data[i]);
	}

	return n / (img->h * img->w);
}

int getConn(IMG* img, int height, int width)
{
	int i, j, res = 0;
	int** arr = convertImgToArray(img, height, width, 0, 0);


	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (arr[i][j] == -1)
			{
				res++;
				setSection(&arr, height, width, i, j, -1, 0);
			}
		}
	}


	for (i = 0; i < height; i++)
		free(arr[i]);
	free(arr);

	return res;
}

int getPerim(IMG* img, int height, int width)
{
	int i, j, k;
	int** arr = convertImgToArray(img, height, width, 1, 1);
	height += 2;
	width += 2;

	for (j = 0; j < width; j++)
	{
		arr[0][j] = 0;
		arr[height - 1][j] = 0;
	}

	for (i = 0; i < height; i++)
	{
		arr[i][0] = 0;
		arr[i][width - 1] = 0;
	}





	int perim = 0;



	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (arr[i][j] == -1)
			{
				for (k = 0; k < possibleWayCount; k++)
				{
					if (arr[possibleMoves[k][0] + i][possibleMoves[k][1] + j] == 0)
					{
						perim++;
						arr[possibleMoves[k][0] + i][possibleMoves[k][1] + j] = 1;
					}
				}
			}
		}
	}

	/*printf("\n\n");
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			switch (arr[i][j])
			{
			case 1:
				printf("#");
				break;
			case 0:
				printf(" ");
				break;
			case -1:
				printf("*");
				break;
			}

		}
		printf("\n");
	}
	*/


	for (i = 0; i < height; i++)
		free(arr[i]);
	free(arr);

	return perim;
}

void show(int i)
{
	printf("Gliph [%d],\n", i);

	IMG* img = G[i];

	int** arr = convertImgToArray(img, img->h, img->w, 0, 0);

	int x, y;

	for (x = 0; x < img->h; x++)
	{
		for (y = 0; y < img->w; y++)
		{
			putchar(arr[x][y] == -1 ? '*' : ' ');
		}
		putchar('|');
		putchar('\n');
	}



	for (i = 0; i < img->h; i++)
		free(arr[i]);
	free(arr);

	/*int x, y;
	unsigned char* s = (unsigned char*)G[i]->data;
	int len = ((int)(ceilf((float)(img->w) / (float)8)));

	for (x = 0; x < img->bytes; x++)
	{
		for (y = 7; y >= 0 && (((x + 1) % len != 0) || (len == 1)); y--)
		{
			putchar(((s[x] >> y) & 1) == 1 ? '*' : ' ');
		}

		if ((x + 1) % len == 0)
		{
			putchar('|');
			putchar('\n');
		}
	}*/

	// INFO

	printf("w: %d, h: %d\n", img->w, img->h);     // ширина и высота в пикслелях        form file
	printf("dx: %d \n", img->dx);      // расстояние до следующего глифа (если буква) --
	printf("count pixels: %d \n", img->count);   // черных пикселей всего
	printf("id: %d \n", img->id);      // иднтификатор - "номер" глифа
	printf("bytes count: %d \n", img->bytes);   // /количество байтов в битовой карте
	printf("density: %d \n", img->density); // плотность черных пикселей
	printf("diam: %d \n", img->diam);       // диаметр в манхеттенской метрике  // амые дальние чёрные пиксели
	printf("perim: %d \n", img->perim);      // периметр глифа
	printf("conn: %d \n", img->conn);       // связность

}
