#!/bin/bash

echo "=== Teste de Corretude - Atividade 3 ==="
echo

# Compila
echo "Compilando..."
javac ativ3.java || exit 1
echo

# Array de casos de teste: "N NTHREADS"
casos=("20 1" "20 3" "100 2" "100 5" "500 4" "500 10" "1000 1" "1000 6" "2000 8" "5000 2" "5000 12" "10000 4")

echo "Executando casos de teste..."
for caso in "${casos[@]}"; do
    echo "Teste: N=$(echo $caso | cut -d' ' -f1), NTHREADS=$(echo $caso | cut -d' ' -f2)"
    java ativ3 $caso 2>&1 | grep -E "(Sequencial|Concorrente)"
    echo
done

echo "Testes finalizados!"