#!/bin/bash
# ------------------------------
# Скрипт для замера ускорения OpenMP-программы
# ------------------------------

SEQ_EXEC=./var.out              # последовательная версия
PAR_EXEC=./var_par.out          # параллельная версия
OUTPUT=./results/variant_1a/data_speedup.csv         # CSV с результатами

THREADS_LIST="2 4 8"

echo "p,T_seq,T_par,Speedup,Efficiency" > $OUTPUT

# Выполним один прогон последовательной версии
T_SEQ=$($SEQ_EXEC | grep "Sequential time" | awk '{print $3}')

echo "T_seq = $T_SEQ sec"

# Теперь тестируем параллельную версию с разным числом потоков
for p in $THREADS_LIST; do
  export OMP_NUM_THREADS=$p
  T_PAR=$($PAR_EXEC | grep "Parallel time" | awk '{print $3}')
  
  SPEEDUP=$(awk -v t1="$T_SEQ" -v tp="$T_PAR" 'BEGIN {print t1/tp}')
  EFFICIENCY=$(awk -v s="$SPEEDUP" -v p="$p" 'BEGIN {print s/p}')
  
  echo "$p,$T_SEQ,$T_PAR,$SPEEDUP,$EFFICIENCY" | tee -a $OUTPUT
done
