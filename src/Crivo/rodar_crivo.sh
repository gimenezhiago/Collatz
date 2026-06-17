#!/bin/bash
# =============================================================================
# rodar_crivo.sh
# Compila e executa TesteIntervaloCrivoImpar (BOOL+OpenMP) e
# TesteIntervaloCrivoImparBITSET (BITSET+OpenMP) com GMP, para múltiplos
# valores de N de 10^8 até 10^12, com múltiplas rodadas.
#
# Gera:
#   resultados.txt          — TSV acumulativo de todas as rodadas
#   resultados_<N>.json     — JSON por N completado
#   Crivo_preenchido.xlsx   — Planilha atualizada após cada N
# =============================================================================

set -euo pipefail

SRC_BOOL="TesteIntervaloCrivoImpar.c"
SRC_BITSET="TesteIntervaloCrivoImparBITSET.c"
BIN_BOOL="./TesteIntervaloCrivoImpar"
BIN_BITSET="./TesteIntervaloCrivoImparBITSET"

VALORES_N=(100000000 1000000000 10000000000 100000000000 1000000000000)
RODADAS=10
OMP_THREADS=8
SAIDA="resultados.txt"
XLSX_IN="Crivo_organizado.xlsx"
XLSX_OUT="Crivo_preenchido.xlsx"

echo "=== Compilando ==="
gcc -O3 -fopenmp "$SRC_BOOL"   -o "$BIN_BOOL"   -lm -lgmp
echo "  [OK] $BIN_BOOL"
gcc -O3 -fopenmp "$SRC_BITSET" -o "$BIN_BITSET" -lm -lgmp
echo "  [OK] $BIN_BITSET"

echo "# programa N rodada tempo_s primos marcacoes mem_kb" > "$SAIDA"

run_programa() {
    local prog="$1"
    local N="$2"
    local rodada="$3"

    local tmpout
    tmpout=$(mktemp)

    local t_start t_end tempo
    t_start=$(date +%s%N)
    OMP_NUM_THREADS="$OMP_THREADS" "$prog" "$N" > "$tmpout"
    t_end=$(date +%s%N)
    tempo=$(echo "scale=6; ($t_end - $t_start) / 1000000000" | bc)

    local primos marcacoes mem_kb
    primos=$(grep    "^PRIMOS="      "$tmpout" | cut -d= -f2)
    marcacoes=$(grep "^MARCACOES="  "$tmpout" | cut -d= -f2)
    mem_kb=$(grep    "^MEM_KB="     "$tmpout" | cut -d= -f2)
    rm -f "$tmpout"

    local nome
    nome=$(basename "$prog")
    echo "$nome $N $rodada $tempo $primos $marcacoes $mem_kb" | tee -a "$SAIDA"
}

gerar_json_n() {
    local N="$1"
    local arquivo_json="resultados_${N}.json"

    python3 - "$SAIDA" "$N" "$arquivo_json" << 'PYEOF'
import sys, json

arq_res  = sys.argv[1]
N_alvo   = int(sys.argv[2])
arq_json = sys.argv[3]

dados = {"N": N_alvo, "BOOL": {}, "BITSET": {}}

with open(arq_res) as f:
    for linha in f:
        linha = linha.strip()
        if not linha or linha.startswith("#"):
            continue
        partes = linha.split()
        if len(partes) < 7:
            continue
        prog, N, rodada, tempo, primos, marcacoes, mem_kb = partes[:7]
        if int(N) != N_alvo:
            continue
        tipo = "BOOL" if "BITSET" not in prog.upper() else "BITSET"
        dados[tipo][int(rodada)] = {
            "tempo_s":   float(tempo),
            "primos":    int(primos),
            "marcacoes": int(marcacoes),
            "mem_mb":    round(float(mem_kb) / 1024.0, 3)
        }

with open(arq_json, "w") as f:
    json.dump(dados, f, indent=2)
print(f"  [JSON] {arq_json} salvo.")
PYEOF
}

echo ""
echo "=== Executando testes ==="
echo ""

for N in "${VALORES_N[@]}"; do
    echo "--- N = $N ---"

    for rodada in $(seq 1 "$RODADAS"); do
        echo -n "  BOOL   rodada $rodada ... "
        run_programa "$BIN_BOOL" "$N" "$rodada"
    done

    for rodada in $(seq 1 "$RODADAS"); do
        echo -n "  BITSET rodada $rodada ... "
        run_programa "$BIN_BITSET" "$N" "$rodada"
    done

    gerar_json_n "$N"

    echo "  [XLSX] Atualizando $XLSX_OUT ..."
    python3 preencher_tabela.py "$SAIDA" "$XLSX_IN" "$XLSX_OUT"

    echo ""
done

echo "=== Resultados salvos em: $SAIDA | $XLSX_OUT ==="
