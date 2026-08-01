#!/usr/bin/env bash
# =============================================================================
# rodar_experimentos.sh
# Executa os quatro binários do Cubo (Sequencial, Paralelo, VNS, VNSParalelo)
# para movimentos de embaralhamento de 1 a 20, com N=10 repetições cada.
#
# Uso:
#   chmod +x rodar_experimentos.sh
#   ./rodar_experimentos.sh [N_REPETICOES] [SEED_BASE]
#
# Saída: resultados.csv  (pronto para importar no preencher_planilha.py)
# =============================================================================

set -euo pipefail

N_REP=${1:-10}        # número de repetições por configuração
SEED_BASE=${2:-42}    # semente base (cada repetição incrementa: seed=SEED_BASE+rep-1)

BINARIO_SEQ="./TesteCuboSequencial"
BINARIO_PAR="./TesteCuboParalelo"
BINARIO_VNS="./TesteCuboVNS"
BINARIO_VNSPAR="./TesteCuboVNSParalelo"

CSV="resultados.csv"
THREADS=(4 8 16 32)

# Verifica que os binários existem
for bin in "$BINARIO_SEQ" "$BINARIO_PAR" "$BINARIO_VNS" "$BINARIO_VNSPAR"; do
    if [[ ! -x "$bin" ]]; then
        echo "ERRO: binário não encontrado ou não executável: $bin"
        echo "Compile com:"
        echo "  g++ -O3 -o TesteCuboSequencial  TesteCuboSequencial.cpp"
        echo "  g++ -O3 -o TesteCuboParalelo    TesteCuboParalelo.cpp    -ltbb"
        echo "  g++ -O3 -o TesteCuboVNS         TesteCuboVNS.cpp"
        echo "  g++ -O3 -o TesteCuboVNSParalelo TesteCuboVNSParalelo.cpp -ltbb"
        exit 1
    fi
done

# Cabeçalho CSV (formato longo: uma linha por execução)
echo "Algoritmo,Threads,Mov,Repeticao,Tempo_s,Fitness,Resolvido,Geracao" > "$CSV"

echo "============================================================"
echo "Iniciando experimentos: MOV=1..20, N=${N_REP} repetições"
echo "Algoritmos: Sequencial, Paralelo(4/8/16/32T), VNS, VNSParalelo(4/8/16/32T)"
echo "Resultados em: $CSV"
echo "============================================================"

# ----------------------------------------------------------
# Função que executa um binário, mede tempo e extrai RESULTADO
# Argumentos: <binario> <mov> <threads_ou_vazio> <seed> <rep>
# ----------------------------------------------------------
run_and_log() {
    local bin="$1"
    local mov="$2"
    local threads="$3"   # "" para algoritmos sequenciais (Sequencial/VNS)
    local seed="$4"
    local rep="$5"

    if [[ -z "$threads" ]]; then
        cmd=("$bin" "$mov" "$seed")
    else
        cmd=("$bin" "$mov" "$threads" "$seed")
    fi

    local t_start t_end tempo resultado
    t_start=$(date +%s%N)
    resultado=$("${cmd[@]}" 2>/dev/null)
    t_end=$(date +%s%N)
    tempo=$(echo "scale=4; ($t_end - $t_start) / 1000000000" | bc)

    local linha_res
    linha_res=$(echo "$resultado" | grep "^RESULTADO")
    if [[ -z "$linha_res" ]]; then
        echo "AVISO: sem linha RESULTADO para bin=$bin mov=$mov rep=$rep" >&2
        return
    fi

    IFS=',' read -r _ algo_campo threads_campo mov_campo fitness_campo resolvido_campo geracao_campo <<< "$linha_res"

    echo "$algo_campo,$threads_campo,$mov,$rep,$tempo,$fitness_campo,$resolvido_campo,$geracao_campo" >> "$CSV"
    echo "  OK: $algo_campo | threads=$threads_campo | mov=$mov | rep=$rep | t=${tempo}s | fit=$fitness_campo | $resolvido_campo"
}

# ----------------------------------------------------------
# Loop principal
# ----------------------------------------------------------
for mov in $(seq 1 20); do
    echo ""
    echo "--- MOV = $mov ---"

    for rep in $(seq 1 "$N_REP"); do
        seed=$(( SEED_BASE + rep - 1 ))

        # Sequencial (GA)
        run_and_log "$BINARIO_SEQ" "$mov" "" "$seed" "$rep"

        # Paralelo (GA + TBB)
        for t in "${THREADS[@]}"; do
            run_and_log "$BINARIO_PAR" "$mov" "$t" "$seed" "$rep"
        done

        # VNS sequencial
        run_and_log "$BINARIO_VNS" "$mov" "" "$seed" "$rep"

        # VNS Paralelo (+ TBB)
        for t in "${THREADS[@]}"; do
            run_and_log "$BINARIO_VNSPAR" "$mov" "$t" "$seed" "$rep"
        done
    done
done

echo ""
echo "============================================================"
echo "Concluído! Arquivo gerado: $CSV"
echo "Execute agora: python3 preencher_planilha.py"
echo "============================================================"
