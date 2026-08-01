#!/usr/bin/env python3
"""
preencher_planilha.py
=====================
Lê o arquivo resultados.csv gerado pelo rodar_experimentos.sh e preenche
a aba "Resultados" da planilha CuboProfundidade_1a20.xlsx (formato longo:
uma linha por execução). A aba "Resumo por Profundidade" usa fórmulas
(AVERAGEIFS/COUNTIFS) que apontam para a coluna inteira, então ela se
atualiza sozinha ao abrir o arquivo — não precisa reprocessar nada aqui.

Uso:
    python3 preencher_planilha.py [resultados.csv] [CuboProfundidade_1a20.xlsx]

Por padrão assume os nomes acima no diretório atual.
"""

import sys
import csv
from pathlib import Path
from openpyxl import load_workbook

CSV_FILE  = sys.argv[1] if len(sys.argv) > 1 else "resultados.csv"
XLSX_FILE = sys.argv[2] if len(sys.argv) > 2 else "CuboProfundidade_1a20.xlsx"

ALGORITMOS_VALIDOS = {"Sequencial", "Paralelo", "VNS", "VNSParalelo"}


def main():
    csv_path  = Path(CSV_FILE)
    xlsx_path = Path(XLSX_FILE)

    if not csv_path.exists():
        print(f"ERRO: arquivo CSV não encontrado: {csv_path}")
        sys.exit(1)
    if not xlsx_path.exists():
        print(f"ERRO: planilha não encontrada: {xlsx_path}")
        sys.exit(1)

    print(f"Carregando planilha: {xlsx_path}")
    wb = load_workbook(str(xlsx_path))
    ws = wb["Resultados"]

    # Descobre a próxima linha livre (mantém dados já existentes, permite
    # rodar o script várias vezes acumulando resultados de baterias diferentes)
    proxima_linha = ws.max_row + 1
    if ws.cell(1, 1).value is None:
        proxima_linha = 2  # planilha só com cabeçalho

    print(f"Lendo resultados: {csv_path}")
    erros = 0
    preenchidos = 0

    with open(csv_path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                algo      = row["Algoritmo"].strip()
                threads   = int(row["Threads"])
                mov       = int(row["Mov"])
                rep       = int(row["Repeticao"])
                tempo     = float(row["Tempo_s"])
                fitness   = float(row["Fitness"])
                resolvido = row["Resolvido"].strip().upper()
                geracao   = int(row["Geracao"])
            except (KeyError, ValueError) as e:
                print(f"  AVISO: linha ignorada ({e}): {row}")
                erros += 1
                continue

            if algo not in ALGORITMOS_VALIDOS:
                print(f"  AVISO: algoritmo desconhecido '{algo}', linha ignorada")
                erros += 1
                continue

            ws.cell(proxima_linha, 1).value = algo
            ws.cell(proxima_linha, 2).value = threads
            ws.cell(proxima_linha, 3).value = mov
            ws.cell(proxima_linha, 4).value = rep
            ws.cell(proxima_linha, 5).value = tempo
            ws.cell(proxima_linha, 6).value = fitness
            ws.cell(proxima_linha, 7).value = resolvido
            ws.cell(proxima_linha, 8).value = geracao
            proxima_linha += 1
            preenchidos += 1

    print(f"\nLinhas adicionadas: {preenchidos} | Erros/avisos: {erros}")
    wb.save(str(xlsx_path))
    print(f"Planilha salva: {xlsx_path}")
    print("\nPronto! A aba 'Resumo por Profundidade' recalcula sozinha ao abrir no Excel/LibreOffice.")


if __name__ == "__main__":
    main()
