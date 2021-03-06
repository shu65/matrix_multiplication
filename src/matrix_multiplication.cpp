/*
 * matrix_multiplication.cpp
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
#include <string>
#include <assert.h>
#include "matrix_multiplication.h"

#ifdef MKL
#include <mkl.h>
#endif
#ifdef MIC
#include <offload.h>
#include <omp.h>
#endif
std::string GetName(const MatrixMultiplicationImplementation &implementation) {
  switch (implementation) {
  case kCpuReference:
    return std::string("CPU Reference");
    break;
  case kMic:
    return std::string("Mic");
    break;
  default:
    return std::string("");
    break;
  }
  
  return std::string("");
}

int MultiplyMatricesCpuReference(size_t column_a, size_t row_a, float* matrix_a,
				 size_t column_b, size_t row_b, float* matrix_b, float* matrix_c) {
  for (size_t column_a_i = 0; column_a_i < column_a; ++column_a_i) {
    for (size_t row_b_i = 0; row_b_i < row_b; ++row_b_i) {
      float c = 0;
      for (size_t row_a_i = 0; row_a_i < row_a; ++row_a_i) {
	c += matrix_a[column_a_i*row_a + row_a_i]*matrix_b[column_b*row_a_i + row_b_i];
      }
      matrix_c[column_a_i*row_b + row_b_i] = c;
    }
  }
  return 0;
}

#ifdef MIC
int MultiplyMatricesMicImple(size_t column_a, size_t row_a, float* matrix_a,
			   size_t column_b, size_t row_b, float* matrix_b, float* matrix_c) {
  int number_threads = -1;
  int mic_id = 0;
#pragma offload target(mic:mic_id) \
  in(matrix_a:length(column_a*row_a) alloc_if(1) free_if(1)) \
  in(matrix_b:length(column_b*row_b) alloc_if(1) free_if(1)) \
  out(matrix_c:length(column_a*row_b) alloc_if(1) free_if(1))
#pragma omp parallel
  {
    number_threads = omp_get_num_threads();
#pragma omp for
    for (size_t column_a_i = 0; column_a_i < column_a; ++column_a_i) {
      for (size_t row_b_i = 0; row_b_i < row_b; ++row_b_i) {
	float c = 0;
	for (size_t row_a_i = 0; row_a_i < row_a; ++row_a_i) {
	  c += matrix_a[column_a_i*row_a + row_a_i]*matrix_b[column_b*row_a_i + row_b_i];
	}
	matrix_c[column_a_i*row_b + row_b_i] = c;
      } 
    }
  }
  std::cout << "Number threads in MIC : " << number_threads << std::endl;
  return 0;
}
#endif


int MultiplyMatricesMic(size_t column_a, size_t row_a, float* matrix_a,
			   size_t column_b, size_t row_b, float* matrix_b, float* matrix_c) {
#ifdef MIC
  
  int ret = MultiplyMatricesMicImple(column_a, row_a, matrix_a, column_b, row_b, matrix_b, matrix_c);
  return ret;
#else 
    std::cerr << "Error : Can't use Mic" << std::endl;
    return 1;
#endif
}




int MultiplyMatrices(const MatrixMultiplicationImplementation &implementation, size_t column_a, size_t row_a, float* matrix_a,
		     size_t column_b, size_t row_b, float* matrix_b, float* matrix_c) {
  assert(row_a == column_b);
  switch (implementation) {
  case kCpuReference:
    return MultiplyMatricesCpuReference(column_a, row_a, matrix_a, column_b, row_b, matrix_b, matrix_c);
    break;
 case kMic:
    return MultiplyMatricesMic(column_a, row_a, matrix_a, column_b, row_b, matrix_b, matrix_c);
    break;
  default:
    return 1;
    break;
  } 
  return 1;
}
