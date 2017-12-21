#!/bin/bash
(cd "${0%/*}" && cd ../)

num_of_attributes=4
data_file="datasets/iris-data.txt"

cat $data_file | ./scripts/split.sh 10 ./id3 "$num_of_attributes" >> tmp/out$1
