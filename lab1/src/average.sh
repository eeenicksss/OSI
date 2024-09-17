#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Не переданы аргументы."
  exit 1
fi

sum=0

for arg in "$@"; do
  sum=$((sum + arg))
done

average=$(($sum / $#))

echo "Количество аргументов: $#"
echo "Сумма: $sum"
echo "Среднее арифметическое: $average"
