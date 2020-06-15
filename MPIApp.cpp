#include <iostream>
#include <time.h>
#include <mpi.h>     /* For MPI Functions, etc */
#include "Matrix.h"
#include "Trap.h"	/* Trap Integral Auxiliary */
#include "OddEvenSorting.h"

void testMatVecMul(int total = 10000, int m = 500, int n = 500);
void testTrapIntegral(int total = 10000, int range = 100, int n = 1000000);
void testOddEvenSorting(int total = 10000, int n = 10000);

int main(int argc, char* argv[])
{
	int command = atoi(argv[1]);

	if (command == 1)
	{
		if (argc == 5)
			testMatVecMul(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
		else if (argc == 4)
			testMatVecMul(10000, atoi(argv[2]), atoi(argv[3]));
		else if (argc == 3)
			testMatVecMul(atoi(argv[2]));
		else if (argc == 2)
			testMatVecMul();
		exit(0);
	}
	else if (command == 2)
	{
		if (argc == 5)
			testTrapIntegral(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
		if (argc == 4)
			testTrapIntegral(10000, atoi(argv[2]), atoi(argv[3]));
		else if (argc == 3)
			testTrapIntegral(atoi(argv[2]));
		else if (argc == 2)
			testTrapIntegral();
		exit(0);
	}
	else if (command == 3)
	{
		if (argc == 4)
			testOddEvenSorting(atoi(argv[2]), atoi(argv[3]));
		else if (argc == 3)
			testOddEvenSorting(10000, atoi(argv[2]));
		else if (argc == 2)
			testOddEvenSorting();
		exit(0);
	}

	printf("Argument Error.\n\n");
	printf("Available Arguments:\n");
	printf("\t1 - Test Matrix Vector Multiplication.\n");
	printf("\t2 - Test Trap Integral.\n\t3 - Test Odd Even Sorting.\n");
	exit(-1);
}  /* main */

void testMatVecMul(int total, int m, int n)
{
	int my_rank, comm_sz;
	double* local_A;
	double* local_x;
	double* local_y;
	int local_m, local_n;
	double start, local_elapse, elapse;

	/* Start up MPI */
	MPI_Init(NULL, NULL);
	/* Get my process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	/* Get the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	if (my_rank == 0)
	{
		printf("Running Matrix Vector Multiplication Test...\n\n");
		printf("Average %d times\nm: %d, n: %d\n\n", total, m, n);
	}
		

	/* Record start time */
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();

	local_m = m / comm_sz;
	local_n = n / comm_sz;

	for (int i = 0; i < total; ++i)
	{
		allocate_arrays(&local_A, &local_x, &local_y, local_m, n, local_n);
		srand(my_rank * (unsigned)time(NULL));
		generate_matrix(local_A, local_m, n);
		generate_vector(local_x, local_n);
		matrix_vector_mul(local_A, local_x, local_y, local_m, n, local_n, MPI_COMM_WORLD);
		/*print_matrix("A", local_A, m, local_m, n, my_rank, MPI_COMM_WORLD);
		print_vector("x", local_x, n, local_n, my_rank, MPI_COMM_WORLD);
		print_vector("y", local_y, m, local_m, my_rank, MPI_COMM_WORLD);*/
	}

	/* Compute elapse time */
	local_elapse = MPI_Wtime() - start;
	MPI_Reduce(&local_elapse, &elapse, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


	if (my_rank == 0)
		printf("MatrixVectorMul\t#Processor: %d\tTotal time: %.6fms\tAverage time: %.6fms\n",
			comm_sz, elapse * 1000, elapse * 1000 / total);

	/* Shut down MPI */
	MPI_Finalize();
}

void testTrapIntegral(int total, int range, int n)
{
	int my_rank, comm_sz, local_n;
	double h, local_a, local_b;
	double local_int, total_int;
	double start, local_elapse, elapse;

	/* Start up MPI */
	MPI_Init(NULL, NULL);
	/* Get my process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	/* Get the number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	/* Initialize the Class Trap */
	auto func = [](double x) {return x * x; };
	Trap trap(func);

	if (my_rank == 0)
	{
		printf("Running Trap Integral Test...\n\n");
		printf("Average %d times\nrange: %d, #trapezoids: %d\n\n", total, range, n);
	}

	/* Record start time */
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();

	srand(1);
	local_n = n / comm_sz;

	for (int time = 0; time < total; ++time)
	{
		double a = rand() % 100;
		double b = a + range;
		h = (b - a) / n;
		local_a = a + my_rank * local_n * h;
		local_b = local_a + local_n * h;

		local_int = trap.integral(local_a, local_b, local_n, h);

		/* Add up the integrals calculated by each process */
		MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

		/* Print the result */
		/*if (my_rank == 0)
		{
			printf("\nWith n = %d trapezoids, our estimate\n", n);
			printf("of the integral from %.4f to %.4f = %f\n", a, b, total_int);
		}*/
	}

	/* Compute elapse time */
	local_elapse = MPI_Wtime() - start;
	MPI_Reduce(&local_elapse, &elapse, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
		printf("Integral func: x*x.\t#Processor: %d\tTotal time: %.6fms\tAverage time: %.6fms\n",
			comm_sz, elapse * 1000, elapse * 1000 / total);

	/* Shut down MPI */
	MPI_Finalize();
}

void testOddEvenSorting(int total, int n)
{
	int my_rank, comm_sz;
	int* local_A;
	int local_n;
	double start, local_elapse, elapse;
	MPI_Comm comm;

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &comm_sz);
	MPI_Comm_rank(comm, &my_rank);

	if (my_rank == 0)
	{
		printf("Running Odd Even Sorting Test...\n\n");
		printf("Average %d times\nlist length: %d\n\n", total, n);
	}

	/* Record start time */
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();

	local_n = n / comm_sz;
	local_A = new int[local_n];

	for (int i = 0; i < total; ++i)
	{
		srand(my_rank * (unsigned)time(NULL));
		// Generate random numbers
		generate_list(local_A, local_n);
		// Sort by odd even sorting
		sort(local_A, local_n, my_rank, comm_sz, comm);
		
		// Print the result
		//print_list(local_A, local_n, my_rank, comm_sz, comm);
	}

	/* Compute elapse time */
	local_elapse = MPI_Wtime() - start;
	MPI_Reduce(&local_elapse, &elapse, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
		printf("Odd Even Sorting\t#Processor: %d\tTotal time: %.6fms\tAverage time: %.6fms\n",
			comm_sz, elapse * 1000, elapse * 1000 / total);

	/* Shut down MPI */
	MPI_Finalize();
}