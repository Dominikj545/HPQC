#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void check_uni_size(int uni_size);

int main(int argc, char **argv)
{
	int ierror = 0;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: mpirun -np 2 %s [NUM_PINGS] [ARRAY_SIZE]\n", argv[0]);
		exit(-1);
	}

	int num_pings = atoi(argv[1]);
	int array_size = atoi(argv[2]);

	ierror = MPI_Init(&argc, &argv);

	int my_rank, uni_size;
	my_rank = uni_size = 0;
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	check_uni_size(uni_size);

	// create the array to send back and forth
	int* data = malloc(array_size * sizeof(int));
	for (int i = 0; i < array_size; i++)
	{
		data[i] = i;
	}

	int tag = 0;
	int counter = 0;
	MPI_Status status;

	double start_time = MPI_Wtime();

	while (counter < num_pings)
	{
		if (0 == my_rank)
		{
			MPI_Send(data, array_size, MPI_INT, 1, tag, MPI_COMM_WORLD);
			MPI_Recv(data, array_size, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
		} // end if (0 == my_rank)
		else
		{
			MPI_Recv(data, array_size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Send(data, array_size, MPI_INT, 0, tag, MPI_COMM_WORLD);
		} // end else
		counter++;
	}

	double end_time = MPI_Wtime();

	if (0 == my_rank)
	{
		double elapsed = end_time - start_time;
		double avg = elapsed / num_pings;
		long bytes = (long)array_size * sizeof(int);
		printf("Array: %d ints (%ld bytes), Pings: %d, Avg: %e s\n",
				array_size, bytes, num_pings, avg);
	}

	free(data);
	ierror = MPI_Finalize();
	return 0;
}

void check_uni_size(int uni_size)
{
	if (2 == uni_size)
	{
		return;
	}
	else
	{
		fprintf(stderr, "Pingpong needs exactly 2 processes. Got %d\n", uni_size);
		exit(-1);
	}
}
