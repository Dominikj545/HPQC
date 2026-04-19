import time

def main():
    # opens the input file for reading
    in_file = open("../data/output.txt", "r")

    # gets the start time for the loop
    start_time = time.time()
    # reads the entire file into memory
    data = in_file.read()
    # gets the end time for the loop
    end_time = time.time()

    # closes the file
    in_file.close()

    # gets the total time
    run_time = end_time - start_time
    # prints the output
    print("\n\nTime for file read: {} seconds\n".format(run_time))

if __name__ == "__main__":
    main()
