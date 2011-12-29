#!/bin/bash

assert_command_exists () {
	command -v $1 &>/dev/null || { echo "ERROR: I require $1 but it's not installed. Aborting." >&2; exit 1; }
}

assert_command_exists valgrind
assert_command_exists lua

valgrind --leak-check=full lua test_luaxml2_memoryleaks.lua

