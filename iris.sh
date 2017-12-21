#!/bin/bash
cd "${0%/*}"

count_max=100
num_of_attributes=4
data_file="datasets/iris-data.txt"
out="iris-results.txt"

echo "Agent has started."
for ((count=1; count <= $count_max; count++)); do
	./scripts/iris-pass.sh "$count"
done

echo "Agent has finished."

paste tmp/out{1..100} > $out
rm tmp/out*
echo "finished."
