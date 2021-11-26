About
=====

This tool samples Lidar output files to make them suitable
for 3D printing. By default, the lidar scans may output
geometry with very thin columns which are not printable due
to the minimum wall thickness. This tool samples the STL
model using a grid where each grid cell corresponds to a
column in the 3D print. By ensuring the grid cell size
matches the minimum wall thickness we can ensure the model
is printable.

STL Files
=========

The tool expects STL files in ascii format.

You can use Blender to convert between binary/ascii STL
files.

1. Launch Blender (https://www.blender.org/)
2. Import STL file
3. Export STL file (check the ascii checkbox for ascii).


Build Tool
==========

Run make to build the sample-lidar tool.

	make

Sample Lidar Output Files
=========================

Run the sample-lidar tool and pass the base height (2.0 mm),
scale (0.5) and cell size (1.0 mm).

	./sample-lidar base scale cell_size in.stl out.stl

You may wish to convert back to binary STL files before
uploading to shapeways for 3D printing since binary files
are much smaller.

Screenshot
==========

Here is a screenshot showing the sampled model on the left
and the original on the right.

![alt text](screenshot.jpg?raw=true "Sampled Model Comparison")

License
=======

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
