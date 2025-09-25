#!/usr/bin/env bash

set -eo pipefail

if [ ! -x "../build/polygon_area" ]; then
    echo "Ошибка: ./polygon_area не найден."
    exit 1
fi

echo "coordinates_num       non_parallel_time      parallel_time"

for (( exp=0; exp<=7; exp++ )); do
    num=$((10**exp))
    ../build/polygon_area "$num"
done

for (( num=0; num<=10000000; num += 100000 )); do
    ../build/polygon_area "$num"
done
