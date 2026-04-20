#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// struct to hold all the command line arguments
struct args
{
	int points;
	int cycles;
	int samples;
	char* output_path;
};

// declares the functions that will be called within main
struct args check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	// get all arguments from the command line
	struct args my_args = check_args(argc, argv);

	int points = my_args.points;
	int cycles = my_args.cycles;
	int samples = my_args.samples;
	int time_steps = cycles * samples + 1; // total timesteps
	double step_size = 1.0/samples;

	// spring model parameters as suggested in the assignment
	double k = 50.0;
	double mass = 1.0;
	double damping = 0.995;

	// creates a vector for the time stamps in the data
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	for (int i = 0; i < time_steps; i++)
	{
		time_stamps[i] = i * step_size;
	}

	// creates vectors for position and velocity
	double* positions = (double*) malloc(points * sizeof(double));
	double* velocities = (double*) malloc(points * sizeof(double));
	// and initialises every element to zero
	initialise_vector(positions, points, 0.0);
	initialise_vector(velocities, points, 0.0);

	// creates a file at the path specified by the user
	FILE* out_file;
	out_file = fopen(my_args.output_path, "w");
	if (out_file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open file %s\n", my_args.output_path);
		exit(-1);
	}
	print_header(&out_file, points);

	// iterates through each time step in the collection
	for (int t = 0; t < time_steps; t++)
	{
		// first point is driven by the sin wave
		positions[0] = driver(time_stamps[t]);
		velocities[0] = 0.0;

		// last point is fixed at zero (clamped end)
		positions[points - 1] = 0.0;
		velocities[points - 1] = 0.0;

		// update middle points using spring force f = -kx
		// each point is pulled towards both neighbors
		for (int i = 1; i < points - 1; i++)
		{
			double force = k * (positions[i - 1] + positions[i + 1] - 2.0 * positions[i]);
			double accel = force / mass;
			velocities[i] += accel * step_size;
			velocities[i] *= damping;
			positions[i] += velocities[i] * step_size;
		}

		// prints an index and time stamp
		fprintf(out_file, "%d, %lf", t, time_stamps[t]);

		// iterates over all of the points on the line
		for (int j = 0; j < points; j++)
		{
			// prints each y-position to a file
			fprintf(out_file, ", %lf", positions[j]);
		}
		// prints a new line
		fprintf(out_file, "\n");
	}

	// if we use malloc, must free when done!
	free(time_stamps);
	free(positions);
	free(velocities);

	// closes the file
	fclose(out_file);

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
		fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_FILE]\n", argv[0]);

		// and exit COMPLETELY
		exit (-1);
	}
	return my_args;
}
