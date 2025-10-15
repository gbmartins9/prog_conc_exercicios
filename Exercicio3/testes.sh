#!/bin/bash

echo "================================================================"
echo "     BATERIA DE TESTES - PRODUTOR/CONSUMIDOR"
echo "================================================================"
echo ""

# Compila o programa
echo "Compilando produtor_consumidor.c..."
gcc -o produtor_consumidor produtor_consumidor.c -Wall -pthread -lm
if [ $? -ne 0 ]; then
    echo "[ERRO] Falha na compilacao"
    exit 1
fi
echo "[OK] Compilacao bem-sucedida"
echo ""

TOTAL_TESTES=0
TESTES_PASSOU=0
TESTES_FALHOU=0

# Função para executar um teste
executar_teste() {
    local N=$1
    local M=$2
    local THREADS=$3
    local DESCRICAO=$4
    
    TOTAL_TESTES=$((TOTAL_TESTES + 1))
    
    echo "----------------------------------------------------------------"
    echo "Teste #$TOTAL_TESTES: $DESCRICAO"
    echo "Parametros: N=$N, M=$M, Threads=$THREADS"
    echo "----------------------------------------------------------------"
    
    # Executa o programa e captura a saída
    OUTPUT=$(./produtor_consumidor $N $M $THREADS 2>&1)
    EXIT_CODE=$?
    
    # Verifica se executou com sucesso
    if [ $EXIT_CODE -ne 0 ]; then
        echo "[FALHOU] Codigo de saida $EXIT_CODE"
        TESTES_FALHOU=$((TESTES_FALHOU + 1))
        echo ""
        return
    fi
    
    # Verifica se a corretude foi validada
    if echo "$OUTPUT" | grep -q "CORRETUDE VALIDADA"; then
        echo "[PASSOU] Corretude validada"
        TESTES_PASSOU=$((TESTES_PASSOU + 1))
        
        # Extrai estatísticas
        PRIMOS_SEQ=$(echo "$OUTPUT" | grep "Primos (sequencial):" | awk '{print $3}')
        PRIMOS_CONC=$(echo "$OUTPUT" | grep "Primos (concorrente):" | awk '{print $3}')
        VENCEDORA=$(echo "$OUTPUT" | grep "Thread vencedora:" | awk '{print $4}')
        TEMPO=$(echo "$OUTPUT" | grep "Tempo de execução:" | awk '{print $4}')
        
        echo "   Primos encontrados: $PRIMOS_SEQ (seq) = $PRIMOS_CONC (conc)"
        echo "   Thread vencedora: $VENCEDORA"
        echo "   Tempo: $TEMPO segundos"
    else
        echo "[FALHOU] Corretude nao validada"
        TESTES_FALHOU=$((TESTES_FALHOU + 1))
        echo "$OUTPUT" | tail -20
    fi
    
    echo ""
}

# ============================================================
# TESTES BASICOS
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "                    TESTES BASICOS"
echo "----------------------------------------------------------------"
echo ""

executar_teste 10 2 2 "Teste mínimo"
executar_teste 20 5 2 "Teste pequeno"
executar_teste 50 5 3 "Teste médio-pequeno"
executar_teste 100 10 4 "Teste padrão"

# ============================================================
# TESTES COM DIFERENTES TAMANHOS DE BUFFER
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "           TESTES - VARIACAO DE BUFFER"
echo "----------------------------------------------------------------"
echo ""

executar_teste 100 5 4 "Buffer pequeno (M=5)"
executar_teste 100 10 4 "Buffer médio (M=10)"
executar_teste 100 20 4 "Buffer grande (M=20)"
executar_teste 100 25 4 "Buffer muito grande (M=25)"

# ============================================================
# TESTES COM DIFERENTES NUMEROS DE THREADS
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "           TESTES - VARIACAO DE THREADS"
echo "----------------------------------------------------------------"
echo ""

executar_teste 200 10 1 "1 thread consumidora"
executar_teste 200 10 2 "2 threads consumidoras"
executar_teste 200 10 4 "4 threads consumidoras"
executar_teste 200 10 8 "8 threads consumidoras"
executar_teste 200 10 16 "16 threads consumidoras"

# ============================================================
# TESTES COM N GRANDE
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "              TESTES - VALORES GRANDES"
echo "----------------------------------------------------------------"
echo ""

executar_teste 500 25 4 "N=500"
executar_teste 1000 50 6 "N=1000"
executar_teste 2000 100 8 "N=2000"
executar_teste 5000 200 10 "N=5000"

# ============================================================
# TESTES DE CASOS EXTREMOS
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "              TESTES - CASOS EXTREMOS"
echo "----------------------------------------------------------------"
echo ""

executar_teste 100 1 4 "Buffer unitário (M=1)"
executar_teste 100 99 4 "Buffer quase cheio (M=99)"
executar_teste 13 3 5 "N primo, M primo"
executar_teste 101 7 11 "Todos primos"

# ============================================================
# TESTES DE N NAO DIVISIVEL POR M
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "         TESTES - N NAO DIVISIVEL POR M"
echo "----------------------------------------------------------------"
echo ""

executar_teste 100 7 3 "100 % 7 = 2"
executar_teste 100 13 4 "100 % 13 = 9"
executar_teste 97 10 5 "97 % 10 = 7"
executar_teste 123 11 6 "123 % 11 = 2"

# ============================================================
# TESTES DE STRESS
# ============================================================
echo ""
echo "----------------------------------------------------------------"
echo "                TESTES DE STRESS"
echo "----------------------------------------------------------------"
echo ""

executar_teste 10000 500 12 "N=10000 - Stress moderado"
executar_teste 20000 1000 16 "N=20000 - Stress alto"

# ============================================================
# RESUMO FINAL
# ============================================================
echo ""
echo "================================================================"
echo "                    RESUMO DOS TESTES"
echo "================================================================"
echo ""
echo "  Total de testes executados: $TOTAL_TESTES"
echo "  Testes que passaram:        $TESTES_PASSOU"
echo "  Testes que falharam:        $TESTES_FALHOU"
echo ""

if [ $TESTES_FALHOU -eq 0 ]; then
    echo "  TODOS OS TESTES PASSARAM"
    echo ""
    
    # Limpeza do executavel
    echo "Limpando arquivos temporarios..."
    rm -f produtor_consumidor
    echo "Limpeza concluida"
    echo ""
    exit 0
else
    TAXA_SUCESSO=$((TESTES_PASSOU * 100 / TOTAL_TESTES))
    echo "  Taxa de sucesso: $TAXA_SUCESSO%"
    echo ""
    
    # Limpeza do executavel
    echo "Limpando arquivos temporarios..."
    rm -f produtor_consumidor
    echo ""
    exit 1
fi
