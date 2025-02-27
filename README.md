# MARCHING SQUARES ALGORITHM

Codul presupune implementarea unui algoritm pentru generarea conturului
folosind tehnica **Marching Squares**. Algoritmul are ca scop
procesarea imaginilor in format PPM, manipularea matricelor si
utilizarea alocarii dinamice de memorie, procesand comenzile primite.

## Implementare

### Structuri de date

- **Structura `image`**: Stocheaza dimensiunile, matricea RGB si grid-ul binar.
- **Structura `color_rgb`**: Reprezinta un pixel prin componentele r, g, b.

### Functii implementate

1. **Citirea imaginii**:

   - `read()`: Verifica formatul si valorile pixelilor din input si afiseaza
     mesaje de eroare daca este necesar.
   - In caz de eroare, elibereaza memoria.

2. **Scrierea imaginii**:

   - `write()`: Afiseaza matricea de pixeli.

3. **Redimensionare**:

   - `resize()`: Parcurg matricea si creez o noua matrice de 4x4 ori mai mare
     pe care o umplu multiplicand fiecare pixel.

   1 0 1 -> 1 0 1
   
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1
            1 0 1

4. **Generare grila**:

   - `grid()`: Verific daca este o imagine incarcata, apoi initializez
   o matrice `medii` in care stochez media aritmetica a componentelor
   r g b din fiecare pixel. Functia calculate_nodes initializeaza o
   matrice noduri de dimensiuni img->height / 4 + 1 in care voi retine
   media aritmetica a valorilor vecinilor de pe diagonala.
   - Daca valorile din matricea noduri sunt mai mici sau egale fata de
   threshold (200), atunci img->binary va fi umpluta cu 1, altfel cu 0
   - In final, afisez grid-ul calculat.

5. **Marching Squares**:
   - `marching_squares()`: Verific daca a fost calculat grid-ul. Daca nu, il
   calculez. Apoi initializez contururile pe baza a celor 16 cazuri prezentate
   in enunt. Functia `process_cell_grid` aplica configuratiile contururilor pe
   baza gridului si modifica matricea de culori. Apoi matricea veche de culori
   este eliberata si este setat pointerul spre noua matrice.

### Gestionarea memoriei

- **Functii de alocare si eliberare**:

  - `alloc_matrix`, `alloc_color`: Alocare dinamica a memoriei.
  - `dealloc_matrix`, `dealloc_color`: Eliberare eficienta a memoriei.

- **Gestionare globala**:
  - `exit_program()`: Eliberare completa a resurselor si resetarea
  campurilor structurilor.
