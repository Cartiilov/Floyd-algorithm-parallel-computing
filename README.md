# SRIR - Algorytm Floyda

## Spis treści

- [Struktura plików](#struktura-plików)
- [Dane](#dane)
- [Instrukcja obsługi](#instrukcja-obsługi)
- [Przykłady](#przykłady)

## Struktura plików

Do zmiany
```bash
Floyd-algorithm-parallel-computing│
├── src/
│   ├── `mpi_floyd.c`
│   ├── `read_and_distribute_matrix.c`
│   └── `utilities.c`
│
├── include/
│   ├── `read_and_distribute_matrix.c`
│   └── `utilities.h`
│
├── `graph_viz.c` - podstawowy przykład z kodem wizualizującym graf przy pomocy MPI oraz MPE_graphics
└── `Makefile`
```

## Instrukcja obsługi
Aby program zadziałał należy najpierw wykonać komendę:
```bash
source /opt/nfs/config/source_mpich420.sh
```
A następnie
```bash
/opt/nfs/config/station204_name_list.sh 1 16 > nodes
```

Przykładowe wywołanie programu na trzech node 
```bash
mpicc -o alg mpi_working.c -lm && mpiexec -f nodes -n 3 ./alg 4 matrix_diff.txt
```
Plikowi wykonywalnemu (tutaj ./alg) należy dostarczyć dwóch parametrów wejściowych: pierwszym argumentem jest rozmiar macierzy sąsiedztwa definującej graf, a drugim nazwa pliku źródłowego, w którym ta macierz się znajduje.

## Przykłady

> ![Wizualizacja grafu](image.png)
