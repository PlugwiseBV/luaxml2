#!/bin/bash

assert_command_exists () {
	command -v $1 &>/dev/null || { echo "ERROR: I require $1 but it's not installed. Aborting." >&2; exit 1; }
}

assert_command_exists valgrind
assert_command_exists lua

NUM_RUNS=2500
echo -e "\n\tTesting luaxml2.validateRelaxNG() by running it $NUM_RUNS times, while using valgrind to check for memory leaks.\n"

valgrind --leak-check=full lua test/test_luaxml2.lua "$NUM_RUNS"
