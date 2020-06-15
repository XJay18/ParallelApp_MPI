#include "OddEvenSorting.h"

void generate_list(int* local_A, int local_n)
{
	for (int i = 0; i < local_n; i++)
	{
		local_A[i] = rand();
	}
}

void sort(int* local_A, int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
	int phase;
	int* temp_B = new int[local_n];
	int* temp_C = new int[local_n];
	int even_partner;
	int odd_partner;

	// odd phase
	if (my_rank % 2 != 0)
	{
		even_partner = my_rank - 1;
		odd_partner = my_rank + 1;
		if (odd_partner == comm_sz) odd_partner = MPI_PROC_NULL;
	}
	// even phase
	else
	{
		even_partner = my_rank + 1;
		odd_partner = my_rank - 1;
		if (even_partner == comm_sz) even_partner = MPI_PROC_NULL;
	}

	// Use quick sort to sort the array locally
	qsort(local_A, local_n, sizeof(int), compare);

	for (phase = 0; phase < comm_sz; phase++)
	{
		// Execute one iteration of odd even sorting.
		odd_even_iter(local_A, temp_B, temp_C, local_n, phase,
			even_partner, odd_partner, my_rank, comm_sz, comm);
	}
}

int compare(const void* a_p, const void* b_p)
{
	int a = *((int*)a_p);
	int b = *((int*)b_p);

	if (a < b)
		return -1;
	if (a == b)
		return 0;
	/* a > b */
	return 1;
}

void odd_even_iter(int* local_A, int* temp_B, int* temp_C, 
	int local_n, int phase, int even_partner, int odd_partner, 
	int my_rank, int comm_sz, MPI_Comm comm)
{
	MPI_Status status;

	if (phase % 2 == 0) {
		if (even_partner >= 0) {
			MPI_Sendrecv(local_A, local_n, MPI_INT,
				even_partner, 0,
				temp_B, local_n, MPI_INT,
				even_partner, 0, comm, &status);
			if (my_rank % 2 != 0)
				merge_high(local_A, temp_B, temp_C, local_n);
			else
				merge_low(local_A, temp_B, temp_C, local_n);
		}
	}
	else { /* odd phase */
		if (odd_partner >= 0) {
			MPI_Sendrecv(local_A, local_n, MPI_INT,
				odd_partner, 0,
				temp_B, local_n, MPI_INT,
				odd_partner, 0, comm, &status);
			if (my_rank % 2 != 0)
				merge_low(local_A, temp_B, temp_C, local_n);
			else
				merge_high(local_A, temp_B, temp_C, local_n);
		}
	}
}

void merge_low(int* local_A, int* temp_B, int* temp_C, int local_n)
{
	int m_i, r_i, t_i;
	m_i = r_i = t_i = 0;
	while (t_i < local_n)
	{
		if (local_A[m_i] <= temp_B[r_i]) {
			temp_C[t_i] = local_A[m_i];
			t_i++; m_i++;
		}
		else {
			temp_C[t_i] = temp_B[r_i];
			t_i++; r_i++;
		}
	}

	memcpy(local_A, temp_C, local_n * sizeof(int));
}

void merge_high(int* local_A, int* temp_B, int* temp_C, int local_n)
{
	int ai, bi, ci;

	ai = local_n - 1;
	bi = local_n - 1;
	ci = local_n - 1;
	while (ci >= 0) {
		if (local_A[ai] >= temp_B[bi]) {
			temp_C[ci] = local_A[ai];
			ci--; ai--;
		}
		else {
			temp_C[ci] = temp_B[bi];
			ci--; bi--;
		}
	}

	memcpy(local_A, temp_C, local_n * sizeof(int));
}

void print_list(int* local_A, int local_n, int my_rank, int comm_sz, MPI_Comm comm)
{
	int i, n;
	n = comm_sz * local_n;
	int* A = new int[n];

	if (my_rank == 0)
	{

		MPI_Gather(local_A, local_n, MPI_INT, A,
			local_n, MPI_INT, 0, comm);
		printf("List:\n");
		printf("[");
		for (i = 0; i < n; i++)
		{
			printf("%d", A[i]);
			printf(i == n - 1 ? "" : ", ");
		}
		printf("]\n");
	}
	else
	{
		MPI_Gather(local_A, local_n, MPI_INT, A,
			local_n, MPI_INT, 0, comm);
	}
}
