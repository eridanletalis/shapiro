// ConsoleApplication7.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <iomanip>

int createReport(double *x1, double *y1, double *x2, double *y2, double *x3, double  *y3);
int detectSize(int *x, int *y, int *mode, int *deph, int *pos, char *filename);
int restoreIMG(char * img);
int pgm2txt(int *x, int *y, int *mode, int *deph, int *pos, int **PGM, char *filename, const char *output);
int fillPGM(int * size_x, int * size_y, int *pos, int **PGM, char *filename);

using namespace std;

int main(int argc, char* argv[])
{
	if (argc <= 1 || argc > 3)
	{
		printf("The command format is not correct.\n");
		printf("You entered %d argument(s)\n", argc);
		printf("Please, use format: \n");
		printf("\tsolver -generate P, for generate new random triangle (P is pixels noise probability from 0.0 to 1.0\n");
		printf("or\n");
		printf("\tsolver -restore <img_name.pgm>, for definition of triangle vertices.\n");
		return 1;
	}
	if (strcmp(argv[1], "-generate") == 0)
	{
		//generateIMG((double)atof(argv[2]));
		return 0;
	}
	if (strcmp(argv[1], "-restore") == 0)
	{

		restoreIMG(argv[2]);
		return 0;
	}
	return 1;

}

int detectSize(int *x, int *y, int *mode, int *deph, int *pos, char *filename)
{
	std::ifstream file;
	file.open(filename);
	if (!file) return -1;
	char * buffer;
	buffer = new char[128];
	int flag = 0;
	while (*x == 0 || *y == 0 || *mode == -1 || *deph == 0)
	{
		file.getline(buffer, 128, '\n');
		if (buffer[0] == '#') continue;

		if (buffer[0] == 'P')
		{
			if ((int)buffer[1] > 47 && (int)buffer[1] < 58)
			{	
				*mode = buffer[1] - '0';
				(*pos)++;
				file.getline(buffer, 128, '\n');
				while (buffer[0] == '#')
				{
					file.getline(buffer, 128, '\n');
					(*pos)++;
				}
				char * buf;
				char * context;
				buf = strtok_s(buffer, " ", &context);
				*x = atoi(buf);
				buf = strtok_s(NULL, " ", &context);
				*y = atoi(buf);
				(*pos)++;
				file.getline(buffer, 128, '\n');
				while (buffer[0] == '#')
				{
					file.getline(buffer, 128, '\n');
					(*pos)++;
				}
				*deph = atoi(buffer);
				(*pos)++;
				continue;
			}
			else
			{
				file.close();
				delete[] buffer;
				return -1;
			}
		}
		else
		{
			file.close();
			delete[] buffer;
			return -1;
		}
	}
	delete[] buffer;
	return 0;
}

int restoreIMG(char * img)
{
	
	int size_x = 0;
	int size_y = 0;
	int mode = -1;
	int deph = 0;
	int pos = 0;
	detectSize(&size_x, &size_y, &mode, &deph, &pos, img);
	int **PGM;
	PGM = new int*[size_y];
	fillPGM(&size_x, &size_y, &pos, PGM, img);
	double  x1 = 0.0;
	double  y1 = 0.0;
	double  x2 = 0.0;
	double  y2 = 0.0;
	double  x3 = 0.0;
	double  y3 = 0.0;
	pgm2txt(&size_x, &size_y, &mode, &deph, &pos, PGM, img, "output.txt");
	#pragma omp parallel for
	for (int i = 0; i < size_y; i++)
		delete[] PGM[i];
	delete[] PGM;
	return 0;
}

int createReport(double *x1, double *y1, double *x2, double *y2, double *x3, double  *y3)
{

	return 0;
}

int pgm2txt(int *x, int *y, int *mode, int *deph, int *pos, int **PGM, char *filename, const char *output)
{
	std::ofstream out;
	out.open(output);
	if (out.is_open())
	{
		for (int r = 0; r < *y; r++)
		{
			for (int c = 0; c < *x; c++)
				out << PGM[r][c] << " ";
			out << std::endl;
		}


		out.close();
	}

	return 0;
}

int fillPGM(int * x, int * y, int *pos, int **PGM, char *filename)
{
	std::ifstream file;
	file.open(filename);
	if (!file) return -1;
	char * buffer;
	buffer = new char[128];
	int flag = *pos;
	buffer = new char[*x * 10];
	while (flag)
	{
		file.getline(buffer, *x * 10, '\n');
		flag--;
	}
	int row = *y;
	while (row)
	{
		file.getline(buffer, *x * 10, '\n');
		PGM[*y - row] = new int[*x];
		int col = *x;
		char * buf;
		char * context;
		buf = strtok_s(buffer, " ", &context);
		while (buf && 1 + col)
		{
			int num = atoi(buf);
			PGM[*y - row][*x - col] = num;
			buf = strtok_s(NULL, " ", &context);
			col--;
		}
		row--;
	}
	return 0;
}