Projekt: flp20-log, rubikova kocka
Autor: Matus Liscinsky
Login: xlisci02
Datum: 26.4.2020

Preklad programu: `make`
Otestovanie programu: `make test`

## Implementacia algoritmu:
Program hlada postupnost krokov, ktorou sa dostane rubikova kocka zo stavu na vstupe do koncoveho stavu, kedy je
kocka spravne zlozena. Citanie vstupu je zabezpecene funkciami z poskytnuteho ukazkoveho suboru `input2.pl`.
Nasledne sa kocka prevedie do tvaru kde su vsetky informacie o danej stene kocky ulozene v poli pri sebe.
Samotne riesenie spociva v hladani postupnosti n krokov (rotacii kocky), kde n je z mnoziny prirodzenych cisel
vratane nuly, a to tak, ze sa postupne vyskusaju vsetky kombinacie n rotacii. V prvom kroku je n = 0 (tzn. ziadna operacia,
v pripade, ze kocka na vstupe je uz zlozena), pri neuspechu sa skusaju vsetky kombinacie rotacii dlzky n = 1,
(tzn. vysledky po jednej operacii),pri neuspechu sa skusaju kombinacie rotacii dlzky n = 2 a tak dalej az kym sa nenajde
ta spravna postupnost krokov. Cely riadiaci algoritmus zastresuje dvojica funkcii `try_step` a `solve_cube`.

Z podstaty algoritmu vyplyva, ze algoritmus je optimalny, takze vzdy najde to najlepsie riesenie.
Celkovo je implementovanych 12 zakladnych operacii, ktore sa pouzivaju pri skladani rubikovej kocky, menovite
ide o posun hornej, dolnej, prednej, zadnej, lavej a pravej steny do obidvoch smerov (v smere a oproti smeru
hodinovych ruciciek). Pri implementacii je vyuzita vlastnost reverzibility tychto operacii a preto sa na rotacie
oproti smere hodinovych ruciciek vyuziva odpovedajuca rotacia v opacnom smere so zamenenymi parametrami.


## Testovanie:
Na otestovanie projektu bol vytvoreny skript test.py (adresar `test`), ktory sa spusti prikazom
`make test` z hlavneho adresara projektu a je pren potrebny interpret jazyka Python3.
Tento testovaci skript spusta program s testovacimi vstupmi, ktore su ulozene v adresaroch
`test/1tah`, `test/2tahy`, `test/3tahy`, `test/4tahy`, `test/5tahov`, `test/6tahov`, `test/7tahov`.
Kazdy z tychto adresarov obsahuje 3 testovacie vstupy a celkovo tak prebehne 18 testov.
Projekt bol v tomto rozsahu testovany aj na skolskom serveri `merlin` a vysledky su nasledovne:

test/1tah/1tah_0.in: 0.018s
test/1tah/1tah_1.in: 0.016s
test/1tah/1tah_2.in: 0.018s

test/2tahy/2tahy_0.in: 0.018s
test/2tahy/2tahy_1.in: 0.019s
test/2tahy/2tahy_2.in: 0.019s

test/3tahy/3tahy_0.in: 0.021s
test/3tahy/3tahy_1.in: 0.020s
test/3tahy/3tahy_2.in: 0.020s

test/4tahy/4tahy_0.in: 0.061s
test/4tahy/4tahy_0.in: 0.075s
test/4tahy/4tahy_0.in: 0.031s

test/5tahov/5tahov_0.in: 0.183s
test/5tahov/5tahov_1.in: 0.703s
test/5tahov/5tahov_2.in: 0.744s

test/6tahov/6tahov_0.in: 7.879s
test/6tahov/6tahov_1.in: 10.515s
test/6tahov/6tahov_2.in: 7.912s

test/7tahov/7tahov_0.in: 1m30.177s
test/7tahov/7tahov_1.in: 1m34.090s
test/7tahov/7tahov_2.in: 1m28.374s


Pri vstupe vyzadujucom 5,6,7 tahov ma na vysledny cas vacsi vplyv to, v akom stave sa kocka na zaciatku nachadza,
a teda aka postupnost krokov vedie k rieseniu, pretoze s poctom krokov optimalneho riesenia
rychlo rastie mnozstvo kombinacii moznych postupnosti rotacii.
