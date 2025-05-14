# WordcountParallel

Este repositório contém quatro versões da aplicação **Word Count** implementadas em C++ com diferentes abordagens de paralelismo. O objetivo do projeto é estudar e comparar o desempenho de técnicas paralelas para contagem de palavras em arquivos de texto.

## Estrutura 

- `sequential/` – Implementação sequencial em C++.
- `openmp/` – Versão paralela com OpenMP (multi-thread).
- `mpi/` – Versão com MPI tradicional.
- `mapreduce/` – Versão com MPI que simula o modelo MapReduce, utilizando apenas `MPI_Send` e `MPI_Recv`.

## 🚀 Como executar
