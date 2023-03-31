#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SEED 69
#define N 1000
#define RAND_MOD 100000000

bool is_prime(int n)
{
	for (int div = 2; div < n; div++)
		if (n % div == 0)
			return false;
	return true;
}

int main(void)
{
	srand(SEED);

	for (int i = 0; i < N; i++)
	{
		int data = rand() % RAND_MOD;
		printf("Main: %d is%s a prime\n", data, is_prime(data) ? "" : " not");
	}

	return 0;
}
