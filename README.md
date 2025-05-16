# WordcountParallel

Este repositório contém quatro versões da aplicação **Word Count** implementadas em C++ com diferentes abordagens de paralelismo. O objetivo do projeto é estudar e comparar o desempenho de técnicas paralelas para contagem de palavras em arquivos de texto.

## Versões

- `Sequential` – Implementação sequencial em C++.
- `OMP` – Versão paralela com OpenMP.
- `MPI` – Versão com MPI tradicional.
- `MapReduce` – Versão com MPI que simula o modelo MapReduce, utilizando apenas `MPI_Send` e `MPI_Recv`.

## Compilação e execução

- Sequential:
  g++ -o wordcount_sequential sequential/wordcount.cpp
  ./wordcount_sequential input.txt

- OMP:
  g++ -fopenmp -o wordcount_omp openmp/wordcount.cpp
  ./wordcount_omp input.txt

- MPI:
  mpic++ -o wordcount_mpi mpi/wordcount.cpp
  mpirun -np <n_processos> ./wordcount_mpi input.txt

- MapReduce:
  mpic++ -o wordcount_mpimapreduce mpimapreduce/wordcount.cpp
  mpirun -np 7 ./wordcount_mpimapreduce input.txt


