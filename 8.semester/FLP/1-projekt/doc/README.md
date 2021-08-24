Projekt: plg-2-nka
Autor: Matus Liscinsky
Login: xlisci02
Datum: 2.4.2020

Preklad programu: `make`
Otestovanie programu: `make test`

## Implementacia algoritmu:
Program implementuje algoritmus prevodu pravej linearnej gramatiky (PLG) na nedeterministicky
konecny automat (NKA). Navyse program akcpetuje vo vstupnej gramatike aj jednoduche pravidla,
preto obsahuje implemtovany algoritmus na ich odstranenie.
Zakladnou castou celeho algoritmu je funkcia rewriteGrammar (`Algorithm.hs`) ktorej ulohou
je prevod PLG na gramatiku, ktora obsahuje iba pravidla tvaru A->aB a A->#.
Tato funkcia rozdeli dane pravidla do styroch skupin podla ich tvaru:
1. A->aB, A->#
2. A->xB
3. A->x
4. A->B

Pravidla prvej skupiny su uz v zelanom tvare. Pravidlam skupiny 2 a 3 sa priradia unikatne id a vygeneruju
sa z nich nove pravidla podla vety 3.2 (studijna opora TIN). Implementacia tohto kroku je zahrnuta
vo funkcii unfoldRules (`Algorithm.hs`), ktora postupne rozbali jednotlive pravidla na nove za pomoci
funkcii generateFromTNrule a generateFromTrule (`Algorithm.hs`).
Okrem ineho algoritmus zahrna tvorbu novych neterminalov, ktore su tvorene z povodneho neterminalu
na lavej strane pravidla, id pravidla, oddelovaca (0) a dlzky pravej strany pravidla:
Priklad, pravidla:
A->aaaB
B->bbb
sa prepisu na pravidla:
A->aA104     -- id=1 oddelovac=0 |aaaB| = 4
A104->aA103  -- id=1 oddelovac=0 |aaB| = 3
A103->aB
B->bB203     -- id=2 oddelovac=0 |bbb| = 3
B203->bB202  -- id=2 oddelovac=0 |bb| = 2
B202->bB201  -- id=2 oddelovac=0 |b| = 1
B201->#

Tieto nove neterminaly su pridane ku existujucej mnozine neterminalov.
ROZSIRENIE: Nasledne je aplikovany algoritmus pre odstranenie jednoduchych pravidiel
(funkcia removeSimpleRules v `Algorithm.hs`), cim vzniknu nove a zaroven posledne
pravidla, ktore su sucastou pretvorenej gramatiky.

Prevod gramatiky s pravidlami v spominanom tvare na NKA je uz trivialny. Kazdemu z neterminalov sa
priradi unikatne nezaporne cislo, reprezentujuce stav v NKA. Pociatocny stav je stav prisluchajuci
pociatocnemu neterminalu vstupnej gramatiky.
Mnozina koncovych stavov a pravidla NKA sa ziskaju z pravidiel gramatiky podla vety 3.6 (studijna opora TIN),
ktorej implementacia sa nachadza vo funkciach getFinishStates a getAutomataRules (`Automata.hs`).


## Kontrola vstupnej gramatiky:
Este pred samotnou kontrolou sa z pravidiel odstranuju nadbytocne prazdne retazce #, nasledovne:
V pripade pravidla tvaru:
B-># je pravidlo nezmenene,
B->##### je prevedene na pravidlo tvaru B->#, a
z ostatnych pravidiel je prazdny retazec odstraneny, t.j. napriklad z pravidla
B-> #aa#B sa stane pravidlo B->aaB.
Navyse vsetky duplikaty v zozname neterminalov, terminalov a aj pravidiel su pred kontrolou
takisto odstranene.
Samotna kontrola spociva v overeni formatu PLG a jej jednotlivych prvkov, coz je implementovane vo
funkciach checkNonterminals, checkTerminals, checkStartingNonterminal a checkRules (`Grammar.hs`).


## Kontrola argumentov:
Program overuje pocet argumentov a volbu uzivatela. Pri spusteni s nekorektnymi argumentmi program
konci chybovou hlaskou obsahujucou napovedou k spusteniu.


## Testy:
Na otestovanie projektu bol vytvoreny skript test.py (adresar `test`), ktory sa spusti prikazom
`make test` z hlavneho adresara projektu a je pren potrebny interpret jazyka Python3.
Tento testovaci skript spusta program s testovacimi vstupmi, ktore su ulozene v adresaroch
`test/incorrect` a `test/correct`.
Celkovo prebehne 42 testov na chybnych testovacich vstupoch a 3 * 10 testov na spravnych
testovacich vstupoch (10 testov pre kazdu z 3 moznych volieb).
Projekt bol v tomto rozsahu testovany aj na skolskom serveri merlin.
