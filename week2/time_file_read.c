#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
	// creates and initialises the variables
	struct timespec start_time, end_time, time_diff;
	double runtime = 0.0;
	FILE *in_file;
	char buffer[256];

	// opens the input file for reading
	in_file = fopen("../data/output.txt", "r");

	// checks if the file was opened successfully
	if (NULL == in_file)
	{
		// raises an error
		fprintf(stderr, "ERROR: Could not open ../data/output.txt for reading.\n");
		// and crashes out
		exit(-1);
	}

	// gets the time before the loop
	timespec_get(&start_time, TIME_UTC);
	// reads the file line by line into memory
	while (NULL != fgets(buffer, 256, in_file))
	{
		// does nothing with the data, just reads it
	}
	// gets the time after the loop
	timespec_get(&end_time, TIME_UTC);

	// closes the file
	fclose(in_file);

	// calculates the runtime
	time_diff = calculate_runtime(start_time, end_time);
	runtime = to_second_float(time_diff);
	// outputs the runtime
	printf("\n\nRuntime for file read loop: %lf seconds.\n\n", runtime);
	return 0;
}

double to_second_float(struct timespec in_time)
{
	// creates and initialises the variables
	float out_time = 0.0;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;
	// extracts the elements from in_time
	seconds = in_time.tv_sec;
	nanoseconds = in_time.tv_nsec;
	// calculates the time in seconds
	out_time = seconds + nanoseconds/1e9;
	// returns the time as a double
	return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
	// creates and initialises the variables
	struct timespec time_diff;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;
	// extracts the elements from start_time and end_time
	seconds = end_time.tv_sec - start_time.tv_sec;
	nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
	// if the ns part is negative
	if (nanoseconds < 0)
	{
		// "carry the one!"
		seconds = seconds - 1;
		nanoseconds = ((long int) 1e9) - nanoseconds;
	}
	// creates the runtime
	time_diff.tv_sec = seconds;
	time_diff.tv_nsec = nanoseconds;
	return time_diff;
}
