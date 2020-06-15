#pragma once
#ifndef OddEvenSorting_H
#define OddEvenSorting_H
#include <iostream>
#include <mpi.h>

/* Initialize the storage of the local list. */
void generate_list(int* local_A, int local_n);

/* Sort local list, use odd even sorting to sort the global list. */
void sort(int* local_A, int local_n, int my_rank, int comm_sz, MPI_Comm comm);

/* Compare 2 ints, return -1, 0, or 1, respectively, when the first int
 * is less than, equal, or greater than the second. */
int compare(const void* a_p, const void* b_p);

/* Execute one iteration of odd even sorting. */
void odd_even_iter(int* local_A, int* temp_B, int* temp_C,
	int local_n, int phase, int even_partner, int odd_partner,
	int my_rank, int comm_sz, MPI_Comm comm);

/* Merge the smallest local_n elements in local_A and temp_B into temp_C.
 * Then copy temp_C back into local_A. */
void merge_low(int* local_A, int* temp_B, int* temp_C, int local_n);

/* Merge the largest local_n elements in local_A and temp_B into temp_C.
 * Then copy temp_C back into local_A. */
void merge_high(int* local_A, int* temp_B, int* temp_C, int local_n);

/* Print the result of a list. */
void print_list(int* local_A, int local_n, int my_rank, int comm_sz, MPI_Comm comm);

#endif
