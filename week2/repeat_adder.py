import sys

def main():
    output = 0

    try:
        in_arg = int(sys.argv[1])
        multiple = int(sys.argv[2])
    except:
        raise Exception("Incorrect arguments.\nUsage: python repeat_adder.py [NUM] [NUM]\ne.g.\npython repeat_adder.py 2 3")

    for i in range(in_arg):
        output = output + multiple

    print("{}\n".format(output))

if __name__ == "__main__":
    main()
