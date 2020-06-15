#include "Trap.h"
#include <iostream>
#include <mpi.h>

Trap::Trap(double(*func)(double))
{
	f = func;
}

double Trap::integral(double a, double b, int n, double len)
{
	double result = 0.0;
	result = (f(a) + f(b)) / 2.0;
	for (int k = 1; k <= n - 1; k++)
	{
		result += f(a + k * len);
	}
	result = result * len;
	return result;
}
