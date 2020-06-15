#pragma once
#ifndef Matrix_H
#define Matrix_H
#include <iostream>
#include <mpi.h>
using namespace std;

/* Initialize the storage of the arrays. */
void allocate_arrays(double** local_A, double** local_x, 
	double** local_y, int local_m, int n, int local_n);

/* Generate a random matrix with shape local_m*n. */
void generate_matrix(double local_A[], int local_m, int n);

/* Generate a random vector with shape local_n. */
void generate_vector(double local_x[], int local_n);

/* Compute the result of matrix-vector multiplication. */
void matrix_vector_mul(double local_A[], double local_x[], double local_y[],
	int local_m, int n, int local_n, MPI_Comm comm);

/* Print the result of a matrix. */
void print_matrix(const char title[], double local_A[], int m, int local_m,
	int n, int my_rank, MPI_Comm comm);

/* Print the result of a vector. */
void print_vector(const char title[], double local_x[], int n, int local_n, 
	int my_rank, MPI_Comm comm);
#endif