#!/bin/bash
# ------------------------------
# Скрипт для замера ускорения MPI-программы
# ------------------------------

SEQ_EXEC=./var.out              # последовательная версия
PAR_EXEC=./var_par.out          # MPI версия
OUTPUT=./results/variant_3a/data_speedup.csv         # CSV с результатами

PROCESSES_LIST="2 4 6 8"

echo "p,T_seq,T_par,Speedup,Efficiency" > $OUTPUT

# Выполним один прогон последовательной версии
echo "Запуск последовательной версии..."
T_SEQ=$($SEQ_EXEC | grep "Sequential time" | awk '{print $3}')

echo "T_seq = $T_SEQ sec"

# Тестируем MPI версию с разным числом процессов
for p in $PROCESSES_LIST; do
  echo "Запуск MPI с $p процессами..."
  T_PAR=$(mpirun -np $p $PAR_EXEC | grep "MPI parallel time" | awk '{print $4}')
  
  SPEEDUP=$(awk -v t1="$T_SEQ" -v tp="$T_PAR" 'BEGIN {print t1/tp}')
  EFFICIENCY=$(awk -v s="$SPEEDUP" -v p="$p" 'BEGIN {print s/p}')
  
  echo "$p,$T_SEQ,$T_PAR,$SPEEDUP,$EFFICIENCY" | tee -a $OUTPUT
done

echo "Результаты сохранены в $OUTPUT"