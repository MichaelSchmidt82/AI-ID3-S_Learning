#!/bin/bash
cd "${0%/*}"

count_max=100
num_of_attributes=9
data_file="datasets/cancer-data.txt"
out="cancer-results.txt"

echo "Agent has started."
for ((count=1; count <= $count_max; count++)); do
	./scripts/cancer-pass.sh "$count"
done

echo "Agent has finished."

paste tmp/out{1..100} > $out
rm tmp/out*
echo "finished."
