#!/bin/bash


# ----------------------------------------------------------


# Path to executable file
exe_path="./run.exe"

# Executable arguments
exe_args="words.txt"

# Path to callgrind output file
callgrind_out="profile.out"

# Path to perf stat output file
perf_out="perfomance.txt"


# ----------------------------------------------------------


# For proper shared libraries addresses
LD_BIND_NOW=1

# Callgrind
valgrind --tool=callgrind --callgrind-out-file=$callgrind_out $exe_path $exe_args || exit 1

# Perf
perf stat -o $perf_out $exe_path $exe_args || exit 2

# View callgrind output using KCachegrind
kcachegrind $callgrind_out || exit 3
