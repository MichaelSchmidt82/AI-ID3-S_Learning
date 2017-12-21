#!/bin/bash
(cd "${0%/*}" && cd ../)

num_of_attributes=9
data_file="datasets/cancer-data.txt"

cat $data_file | ./scripts/split.sh 10 ./id3 9 >> tmp/out$1
