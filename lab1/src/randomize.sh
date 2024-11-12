#!/bin/bash

: > numbers.txt

for i in {1..150}; do
  number=$(od -An -N2 -i /dev/random | awk '{print $1 % 1000}')
  echo $number >> numbers.txt
done

echo "Файл numbers.txt создан с 150 случайными числами."
