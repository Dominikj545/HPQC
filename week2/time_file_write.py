import sys
import time

def main():
    # checks if there are the right number of arguments
    try:
        # converts the first argument to an integer
        in_arg = int(sys.argv[1])
    except:
        # raises an error
        raise Exception("Incorrect arguments.\nUsage: python time_file_write.py [NUM]\ne.g.\npython time_file_write.py 23")

    # opens the output file for writing
    out_file = open("../data/output.txt", "w")

    # gets the start time for the loop
    start_time = time.time()
    # iterates over all numbers up to the input
    for i in range(in_arg):
        # writes the index to the file
        out_file.write("{}, ".format(i))
    # gets the end time for the loop
    end_time = time.time()

    # closes the file
    out_file.close()

    # gets the total time
    run_time = end_time - start_time
    # prints the output
    print("\n\nTime for file write loop: {} seconds\n".format(run_time))

if __name__ == "__main__":
    main()
