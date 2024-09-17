#!/bin/bash

# Создаем или перезаписываем файл numbers.txt
: > numbers.txt

# Генерируем 150 случайных чисел и записываем их в файл
for i in {1..150}; do
  # Извлекаем случайные байты из /dev/random и преобразуем их в целое число
  number=$(od -An -N2 -i /dev/random | awk '{print $1 % 1000}')
  echo $number >> numbers.txt
done

echo "Файл numbers.txt создан с 150 случайными числами."
