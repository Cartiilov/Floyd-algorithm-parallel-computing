# SRIR - Algorytm Floyda

## Spis treści

- [Struktura plików](#struktura-plików)
- [Dane](#dane)
- [Instrukcja obsługi](#instrukcja-obsługi)

## Struktura plików

```bash
Floyd-algorithm-parallel-computing│
├── src/
│   └── `upc_floyd.c`
└── build/
```

## Instrukcja obsługi

Abu uruchomić program należy przejść do folderu build i wykonać kolejno komendy
```bash
cmake ..
make
```

Lub cmake z dodatkowym argumentem
```bash
cmake PTHREADS_COUNT=n ..
```
Przyjmując n jako liczbę watków, która nie może być większa niż liczba kolumn w macierzy reprezentującej graf.

Wywołanie programu na przykładowych danych na 3 node (z folderu build)
```bash
UPC_NODEFILE=../nodes upcrun -shared-heap 256M -c 1 -N 4 -n 4 
./floyd 5 ../matrix.txt ../file.txt
```

przy czym wartości po flagach -n i -N muszą zgadzać się z wartością wyżej wspomnianego PTHREADSCOUNT. Należy również podać 3 argumenty po pliku wykonywalnym, pierwszym bedzie liczba kolumn w macierzy, na której program będzie pracował, drugim plik zawierający jej definicję a trzecim plik, w którym znajdą się ewentualne błędy programu.

