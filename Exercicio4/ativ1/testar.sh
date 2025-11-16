#!/bin/bash

# Script de testes para o programa de verificacao de primos
# Testa diferentes configuracoes de threads e limites

echo "=========================================="
echo "  TESTES - POOL DE THREADS (PRIMOS)"
echo "=========================================="

# Compilar o programa
echo -e "\n[1/5] Compilando..."
javac ativ1.java
if [ $? -ne 0 ]; then
    echo "ERRO na compilacao!"
    exit 1
fi
echo "Compilacao OK!"

# Teste 1: Caso pequeno
echo -e "\n[2/5] Teste 1: Caso pequeno (limite=100, threads=2)"
echo "----------------------------------------"
java ativ1 2 100

# Teste 2: Mais threads
echo -e "\n[3/5] Teste 2: Mais threads (limite=1000, threads=4)"
echo "----------------------------------------"
java ativ1 4 1000

# Teste 3: Muitas threads
echo -e "\n[4/5] Teste 3: Muitas threads (limite=5000, threads=8)"
echo "----------------------------------------"
java ativ1 8 5000

# Teste 4: Caso grande
echo -e "\n[5/5] Teste 4: Caso grande (limite=10000, threads=16)"
echo "----------------------------------------"
java ativ1 16 10000

# Comparacao de desempenho
echo -e "\n=========================================="
echo "  COMPARACAO DE DESEMPENHO"
echo "=========================================="
echo -e "\nTestando com limite=50000 e variando threads:\n"

for threads in 1 2 4 8 16; do
    echo "Threads: $threads"
    java ativ1 $threads 50000 | grep "Concorrente:" | awk '{print "  Tempo: " $5}'
done

echo -e "\n=========================================="
echo "  TESTES CONCLUIDOS!"
echo "=========================================="
