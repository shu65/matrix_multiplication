/*
 * main.cpp
 *
 *   Copyright (c) 2013, Shuji Suzuki
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above Copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above Copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <sys/time.h>
#include "matrix_multiplication.h"

struct Options {
	size_t column_a;
	size_t row_a;
	size_t column_b;
	size_t row_b;
	MatrixMultiplicationImplementation implementation;
	unsigned int seed;
};

int ParseOptions(int argc, char** argv, Options *options) {
	options->seed = 2013;
	size_t size = 1024;
	MatrixMultiplicationImplementation implementation = kCpuReference;
	if (argc > 1) {
		size = atoi(argv[1]);
	}
	if (argc > 2) {
		switch (argv[2][0]) {
		case 'r':
			implementation = kCpuReference;
			break;
		default:
			std::cerr << "Error : invalid implementation option " << argv[2]
					<< std::endl;
			return 1;
			break;
		}
	}
	options->column_a = size;
	options->row_a = size;
	options->column_b = size;
	options->row_b = size;
	options->implementation = implementation;
	return 0;
}

double GetTimeSec() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + static_cast<double>(tv.tv_usec) * 1e-6;
}

void SetRandomInit(size_t size, float* data) {
	for (size_t i = 0; i < size; ++i)
		data[i] = rand() / (float) RAND_MAX;
}

void PrintMatrix(size_t column, size_t row, float* matrix) {

	for (size_t column_i = 0; column_i < column; ++column_i) {
		float* matrix_column = matrix + column_i * row;
		for (size_t row_i = 0; row_i < row; ++row_i) {
			std::cout.precision(5);
			std::cout.width(10);
			std::cout << matrix_column[row_i];
		}
		std::cout << std::endl;
	}
}

void PrintDiff(size_t column, size_t row, float *reference_retult_matrix,
		float *selected_result_matrix, int max_error_count,
		float acceptable_range) {
	std::cout << "Acceptable Range : " << acceptable_range << std::endl;
	int error_count = 0;
	for (size_t column_i = 0; column_i < column; ++column_i) {
		for (size_t row_i = 0; row_i < row; ++row_i) {
			float f_diff = fabs(
					reference_retult_matrix[column_i * row + row_i]
							- selected_result_matrix[column_i * row + row_i]);
			if (f_diff > acceptable_range) {
				if (error_count < max_error_count) {
					std::cout << "(\t" << column_i << ",\t" << row_i
							<< ")\tReference="
							<< reference_retult_matrix[column_i * row + row_i]
							<< "\tSelect="
							<< selected_result_matrix[column_i * row + row_i]
							<< "\tDiff=" << f_diff << std::endl;
				} else {
					std::cout << "Over Max errors" << std::endl;
					return;
				}
				error_count++;
			}
		}
	}
	std::cout << "Total Errors = " << error_count << std::endl;
}

int main(int argc, char** argv) {
	Options options;
	ParseOptions(argc, argv, &options);

	srand(options.seed);

	std::vector<float> matrix_a(options.row_a * options.column_a);
	std::vector<float> matrix_b(options.row_b * options.column_b);
	std::vector<float> matrix_c(options.row_b * options.column_a);

	bool check_result = true;

	SetRandomInit(matrix_a.size(), &matrix_a[0]);
	SetRandomInit(matrix_b.size(), &matrix_b[0]);

	//std::cout << "Matrix a" << std::endl;
	//PrintMatrix(options.column_a, options.row_a, &matrix_a[0]);
	std::cout << "Matrix a size=(\t" << options.column_a << ",\t"
			<< options.row_a << ")" << std::endl;
	//std::cout << "Matrix b" << std::endl;
	//PrintMatrix(options.column_b, options.row_b, &matrix_b[0]);
	std::cout << "Matrix b size=(\t" << options.column_b << ",\t"
			<< options.row_b << ")" << std::endl;

	std::cout << "Matrix c size=(\t" << options.column_a << ",\t"
				<< options.row_b << ")" << std::endl;

	std::cout << "Selected implimentation is " << GetName(options.implementation) << std::endl;
	double start_time = GetTimeSec();
	if (MultiplyMatrices(options.implementation, options.column_a,
			options.row_a, &matrix_a[0], options.column_b, options.row_b,
			&matrix_b[0], &matrix_c[0])) {
		std::cerr << "Error : multiply" << std::endl;
	}
	double end_time = GetTimeSec();

	//std::cout << "Matrix c" << std::endl;
	//PrintMatrix(options.column_a, options.row_b, &matrix_c[0]);

	std::cout << "Time : ";
	std::cout.precision(6);
	std::cout.width(10);
	std::cout << end_time - start_time << " sec" << std::endl;

	if (check_result) {
		std::cout << "Checking Result" << std::endl;
		std::vector<float> reference_matrix_c(options.row_b * options.column_a);
		if (MultiplyMatrices(kCpuReference, options.column_a, options.row_a,
				&matrix_a[0], options.column_b, options.row_b, &matrix_b[0],
				&reference_matrix_c[0])) {
			std::cerr << "Error : multiply" << std::endl;
		}
		PrintDiff(options.column_a, options.row_b, &reference_matrix_c[0],
				&matrix_c[0], 100, 1.0e-5f);
	}
}

