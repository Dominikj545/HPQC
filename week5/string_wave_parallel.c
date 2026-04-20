#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// struct to hold all the command line arguments
struct args
{
	int points;
	int cycles;
	int samples;
	char* output_path;
};

struct args check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	int ierror = 0;
	struct args my_args = check_args(argc, argv);

	int points = my_args.points;
	int cycles = my_args.cycles;
	int samples = my_args.samples;
	int time_steps = cycles * samples + 1;
	double step_size = 1.0/samples;

	ierror = MPI_Init(&argc, &argv);

	int my_rank, uni_size;
	my_rank = uni_size = 0;
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// each rank handles a chunk of the string
	int base_chunk = points / uni_size;
	int chunk = base_chunk;
	if (my_rank == uni_size - 1)
	{
		chunk = points - (uni_size - 1) * base_chunk;
	}

	// local positions for this ranks chunk
	double* local = (double*) malloc(chunk * sizeof(double));
	double* local_new = (double*) malloc(chunk * sizeof(double));
	initialise_vector(local, chunk, 0.0);

	// root needs space for the full string and gatherv setup
	double* all_positions = NULL;
	int* recv_counts = NULL;
	int* displacements = NULL;
	FILE* out_file = NULL;

	if (0 == my_rank)
	{
		all_positions = (double*) malloc(points * sizeof(double));

		// work out how many elements to expect from each rank
		recv_counts = (int*) malloc(uni_size * sizeof(int));
		displacements = (int*) malloc(uni_size * sizeof(int));
		for (int r = 0; r < uni_size; r++)
		{
			recv_counts[r] = base_chunk;
			displacements[r] = r * base_chunk;
		}
		// last rank might have more
		recv_counts[uni_size - 1] = points - (uni_size - 1) * base_chunk;

		out_file = fopen(my_args.output_path, "w");
		if (out_file == NULL)
		{
			fprintf(stderr, "ERROR: Could not open file %s\n", my_args.output_path);
			MPI_Abort(MPI_COMM_WORLD, -1);
		}
		print_header(&out_file, points);
	} // end if (0 == my_rank)

	double current_time = 0.0;

	for (int t = 0; t < time_steps; t++)
	{
		current_time = t * step_size;

		// boundary exchange
		double boundary = 0.0;
		int tag = 0;
		int count = 1;
		MPI_Status status;

		if (0 != my_rank)
		{
			MPI_Recv(&boundary, count, MPI_DOUBLE, my_rank - 1, tag, MPI_COMM_WORLD, &status);
		} // end if (0 != my_rank)

		if (my_rank < uni_size - 1)
		{
			MPI_Send(&local[chunk - 1], count, MPI_DOUBLE, my_rank + 1, tag, MPI_COMM_WORLD);
		} // end if (my_rank < uni_size - 1)

		// update this ranks chunk
		if (0 == my_rank)
		{
			local_new[0] = driver(current_time);
		} // end if (0 == my_rank)
		else
		{
			local_new[0] = boundary;
		} // end else

		for (int i = 1; i < chunk; i++)
		{
			local_new[i] = local[i - 1];
		}

		for (int i = 0; i < chunk; i++)
		{
			local[i] = local_new[i];
		}

		// gatherv handles uneven chunk sizes
		MPI_Gatherv(local, chunk, MPI_DOUBLE, all_positions, recv_counts, displacements, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		if (0 == my_rank)
		{
			fprintf(out_file, "%d, %lf", t, current_time);
			for (int j = 0; j < points; j++)
			{
				fprintf(out_file, ", %lf", all_positions[j]);
			}
			fprintf(out_file, "\n");
		} // end if (0 == my_rank)
	} // end for (t = 0; t < time_steps; t++)

	free(local);
	free(local_new);

	if (0 == my_rank)
	{
		fclose(out_file);
		free(all_positions);
		free(recv_counts);
		free(displacements);
	} // end if (0 == my_rank)

	ierror = MPI_Finalize();
	return 0;
}

// prints a header to the file
// double-pointer used to allow this function to move the file pointer
void print_header(FILE** p_out_file, int points)
{
	fprintf(*p_out_file, "#, time");
	for (int j = 0; j < points; j++)
	{
		fprintf(*p_out_file, ", y[%d]", j);
	}
	fprintf(*p_out_file, "\n");
}

// defines a simple harmonic oscillator as the driving force
double driver(double time)
{
	double value = sin(time*2.0*M_PI);
	return(value);
}

// defines a function to initialise all values in a vector to a given inital value
void initialise_vector(double vector[], int size, double initial)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets the elements of the vector to the initial value
		vector[i] = initial;
	}
}

// checks arguments and returns them in a struct
struct args check_args(int argc, char **argv)
{
	struct args my_args;

	if (argc == 5) // points, cycles, samples, output path
	{
		my_args.points = atoi(argv[1]);
		my_args.cycles = atoi(argv[2]);
		my_args.samples = atoi(argv[3]);
		my_args.output_path = argv[4];
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: Wrong number of arguments!\n");
		fprintf(stderr, "Correct use: mpirun -np N %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);

		// and exit COMPLETELY
		exit (-1);
	}
	return my_args;
}
