#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int check_args(int argc, char **argv);
void check_uni_size(int uni_size);

int main(int argc, char **argv)
{
	int ierror = 0;
	int num_pings = check_args(argc, argv);

	ierror = MPI_Init(&argc, &argv);

	int my_rank, uni_size;
	my_rank = uni_size = 0;
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	check_uni_size(uni_size);

	int counter, count, tag;
	counter = tag = 0;
	count = 1;
	MPI_Status status;

	// time the whole loop not each individual ping
	double start_time = MPI_Wtime();

	while (counter < num_pings)
	{
		if (0 == my_rank)
		{
			MPI_Send(&counter, count, MPI_INT, 1, tag, MPI_COMM_WORLD);
			MPI_Recv(&counter, count, MPI_INT, 1, tag, MPI_COMM_WORLD, &status);
		} // end if (0 == my_rank)
		else
		{
			MPI_Recv(&counter, count, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			// client is the one that increments so the loop actually ends
			counter++;
			MPI_Send(&counter, count, MPI_INT, 0, tag, MPI_COMM_WORLD);
		} // end else
	}

	double end_time = MPI_Wtime();

	if (0 == my_rank)
	{
		double elapsed = end_time - start_time;
		// average time for one ping-pong round trip
		double avg = elapsed / num_pings;
		printf("Pings: %d, Total: %lf s, Average: %e s\n",
				counter, elapsed, avg);
	}

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
		fprintf(stderr, "Correct use: mpirun -np 2 %s [NUM_PINGS]\n", argv[0]);
		exit(-1);
	}
	return num_arg;
}

void check_uni_size(int uni_size)
{
	// pingpong only works with exactly 2 processes
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
