#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
int check_args(int argc, char **argv);
int main(int argc, char **argv)
{
	int ierror = 0;
	int num_arg = check_args(argc, argv);
	ierror = MPI_Init(&argc, &argv);
	int my_rank, uni_size;
	my_rank = uni_size = 0;
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);
	int* my_vector = malloc(num_arg * sizeof(int));
	for (int i = 0; i < num_arg; i++)
	{
		my_vector[i] = i;
	}
	int chunk = num_arg / uni_size;
	int start = my_rank * chunk;
	int stop = (my_rank + 1) * chunk;
	if (my_rank == uni_size - 1)
	{
		stop = num_arg;
	}
	struct timespec start_time, end_time;
	timespec_get(&start_time, TIME_UTC);
	int local_sum = 0;
	for (int i = start; i < stop; i++)
	{
		local_sum += my_vector[i];
	}
	// gather all partial sums into an array on root
	int* all_sums = NULL;
	if (0 == my_rank)
	{
		all_sums = malloc(uni_size * sizeof(int));
	} // end if (0 == my_rank)
	MPI_Gather(&local_sum, 1, MPI_INT, all_sums, 1, MPI_INT, 0, MPI_COMM_WORLD);
	timespec_get(&end_time, TIME_UTC);
	double runtime = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
	if (0 == my_rank)
	{
		// root loops through and sums
		int total_sum = 0;
		for (int i = 0; i < uni_size; i++)
		{
			total_sum += all_sums[i];
		}
		printf("Sum: %d\n", total_sum);
		printf("Runtime: %lf seconds\n", runtime);
		free(all_sums);
	} // end if (0 == my_rank)
	free(my_vector);
	ierror = MPI_Finalize();
	return 0;
}
int check_args(int argc, char **argv)
{
	int num_arg = 0;
	if (argc == 2)
	{
		num_arg = atoi(argv[1]);
	}
	else
	{
		fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
		fprintf(stderr, "Correct use: mpirun -np N %s [NUMBER]\n", argv[0]);
		exit(-1);
	}
	return num_arg;
}
