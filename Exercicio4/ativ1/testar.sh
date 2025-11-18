#!/bin/bash

echo "=== Teste de Corretude - Atividade 1 ==="
echo

echo "Compilando..."
javac ativ1.java || exit 1
echo

casos=("2 100" "4 1000" "8 5000" "16 10000" "1 50000" "2 50000" "4 50000" "8 50000" "16 50000" "4 10000000" "8 10000000" "16 10000000")

echo "Executando casos de teste..."
for caso in "${casos[@]}"; do
    echo "Teste: NTHREADS=$(echo $caso | cut -d' ' -f1), LIMITE=$(echo $caso | cut -d' ' -f2)"
    java ativ1 $caso 2>&1 | grep -E "(Sequencial|Concorrente)"
    echo
done

echo "Testes finalizados!"
