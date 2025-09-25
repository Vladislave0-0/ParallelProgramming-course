#!/bin/bash

mpic++ parallel_program.cpp -o parallel_program.x

output_file="./visualization_output/time_results.txt"
echo -n "" > $output_file

for n in {1..8}; do
  if [ $n == 1 ]; then
      echo "Запущено с $n процессом..."
  else
      echo "Запущено с $n процессами..."
  fi
  exec_time=$(mpirun -n $n ./parallel_program.x | grep "Time with" | awk '{print $5}')
  echo "$n $exec_time" >> $output_file
done

echo -e "\n\033[32mРезультаты сохранены в $output_file\033[0m"
