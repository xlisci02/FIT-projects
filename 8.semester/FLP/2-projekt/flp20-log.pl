/**
 * projekt: flp20-log, rubikova kocka
 * autor: Matus Liscinsky
 * login: xlisci02
 * mail: xlisci02@stud.fit.vutbr.cz
 * datum: 26.4.2020
*/

/********** Funkcie pre citanie zo vstupu, prevzate z ukazkoveho suboru input2.pl **********/

/** cte radky ze standardniho vstupu, konci na LF nebo EOF */
read_line(L,C) :-
    get_char(C),
    (isEOFEOL(C), L = [], !;
        read_line(LL,_), % atom_codes(C,[Cd]),
        [C|LL] = L).


/** testuje znak na EOF nebo LF */
isEOFEOL(C) :-
    C == end_of_file;
    (char_code(C,Code), Code==10).


read_lines(Ls) :-
    read_line(L,C),
    ( C == end_of_file, Ls = [] ;
      read_lines(LLs), Ls = [L|LLs]
    ).


/** rozdeli radek na podseznamy */
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1


/** vstupem je seznam radku (kazdy radek je seznam znaku) */
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).

/********** Koniec implementacie citania zo vstupu (input2.pl) **********/

/** Prepis stavu kocky vo forme podorisu do lepsie spracovatelnej formy */
reformat_cube(
[
    Up0,
    Up1,
    Up2,
    [Front0_0, Front0_1, Front0_2, _,
    Right0_0, Right0_1, Right0_2, _,
    Back0_0, Back0_1, Back0_2, _,
    Left0_0, Left0_1, Left0_2],
    [Front1_0, Front1_1, Front1_2, _,
    Right1_0, Right1_1, Right1_2, _,
    Back1_0, Back1_1, Back1_2, _,
    Left1_0, Left1_1, Left1_2],
    [Front2_0, Front2_1, Front2_2, _,
    Right2_0, Right2_1, Right2_2, _,
    Back2_0, Back2_1, Back2_2, _,
    Left2_0, Left2_1, Left2_2],
    Down0,
    Down1,
    Down2
],
[
    Up0,Up1,Up2,

    [Front0_0,  Front0_1, Front0_2],
    [Front1_0, Front1_1, Front1_2],
    [Front2_0, Front2_1, Front2_2],

    [Right0_0, Right0_1, Right0_2],
    [Right1_0, Right1_1, Right1_2],
    [Right2_0, Right2_1, Right2_2],

    [Back0_0, Back0_1, Back0_2],
    [Back1_0, Back1_1, Back1_2],
    [Back2_0, Back2_1, Back2_2],

    [Left0_0, Left0_1, Left0_2],
    [Left1_0, Left1_1, Left1_2],
    [Left2_0, Left2_1, Left2_2],

    Down0,Down1,Down2
]).


/********** Rotacie, vyuziva sa fakt, ze pre kazdu rotaciu existuje reverzna rotacia **********/

/** Rotacia prednou stranou oproti smeru hod. ruciciek */
rotate_cube(front_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(front_clockwise, RotatedCube, Cube).

/** Rotacia prednou stranou v smere hod. ruciciek */
rotate_cube(front_clockwise,
[
    Up0, Up1,[Up2_0, Up2_1, Up2_2],
    [Front0_0, Front0_1, Front0_2], [Front1_0, Front1_1, Front1_2], [Front2_0, Front2_1, Front2_2],
    [Right0_0 | Right0_12], [Right1_0 | Right1_12], [Right2_0 | Right2_12],
    Back0, Back1, Back2,
    [Left0_0, Left0_1, Left0_2], [Left1_0, Left1_1, Left1_2], [Left2_0, Left2_1, Left2_2],
    [Down0_0, Down0_1, Down0_2], Down1, Down2
],
[
    Up0, Up1, [Left2_2, Left1_2, Left0_2],
    [Front2_0, Front1_0, Front0_0],[Front2_1, Front1_1, Front0_1],[Front2_2, Front1_2, Front0_2],
    [Up2_0 | Right0_12],[Up2_1 | Right1_12], [Up2_2 | Right2_12],
    Back0, Back1, Back2,
    [Left0_0, Left0_1, Down0_0],[Left1_0, Left1_1, Down0_1],[Left2_0, Left2_1, Down0_2],
    [Right2_0, Right1_0, Right0_0], Down1, Down2
]).

/** Rotacia pravou stranou oproti smeru hod. ruciciek */
rotate_cube(right_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(right_clockwise, RotatedCube, Cube).

/** Rotacia prednou stranou v smere hod. ruciciek */
rotate_cube(right_clockwise,
[
    [Up0_0, Up0_1, Up0_2], [Up1_0, Up1_1, Up1_2], [Up2_0, Up2_1, Up2_2],
    [Front0_0, Front0_1, Front0_2], [Front1_0, Front1_1, Front1_2], [Front2_0, Front2_1, Front2_2],
    [Right0_0, Right0_1, Right0_2], [Right1_0, Right1_1, Right1_2], [Right2_0, Right2_1, Right2_2],
    [Back0_0, Back0_1, Back0_2], [Back1_0, Back1_1, Back1_2], [Back2_0, Back2_1, Back2_2],
    Left0,Left1,Left2,
    [Down0_0, Down0_1, Down0_2], [Down1_0, Down1_1, Down1_2], [Down2_0, Down2_1, Down2_2]
],
[
    [Up0_0, Up0_1, Front0_2], [Up1_0, Up1_1, Front1_2], [Up2_0, Up2_1, Front2_2],
    [Front0_0, Front0_1, Down0_2], [Front1_0, Front1_1, Down1_2], [Front2_0, Front2_1, Down2_2],
    [Right2_0, Right1_0, Right0_0], [Right2_1, Right1_1, Right0_1], [Right2_2, Right1_2, Right0_2],
    [Up2_2, Back0_1, Back0_2], [Up1_2, Back1_1, Back1_2], [Up0_2, Back2_1, Back2_2],
    Left0,Left1,Left2,
    [Down0_0, Down0_1, Back2_0], [Down1_0, Down1_1, Back1_0], [Down2_0, Down2_1, Back0_0]
]).


/** Rotacia lavou stranou oproti smeru hod. ruciciek */
rotate_cube(left_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(left_clockwise, RotatedCube, Cube).
/** Rotacia lavou stranou v smere hod. ruciciek */
rotate_cube(left_clockwise,
[
    [Up0_0 | Up0_12], [Up1_0 | Up1_12], [Up2_0 | Up2_12],
    [Front0_0 | Front0_12], [Front1_0 | Front1_12], [Front2_0 | Front2_12],
    Right0,Right1,Right2,
    [Back0_0, Back0_1, Back0_2], [Back1_0, Back1_1, Back1_2], [Back2_0, Back2_1, Back2_2],
    [Left0_0, Left0_1, Left0_2], [Left1_0, Left1_1, Left1_2], [Left2_0, Left2_1, Left2_2],
    [Down0_0 | Down0_12], [Down1_0 | Down1_12], [Down2_0 | Down2_12]
],
[
    [Back2_2 | Up0_12], [Back1_2 | Up1_12], [Back0_2 | Up2_12],
    [Up0_0 | Front0_12], [Up1_0 | Front1_12], [Up2_0 | Front2_12],
    Right0,Right1,Right2,
    [Back0_0, Back0_1, Down2_0], [Back1_0, Back1_1, Down1_0], [Back2_0, Back2_1, Down0_0],
    [Left2_0, Left1_0, Left0_0], [Left2_1, Left1_1, Left0_1], [Left2_2, Left1_2, Left0_2],
    [Front0_0 | Down0_12], [Front1_0 | Down1_12], [Front2_0 | Down2_12]
]).

/** Rotacia zadnou stranou oproti smeru hod. ruciciek */
rotate_cube(back_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(back_clockwise, RotatedCube, Cube).
/** Rotacia zadnou stranou v smere hod. ruciciek */
rotate_cube(back_clockwise,
[
    [Up0_0, Up0_1, Up0_2], Up1_1, Up1_2,
    Front0,Front1,Front2,
    [Right0_0, Right0_1, Right0_2], [Right1_0, Right1_1, Right1_2], [Right2_0, Right2_1, Right2_2],
    [Back0_0, Back0_1, Back0_2], [Back1_0, Back1_1, Back1_2], [Back2_0, Back2_1, Back2_2],
    [Left0_0 | Left0_12], [Left1_0 | Left1_12], [Left2_0 | Left2_12],
    Down0, Down_1, [Down2_0, Down2_1, Down2_2]
],
[
    [Right0_2, Right1_2, Right2_2], Up1_1, Up1_2,
    Front0,Front1,Front2,
    [Right0_0, Right0_1, Down2_2], [Right1_0, Right1_1, Down2_1], [Right2_0, Right2_1, Down2_0],
    [Back2_0, Back1_0, Back0_0], [Back2_1, Back1_1, Back0_1], [Back2_2, Back1_2, Back0_2],
    [Up0_2 | Left0_12], [Up0_1 | Left1_12], [Up0_0 | Left2_12],
    Down0, Down_1, [Left0_0, Left1_0, Left2_0]
]).

/** Rotacia vrchnou stranou oproti smeru hod. ruciciek */
rotate_cube(up_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(up_clockwise, RotatedCube, Cube).

/** Rotacia vrchnou stranou v smere hod. ruciciek */
rotate_cube(up_clockwise,
[
    [Up0_0, Up0_1, Up0_2], [Up1_0, Up1_1, Up1_2], [Up2_0, Up2_1, Up2_2],
    Front0, Front1, Front2,
    Right0, Right1, Right2,
    Back0, Back1, Back2,
    Left0, Left1, Left2,
    Down0,Down1,Down2
],
[
    [Up2_0, Up1_0, Up0_0], [Up2_1, Up1_1, Up0_1], [Up2_2, Up1_2, Up0_2],
    Right0, Front1, Front2,
    Back0, Right1, Right2,
    Left0, Back1, Back2,
    Front0, Left1, Left2,
    Down0,Down1,Down2
]).

/** Rotacia spodnou stranou oproti smeru hod. ruciciek */
rotate_cube(down_counterclockwise, Cube, RotatedCube) :-
    rotate_cube(down_clockwise, RotatedCube, Cube).

/** Rotacia spodnou stranou v smere hod. ruciciek */
rotate_cube(down_clockwise,
[
    Up0,Up1,Up2,
    Front0, Front1, Front2,
    Right0, Right1, Right2,
    Back0, Back1, Back2,
    Left0, Left1, Left2,
    [Down0_0, Down0_1, Down0_2], [Down1_0, Down1_1, Down1_2], [Down2_0, Down2_1, Down2_2]
],
[
    Up0,Up1,Up2,
    Front0, Front1, Left2,
    Right0, Right1, Front2,
    Back0, Back1, Right2,
    Left0, Left1, Back2,
    [Down2_0, Down1_0, Down0_0], [Down2_1, Down1_1, Down0_1], [Down2_2, Down1_2, Down0_2]
]).

/********** Riadiaci algoritmus **********/

/** Overenie zlozenej rubikovej kocky */
is_solved(
[
    [Up,Up,Up], [Up,Up,Up], [Up,Up,Up],
    [Front,Front,Front], [Front,Front,Front], [Front,Front,Front],
    [Right,Right,Right], [Right,Right,Right], [Right,Right,Right],
    [Back,Back,Back], [Back,Back,Back], [Back,Back,Back],
    [Left,Left,Left], [Left,Left,Left], [Left,Left,Left],
    [Down,Down,Down], [Down,Down,Down], [Down,Down,Down]
]).

/** Rekurzivna funkcia postupne skusa kombinacie rotacii */
try_step(Cube, [], Cube) .
try_step(Cube, [R | Rotations], FinalCube) :-
    try_step(RotatedCube, Rotations, FinalCube), rotate_cube(R, Cube, RotatedCube).

/** Riadiaca funkcia, ktora overuje ci kombinaciou rotacii doslo k zlozeniu kocky */
solve_cube(Cube, Steps) :- try_step(Cube, Steps, FinalCube), is_solved(FinalCube).


/********** Potrebne funkcie na vypis **********/

/** Vypis kocky vo forme podorysu */
print_cube(
[
    [Up0_0, Up0_1, Up0_2],
    [Up1_0, Up1_1, Up1_2],
    [Up2_0, Up2_1, Up2_2],

    [Front0_0,  Front0_1, Front0_2],
    [Front1_0, Front1_1, Front1_2],
    [Front2_0, Front2_1, Front2_2],

    [Right0_0, Right0_1, Right0_2],
    [Right1_0, Right1_1, Right1_2],
    [Right2_0, Right2_1, Right2_2],

    [Back0_0, Back0_1, Back0_2],
    [Back1_0, Back1_1, Back1_2],
    [Back2_0, Back2_1, Back2_2],

    [Left0_0, Left0_1, Left0_2],
    [Left1_0, Left1_1, Left1_2],
    [Left2_0, Left2_1, Left2_2],

    [Down0_0,  Down0_1, Down0_2],
    [Down1_0, Down1_1, Down1_2],
    [Down2_0, Down2_1, Down2_2]
]) :-
    format("~w~w~w", [Up0_0, Up0_1, Up0_2]),nl,
    format("~w~w~w", [Up1_0, Up1_1, Up1_2]),nl,
    format("~w~w~w", [Up2_0, Up2_1, Up2_2]),nl,

    format("~w~w~w ", [Front0_0, Front0_1, Front0_2]),
    format("~w~w~w ", [Right0_0, Right0_1, Right0_2]),
    format("~w~w~w ", [Back0_0, Back0_1, Back0_2]),
    format("~w~w~w", [Left0_0, Left0_1, Left0_2]),nl,

    format("~w~w~w ", [Front1_0, Front1_1, Front1_2]),
    format("~w~w~w ", [Right1_0, Right1_1, Right1_2]),
    format("~w~w~w ", [Back1_0, Back1_1, Back1_2]),
    format("~w~w~w", [Left1_0, Left1_1, Left1_2]),nl,

    format("~w~w~w ", [Front2_0, Front2_1, Front2_2]),
    format("~w~w~w ", [Right2_0, Right2_1, Right2_2]),
    format("~w~w~w ", [Back2_0, Back2_1, Back2_2]),
    format("~w~w~w", [Left2_0, Left2_1, Left2_2]),nl,

    format("~w~w~w", [Down0_0, Down0_1, Down0_2]),nl,
    format("~w~w~w", [Down1_0, Down1_1, Down1_2]),nl,
    format("~w~w~w", [Down2_0, Down2_1, Down2_2]),nl.


/** Vypis krokov, teda vysledkov aplikacii jednotlivych krokov */
print_steps(_,[]).
print_steps(Cube, [S | Steps]):-
  rotate_cube(S, Cube, RotatedCube),
  nl,print_cube(RotatedCube),
  print_steps(RotatedCube, Steps).

/********** HLAVNA FUNKCIA **********/
start :-
    prompt(_, ''),
    read_lines(Lines),
    reformat_cube(Lines,Cube),
    print_cube(Cube),
    solve_cube(Cube, Steps),
    print_steps(Cube, Steps),
    halt.
