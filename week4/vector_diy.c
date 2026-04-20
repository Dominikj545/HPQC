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
	int chunk = num_arg / uni_size;
	int* my_chunk = malloc(chunk * sizeof(int));
	int tag = 0;
	if (0 == my_rank)
	{
		// root creates and fills the full vector
		int* full_vector = malloc(num_arg * sizeof(int));
		for (int i = 0; i < num_arg; i++)
		{
			full_vector[i] = i;
		}
		// copies its own chunk
		for (int i = 0; i < chunk; i++)
		{
			my_chunk[i] = full_vector[i];
		}
		// sends each other process its chunk
		for (int dest = 1; dest < uni_size; dest++)
		{
			MPI_Send(full_vector + dest * chunk, chunk, MPI_INT, dest, tag, MPI_COMM_WORLD);
		}
		free(full_vector);
	} // end if (0 == my_rank)
	else
	{
		MPI_Status status;
		MPI_Recv(my_chunk, chunk, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	} // end else
	struct timespec start_time, end_time;
	timespec_get(&start_time, TIME_UTC);
	int local_sum = 0;
	for (int i = 0; i < chunk; i++)
	{
		local_sum += my_chunk[i];
	}
	int total_sum = 0;
	MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	timespec_get(&end_time, TIME_UTC);
	double runtime = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
	if (0 == my_rank)
	{
		printf("Sum: %d\n", total_sum);
		printf("Runtime: %lf seconds\n", runtime);
	} // end if (0 == my_rank)
	free(my_chunk);
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
