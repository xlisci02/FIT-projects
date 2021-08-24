-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Grammar(
    printGrammar,
    checkNonterminals,
    checkTerminals,
    checkStartingNonterminal,
    checkRules,
    parseRules,
    rightSide,
    leftSide,
    isShortRightRule,
    isStringNonterminalRule,
    isStringRule,
    isSimpleRule,
) where

import Data.List.Split(splitOn)
import Data.List(nub)
import Helpers
import Types


-- prvotne odstranenie prebytocnych epsilonov
-- pravidla tvaru B-># su OK, B->##### je prevedene na pravidlo tvaru B-># a z ostatnych pravidiel
-- je # odstraneny, t.j. napriklad z pravidla B-> #aa#B sa stane B->aaB
removeEpsilons :: [Rule] -> [Rule]
removeEpsilons [] = []
removeEpsilons (rule:rs)
    | rightSide rule == "#" = rule : removeEpsilons rs -- B -> #
    | nub (rightSide rule) == "#" = [leftSide rule, "#"] : removeEpsilons rs -- B -> {#}*
    | otherwise = [leftSide rule, filter (/='#') (rightSide rule)] : removeEpsilons rs -- others


-- spracovanie pravidiel zo vstupneho suboru, rozdelenie retazca pomocou oddelovaca "->"
parseRules :: [FileLine] -> [Rule]
parseRules xs = removeEpsilons (map (splitOn "->") xs)


-- spatny prevod vnutornej reprezentacie pravidla na vystupny format s oddelovacom "->"
mergeRules :: [Rule] -> FileLine
mergeRules = foldr (\ rule -> (++) (leftSide rule ++ "->" ++ rightSide rule ++ "\n")) ""


-- kontrola syntaxe neterminalov
checkNonterminals :: [Nonterminal] -> Bool
checkNonterminals [] = False
checkNonterminals xs = all (isNonterminal . toChar) xs


-- kontrola syntaxe terminalov, pozn.: mnozina terminalov moze byt prazdna
checkTerminals :: [Terminal] -> Bool
checkTerminals [] = True
checkTerminals xs = all (isTerminal . toChar) xs


-- kontrola pociatocneho neterminalu
checkStartingNonterminal :: StartingNonterminal -> [Nonterminal] -> Bool
checkStartingNonterminal n nonterminals
    | n /= '\0' && n `elem` concat nonterminals = True
    | otherwise = False


-- kontrola pravidiel
checkRules :: [Rule] -> [Nonterminal] -> [Terminal] -> Bool
checkRules rules nonterminals terminals
    | null rules = True
    | checkNonterminals (ls rules)
      && rulesHaveBothSides rules
      && eachIsElemOf (ls rules) nonterminals
      && eachIsElemOf (rs rules) ('#' : concat (nonterminals ++ terminals))
      && all (\r -> isShortRightRule r || isStringNonterminalRule r || isStringRule r ||
        isSimpleRule r) rules
      = True
    | otherwise = False
    where ls = map leftSide
          rs = concatMap rightSide


-- funkcia vracia True ak ma pravidlo obe strany, inak False
rulesHaveBothSides :: [Rule] -> Bool
rulesHaveBothSides rules = all (==True) [check r | r <- rules]
    where notEmptyLeftSide r = not (null (leftSide r))
          notEmptyRightSide r = not (null (rightSide r))
          check r = notEmptyLeftSide r && notEmptyRightSide r && (length r == 2)


-- kontrola neterminalu
isNonterminal :: Char -> Bool
isNonterminal x = x `elem` ['A' .. 'Z']


-- kontrola terminalu
isTerminal :: Char -> Bool
isTerminal x = x `elem` ('#':['a'..'z'])


-- pomocka, wrapper funkcia, pre ziskanie lavej strany pravidla
leftSide :: Rule -> String
leftSide = head


-- pomocka, wrapper funkcia, pre ziskanie pravej strany pravidla
rightSide :: Rule -> String
rightSide = last


-- funkcia vracia True ak pravidlo je tvaru A->aB alebo A->#, inak False
isShortRightRule :: Rule -> Bool
isShortRightRule [] = False
isShortRightRule rule
    | rightSide rule == "#" = True
    | length (rightSide rule) == 2  &&
        isTerminal (head $ rightSide rule) &&
        isNonterminal (last $ rightSide rule) = True
    | otherwise = False



-- funkcia vracia True ak pravidlo je tvaru A->x, inak False
isStringRule :: Rule -> Bool
isStringRule [] = False
isStringRule rule = all isTerminal (rightSide rule)


-- funkcia vracia True ak pravidlo je tvaru A->xB, inak False
isStringNonterminalRule :: Rule -> Bool
isStringNonterminalRule [] = False
isStringNonterminalRule rule = isNonterminal (last $ rightSide rule) &&
    all isTerminal (init $ rightSide rule) && length (rightSide rule) > 1


-- funkcia vracia True ak pravidlo je jednoduche, t.j. ma tvar A->B, inak False
isSimpleRule :: Rule -> Bool
isSimpleRule [] = False
isSimpleRule rule = isNonterminal (head $ leftSide rule) && isNonterminal(head $ rightSide rule) &&
    length (leftSide rule) == 1 && length (rightSide rule) == 1


-- vypis gramatiky na stdout
printGrammar :: PLG -> IO ()
printGrammar (n,t,s,p) = do
    putStrLn $ mergeWithComma n
    putStrLn $ mergeWithComma t
    putStrLn [s]
    putStr $ mergeRules p
