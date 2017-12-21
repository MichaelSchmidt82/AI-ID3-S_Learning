#!/bin/bash
(cd "${0%/*}" && cd ../)

N=$1
shift

## Grab data from standard input
cat | shuf > tmp/temp.$$.txt

## All but last N
head -n -${N} tmp/temp.$$.txt > tmp/temp.$$.train.txt

## Only last N
tail -n ${N} tmp/temp.$$.txt > tmp/temp.$$.test.txt

## Run command with split data appended
./$@ tmp/temp.$$.train.txt tmp/temp.$$.test.txt

## Remove temporary files
rm tmp/temp.$$.*
