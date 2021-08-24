-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


import System.Environment(getArgs)
import Control.Monad(unless)
import Control.Exception(catch, IOException)

import Algorithm
import Automata
import Grammar
import Input
import Types
import Helpers

-- vyber akcie na zaklade volby uzivatela
doAction :: [String] -> PLG -> IO ()
doAction args plg
    | "-i" `elem` args = printGrammar plg
    | "-1" `elem` args = printGrammar $ rewriteGrammar plg
    | "-2" `elem` args = printAutomata $ rewriteGrammar plg
    | otherwise = error usage

-- hlavna funkcia programu plg-2-nka
main :: IO ()
main = do
    -- ziskanie argumentov
    args <- getArgs
    -- nacita sa obsah vstupneho suboru
    content <- (catch :: IO a -> (IOException -> IO a) -> IO a) (readContent args)
            (\_ -> error fileError)
    -- parsing vstupu do vnutornej reprezentacie gramatiky
    (n,t,s,p) <- parseInput content
    -- kontrola gramatiky
    unless (checkNonterminals n &&
            checkTerminals t &&
            checkStartingNonterminal s n &&
            checkRules p n t) $ error badGrammar
    -- samotna akcia dana uzivatelovou volbou
    doAction args (n,t,s,p)
    return ()
