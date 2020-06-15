#include "Matrix.h"
using namespace std;

void allocate_arrays(double** local_A, double** local_x,
	double** local_y, int local_m, int n, int local_n)
{
	*local_A = new double[local_m * n];
	*local_x = new double[local_n];
	*local_y = new double[local_m];
}

void generate_matrix(double local_A[], int local_m, int n)
{
	for (int i = 0; i < local_m; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			// range: -1~1
			local_A[i * n + j] = 2.0 * (static_cast<double>(rand() % 100) / 100.0) - 1.0;
		}
	}
}

void generate_vector(double local_x[], int local_n)
{
	for (int i = 0; i < local_n; ++i)
	{
		// range: -1~1
		local_x[i] = 2.0 * (static_cast<double>(rand() % 100) / 100.0) - 1.0;
	}
}

void matrix_vector_mul(double local_A[], double local_x[], double local_y[],
	int local_m, int n, int local_n, MPI_Comm comm)
{
	double* x = new double[n];

	/* Construct the vector x by gathering from all processors. */
	MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, comm);
	for (int local_i = 0; local_i < local_m; ++local_i)
	{
		local_y[local_i] = 0.0;
		for (int j = 0; j < n; ++j)
		{
			local_y[local_i] += local_A[local_i * n + j] * x[j];
		}
	}
}

void print_matrix(const char title[], double local_A[], int m, int local_m,
	int n, int my_rank, MPI_Comm comm)
{
	int i, j;
	double* A = NULL;

	if (my_rank == 0)
	{
		A = new double[m * n];
		/* Construct the matrix A by gathering from all processors. */
		MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A,
			local_m * n, MPI_DOUBLE, 0, comm);
		printf("\nThe matrix %s:\n", title);
		printf("[");
		for (i = 0; i < m; i++)
		{
			printf(i > 0 ? " [" : "[");
			for (j = 0; j < n; j++)
			{
				printf("%.2f", A[i * n + j]);
				printf(j == n - 1 ? "" : ", ");
			}
			printf(i == m - 1 ? "]]" : "]");
			printf("\n");
		}
	}
	else
	{
		MPI_Gather(local_A, local_m * n, MPI_DOUBLE, A,
			local_m * n, MPI_DOUBLE, 0, comm);
	}
}


void print_vector(const char title[], double local_x[], int n, int local_n,
	int my_rank, MPI_Comm comm)
{
	int i;
	double* x = NULL;

	if (my_rank == 0)
	{
		x = new double[n];
		MPI_Gather(local_x, local_n, MPI_DOUBLE, x,
			local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		printf("\nThe vector %s:\n", title);
		printf("[");
		for (i = 0; i < n; i++)
		{
			printf("%.2f", x[i]);
			printf(i == n - 1 ? "" : ", ");
		}
		printf("]\n");
	}
	else
	{
		MPI_Gather(local_x, local_n, MPI_DOUBLE, x,
			local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
}

