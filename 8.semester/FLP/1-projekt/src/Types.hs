-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Types (
    Nonterminal,
    Terminal,
    StartingNonterminal,
    Rule,
    AnotedNonterminal,
    FileLine,
    PLG
) where

-- typy pre reprezentaciu prvkov gramatiky
type Nonterminal = String
type Terminal = String
type Rule = [String]
type StartingNonterminal = Char

-- anotovany neterminal, pri prevode na NKA je ciselna hodnota oznacenie stavu
type AnotedNonterminal = (String, Int)

-- riadok vstupneho suboru
type FileLine = String

-- gramatika
type PLG = ([Nonterminal], [Terminal], StartingNonterminal, [Rule])
