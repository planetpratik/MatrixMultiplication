/*******************************************************************************
*   This example measures performance of computing the real matrix product
*   C=alpha*A*B+beta*C using a triple nested loop, where A, B, and C are
*   matrices and alpha and beta are double precision scalars.

*	In the second part, we are using MKL dgemm function using CBLAS Interface
*	to calculate matrix product. Purpose is to check performance difference
*	between Regular matrix multiplication and SIMD enabled matrix multiplication.

********************************************************************************/

// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2016-2018 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.


#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <mkl.h>
#include "StopWatch.h"

using namespace MatrixMultiplication;
using namespace std::string_literals;

int main()
{
	// Pointers for storing matrices.
	// Matrix A is ( m x p ) matrix whereas matrix B is ( p x n ) matrix. C is resultant matrix.
	double* A, * B, * C;
	double sum;
	int m = 1, p = 10000, n = 10000;
	StopWatch stopwatch;

	std::string normal_multiplication_output_filename = "normal.txt"s;
	std::string parallel_multiplication_output_filename = "parallel.txt"s;
	std::ofstream normal_multiplication_output_file, parallel_multiplication_output_file;
	normal_multiplication_output_file.exceptions(std::ofstream::badbit);
	parallel_multiplication_output_file.exceptions(std::ofstream::badbit);

	std::cout << "Step 1: Allocating memory for matrices aligned on 64-byte boundries for better performance." << std::endl;
	A = reinterpret_cast<double*>(mkl_malloc(m * p * sizeof(double), 64));
	B = reinterpret_cast<double*>(mkl_malloc(p * n * sizeof(double), 64));
	C = reinterpret_cast<double*>(mkl_malloc(m * n * sizeof(double), 64));

	if (A == nullptr || B == nullptr || C == nullptr)
	{
		std::cout << "ERROR: Can't allocate memory for matrices. Aborting..." << std::endl;
		mkl_free(A);
		mkl_free(B);
		mkl_free(C);
		return 1;
	}
	std::cout << "Memory successfully allocated for 3 matrices." << std::endl;

	// Filling some data in A & B matrices while clearing out Matrix C for storing result.

	for (int i = 0; i < (m * p); i++) 
	{
		A[i] = static_cast<double>(i + 1);
	}

	for (int i = 0; i < (p * n); i++) 
	{
		B[i] = static_cast<double>(-i - 3);
	}

	for (int i = 0; i < (m * n); i++)
	{
		C[i] = static_cast<double>(0.0);
	}

	std::cout << "Step 2: Initialized Data in Matrices A & B." << std::endl;

	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "Step 3: Perform matrix multiplication and record elapsed time for 100 iterations" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;

	try
	{
		normal_multiplication_output_file.open(normal_multiplication_output_filename, std::ios::out);
		stopwatch.Reset();
		for (int i = 0; i < 100; ++i)
		{
			stopwatch.Start();
			// Normal Multiplication Logic
			for (int i = 0; i < m; i++) 
			{
				for (int j = 0; j < n; j++) 
				{
					sum = 0.0;
					for (int k = 0; k < p; k++)
					{
						sum += A[p * i + k] * B[n * k + j];
					}
					C[n * i + j] = sum;
				}
			}
			stopwatch.Stop();
			normal_multiplication_output_file << std::to_string(stopwatch.ElapsedMilliseconds().count()) + "\n"s;
		}
		std::cout << "Normal Matrix Multiplication Output File Created Successfully!\n"s << std::endl;
	}
	catch (const std::ofstream::failure&)
	{
		std::cout << "Exception Opening Normal Matrix Multiplication Output File. Try Again!"s;
	}
	normal_multiplication_output_file.close();
	stopwatch.Reset();

	std::cout << "------------------------------------------------------" << std::endl;
	std::cout << "Using Intel SIMD dgemm function via CBLAS interface & matrix transposition" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl;

	std::cout << "Step 4: Perform matrix multiplication and record elapsed time for 100 iterations\n" << std::endl;
	try
	{
		parallel_multiplication_output_file.open(parallel_multiplication_output_filename, std::ios::out);
		stopwatch.Reset();
		for (int i = 0; i < 100; ++i)
		{
			stopwatch.Start();
			// Parallel Multiplication Logic
			cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans,
				m, n, p, 1.0, A, p, B, n, 1.0, C, n);
			stopwatch.Stop();
			parallel_multiplication_output_file << std::to_string(stopwatch.ElapsedMilliseconds().count()) + "\n"s;
		}
		std::cout << "SIMD Matrix Multiplication Output File Created Successfully!\n"s << std::endl;
	}
	catch (const std::ofstream::failure&)
	{
		std::cout << "Exception Opening SIMD Matrix Multiplication Output File. Try Again!"s << std::endl;
	}
	parallel_multiplication_output_file.close();
	stopwatch.Reset();

	std::cout << "Deallocating memory blocks of all matrices" << std::endl;
	mkl_free(A);
	mkl_free(B);
	mkl_free(C);

	return 0;
}
