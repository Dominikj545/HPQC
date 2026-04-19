# Week 2 - Performance and Parallelism

This exercise compares the runtime performance of C and Python across computation and I/O operations. The programs I wrote for Part 4 are modifications of the `time_print` programs from the module repo, adapted to benchmark file writing and reading instead of printing to screen. I also fixed the `repeat_adder.py` script which had mixed tabs and spaces preventing it from running.

## Running the code

The C programs need to be compiled first, and a `data` directory needs to exist for the output files:
```
mkdir -p ~/bin ~/HPQC/data
gcc time_file_write.c -o ~/bin/time_file_write
gcc time_file_read.c -o ~/bin/time_file_read
```

`time_file_write` and `time_file_write.py` both take a single integer argument specifying how many values to write to `../data/output.txt`:
```
~/bin/time_file_write 100000
python3 time_file_write.py 100000
```

`time_file_read` and `time_file_read.py` take no arguments and just read back whatever was written:
```
~/bin/time_file_read
python3 time_file_read.py
```

The data directory is in `.gitignore` to keep generated files out of the repo.

## Results and analysis

I timed each operation using both the `time` command and internal timing (C uses `timespec_get`, Python uses `time.time()`). The internal timing was consistently lower than the wall clock time from `time` because it excludes startup overhead like loading the Python interpreter. All the results below are from internal timing.

| Operation | N | C (s) | Python (s) | Python/C |
|---|---|---|---|---|
| Hello World | - | 0.001 | 0.009 | 9x |
| Repeat adder | 10^8 | 0.072 | 1.993 | 28x |
| Print to screen | 10^3 | 0.000118 | 0.0004 | 3.4x |
| Print to screen | 10^5 | 0.021 | 0.047 | 2.3x |
| File write | 10^5 | 0.005 | 0.018 | 3.6x |
| File read | 10^6 | 0.002 | 0.008 | 4x |

The largest gap between C and Python was in the repeat adder at roughly 28x. This is a purely computational task with no I/O involved so the full cost of Python interpreting each instruction at runtime is visible. I noticed that the first argument is what controls the loop count, so `repeat_adder 100000000 2` is slow while `repeat_adder 2 100000000` finishes almost instantly despite the large second argument.

For printing to screen the gap dropped to around 2-3x. The printing itself goes through system calls handled by the OS, so the language used to initiate them matters less. The same pattern showed up in file I/O where writing 100,000 integers was 3.6x slower in Python and reading was about 4x slower. In both cases the actual disk operations are handled at the OS level so the language overhead only comes from the loop and string formatting.

File reading was noticeably faster than writing in both languages. For the 10^6 integer file, C read it in 0.002 s compared to 0.043 s to write it. This is because reading just loads raw bytes into memory while writing requires formatting each integer as a string before the OS can write it.

The overall pattern is that C outperforms Python in every case but the size of the gap depends on whether the task is computation-bound or I/O-bound. When the bottleneck is pure computation (repeat adder) the difference is ~28x. When the bottleneck involves the OS handling I/O (file operations, printing) it narrows to 3-4x because both languages end up waiting on the same system calls.
