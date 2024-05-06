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
│   ├── `floyd.c`
│   ├── `graph_viz.c` - podstawowy przykład z kodem wizualizującym graf przy pomocy MPI oraz MPE_graphics
│   ├── `vis.c`
│   └── `main.c`
│
├── include/
│   └── `floyd.h`
│
├── `CMakeLists.txt`
└── build/
```

## Instrukcja obsługi
Aby program zadziałał należy przejść do folderu build i wykonać komendę:
```bash
cmake ..
```
A następnie
```bash
make
```

Przykładowe wywołanie programu na trzech node 
```bash
mpiexec -f nodes -n 3 ./myprogram 5 matrix_diff.txt
```
Plikowi wykonywalnemu (tutaj ./myprogram) należy dostarczyć dwóch parametrów wejściowych: pierwszym argumentem jest rozmiar macierzy sąsiedztwa definującej graf, a drugim nazwa pliku źródłowego, w którym ta macierz się znajduje.

## Przykłady

> ![Wizualizacja grafu](image.png)
