-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Automata(
    printAutomata
) where

import Data.List(intercalate)
import Helpers
import Types
import Grammar


-- vypis automatu z prevedenej PLG
printAutomata :: PLG -> IO ()
printAutomata (new_n, t, s, new_p)  = do
    -- kazdemu neterminalu je priradeny stav automatu
    let automata_states_binding = zip new_n [0::Int ..]
    -- Stavy NKA
    putStrLn $ intercalate "," $ map (\x -> show (x-1)) [1..length automata_states_binding]
    -- Pociatocny stav
    putStrLn $ getBindingOf [s] automata_states_binding
    -- Koncove stavy
    putStrLn $ mergeWithComma $ getFinishStates new_p automata_states_binding
    -- Pravidla
    putStr $ getAutomataRules new_p automata_states_binding


-- ziskanie stavu automatu z neterminalu
getBindingOf :: Nonterminal -> [AnotedNonterminal] -> String
getBindingOf nonterminal (a:anotations)
    | fst a == nonterminal = show (snd a)
    | otherwise = getBindingOf nonterminal anotations


-- ziskanie koncovych stavov
getFinishStates :: [Rule] -> [AnotedNonterminal] -> [Nonterminal]
getFinishStates [] _ = []
getFinishStates (r:rules) anotations
    | rightSide r == "#" = getBindingOf (leftSide r) anotations : getFinishStates rules anotations
    | otherwise = getFinishStates rules anotations


-- z pravidiel gramatiky sa vygeneruje vystupy retazec reprezentujuci pravidla NKA
getAutomataRules :: [Rule] -> [AnotedNonterminal] -> String
getAutomataRules [] _ = ""
getAutomataRules (r:rules) anotations
    | rightSide r == "#" = getAutomataRules rules anotations -- epsilon pravidlo
    | otherwise = intercalate ","
        [getBindingOf (leftSide r) anotations, -- ziskaj cislo neterminalu (stav NKA)
        [head (rightSide r)], -- prvy znak pravej strany pravidla pre prechod
        getBindingOf (tail (rightSide r)) anotations] -- posledny znak pravej strany je neterminal
        ++ "\n" ++ getAutomataRules rules anotations -- pridaj koniec riadka a chod dalej
