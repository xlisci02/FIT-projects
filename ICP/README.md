# [Seminár C++](https://www.fit.vutbr.cz/study/courses/index.php?id=12139) 
  - for *ENGLISH* version click [here](https://github.com/xlisci02/FIT-projects/tree/master/ICP#icp-project-c).
### Projekt
  - Aplikácia pre návrh a editáciu **blokových schém** pre počítanie s **komplexnými čislami**
  - GUI aplikácia s využitím frameworku Qt
  - *Základné vlastnosti*
    - aplikácia umožňuje vytvárať, editovať, ukladať a načítať blokové schémy
    - každá schéma má svoj jedinečný názov
    - vytvorené schémy možno uložiť a opätovne načítať
    - schéma je zložená z blokov a prepojov medzi blokmi
    - nabehnutím myši nad prepoj sa zobrazí aktuálny stav dát
  - *Bloky*
    - každý blok má definované vstupné a výstupné porty
    - s každým portom je spojený typ, ktorý je reprezentovaný množinou dát v podobe dvojíc názov-> hodnota; 
    - hodnota je vždy typu double
    - bloky je možné spojiť len medzi výstupným a vstupným portom
    - každý blok transformuje hodnoty zo vstupných portov na hodnoty výstupných portov
  - *Prepojenie medzi blokmi*
    - systém kontroluje kompatibilitu vstupného a výstupného portu prepoja (rovnaký typ dát)
    - typ dát je priradený prepojí automaticky podľa spojených portov
  - *Výpočet*
    - po zostavení (načítaní) schémy je možné vykonať výpočet
    - systém detekuje cykly v schéme; ak sú v schéme cykly, nemožno vykonať výpočet
    - systém požiada o vyplnenie údajov vstupných portov, ktoré nie sú napojené a potom postupne vykonáva výpočty jednotlivých blokov podľa definovaných vzorcov v každom bloku
    - pri výpočte sa vždy zvýrazní blok, ktorý je práve prepočítavaný
    - výpočty sa dajú prechádzať (jeden krok = prepočet jedného bloku)
    
    ----
# ICP Project (C++)

## Graphical block editor for basic complex number operations.

### Authors
  - [Matúš Liščinský](https://github.com/xlisci02)
  - [Vladimír Marcin](https://github.com/vmarcin)

Block Editor for computing basic complex number operations with user 
friendly and simple GUI. 

### Controls
Choose the block on the left side with operation you want, click on the scene 
and block will appear. Add to the scene how many blocks you wish and connect them with lines. 
To connect two blocks you have to switch mode by clicking on button with line in toolbar. 
Toolbar also includes buttons for moving with blocks, removing them, removing connections, 
running the scheme and stepping the calculation. Before the calculation itself, 
you will be asked for required, undefined values.  
