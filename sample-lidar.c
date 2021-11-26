/*
 * Copyright (c) 2021 Jeff Boody
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	float avg;
	float height;
	int   count;
} cell_sample_t;

int main(int argc, char** argv)
{
	if(argc != 6)
	{
		printf("usage: %s base scale cell_size in.stl out.stl\n", argv[0]);
		return EXIT_FAILURE;
	}

	// typical values are:
	// 2mm base (2.0)
	// scale by half (0.5)
	// 1mm cell size (1.0)
	float base      = strtof(argv[1], NULL);
	float scale     = strtof(argv[2], NULL);
	float cell_size = strtof(argv[3], NULL);

	const char* in  = argv[4];
	const char* out = argv[5];

	FILE* fin = fopen(in, "r");
	if(fin == NULL)
	{
		return EXIT_FAILURE;
	}

	float x;
	float y;
	float z;
	float min_x = 0.0f;
	float min_y = 0.0f;
	float min_z = 0.0f;
	float max_x = 0.0f;
	float max_y = 0.0f;
	float max_z = 0.0f;

	// compute bounding box
	int    idx;
	int    first  = 1;
	float  offset = 0.0f;
	char*  vertex;
	char*  line = NULL;
	size_t n    = 0;
	while(getline(&line, &n, fin) > 0)
	{
		vertex = strstr(line, "vertex");
		if(vertex == NULL)
		{
			continue;
		}

		if(sscanf(vertex, "vertex %f %f %f", &x, &y, &z) != 3)
		{
			continue;
		}

		if(first)
		{
			min_x = x;
			min_y = y;
			min_z = z;
			max_x = x;
			max_y = y;
			max_z = z;
			first = 0;
			continue;
		}

		if(x < min_x)
		{
			min_x = x;
		}
		if(y < min_y)
		{
			min_y = y;
		}
		if(z < min_z)
		{
			min_z = z;
		}
		if(x > max_x)
		{
			max_x = x;
		}
		if(y > max_y)
		{
			max_y = y;
		}
		if(z > max_z)
		{
			max_z = z;
		}
	}
	rewind(fin);
	printf("min: %f, %f, %f\n", min_x, min_y, min_z);
	printf("max: %f, %f, %f\n", max_x, max_y, max_z);

	int w = (int) (scale*(max_x - min_x)/cell_size);
	int d = (int) (scale*(max_y - min_y)/cell_size);
	printf("w=%i, d=%i\n", w, d);

	cell_sample_t* grid;
	grid = (cell_sample_t*)
	       calloc(w*d, sizeof(cell_sample_t));
	if(grid == NULL)
	{
		goto fail_grid;
	}

	// sample the cells
	while(getline(&line, &n, fin) > 0)
	{
		vertex = strstr(line, "vertex");
		if(vertex == NULL)
		{
			continue;
		}

		if(sscanf(vertex, "vertex %f %f %f", &x, &y, &z) != 3)
		{
			continue;
		}

		int xx = (int) (scale*(x - min_x)/cell_size);
		int yy = (int) (scale*(y - min_y)/cell_size);
		if(xx >= w)
		{
			xx = w - 1;
		}
		if(yy >= d)
		{
			yy = d - 1;
		}
		idx = yy*w + xx;

		if(z >= 0.0001f)
		{
			grid[idx].height += z;
			grid[idx].count  += 1;
		}
	}
	rewind(fin);

	// compute the average cell height
	int i;
	int j;
	for(i = 0; i < d; ++i)
	{
		for(j = 0; j < w; ++j)
		{
			idx = i*w + j;
			if(grid[idx].count)
			{
				grid[idx].avg = grid[idx].height/grid[idx].count;
			}

			float avg2 = scale*grid[idx].avg;
			if((grid[idx].avg > 0.0001f) && (avg2 < base))
			{
				float offset2 = base - avg2;
				if(offset2 > offset)
				{
					offset = offset2;
				}
			}
		}
	}

	FILE* fout = fopen(out, "w");
	if(fout == NULL)
	{
		goto fail_fout;
	}

	// output the corrected samples
	while(getline(&line, &n, fin) > 0)
	{
		vertex = strstr(line, "vertex");
		if(vertex == NULL)
		{
			fprintf(fout, "%s", line);
			continue;
		}

		if(sscanf(vertex, "vertex %f %f %f", &x, &y, &z) != 3)
		{
			continue;
		}

		int xx = (int) (scale*(x - min_x)/cell_size);
		int yy = (int) (scale*(y - min_y)/cell_size);
		if(xx >= w)
		{
			xx = w - 1;
		}
		if(yy >= d)
		{
			yy = d - 1;
		}
		idx = yy*w + xx;

		if(z < 0.0001f)
		{
			fprintf(fout, "vertex %f %f 0.0\n",
			        scale*x,
			        scale*y);
		}
		else
		{
			fprintf(fout, "vertex %f %f %f\n",
			        scale*x,
			        scale*y,
			        offset + scale*grid[idx].avg);
		}
	}

	fclose(fout);
	free(grid);
	free(line);
	fclose(fin);

	// success
	return EXIT_SUCCESS;

	// failure
	fail_fout:
		free(grid);
	fail_grid:
	{
		free(line);
		fclose(fin);
	}
	return EXIT_FAILURE;
}
