#!/usr/bin/env bash

for var in ./*.ir; do
	printf "\x1b[31mTest file: $var\n\x1b[90m"
	cat $var
	printf "\x1b[0m\n"
	valgrind ./a.out < $var
	read -s -p "WAIT > "
	echo 
done
