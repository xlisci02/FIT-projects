-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Algorithm (
    rewriteGrammar
) where

import Data.List(nub)
import Types
import Grammar
import Helpers

-- pridanie novych neterminalov do gramatiky po aplikovani vety 3.2
addNewNonterminals :: [Nonterminal]-> [Rule] -> [Nonterminal]
addNewNonterminals oldN newRules = oldN ++ [head n | n <- newRules, head n `notElem` oldN]


-- vygenerovanie noveho neterminalu na zaklade id pravidla a dlzky pravej strany
-- 0 je oddelovac id pravidla a dlzky pravej strany (zamedzenie generovania rovnakych neterminalov)
generateNonterm :: Nonterminal -> Int -> Int -> Nonterminal
generateNonterm str id len =  head str : (show id ++ "0" ++ show len)


-- generovanie novych pravidiel z pravidiel tvaru A->xB
-- ciselny parameter je id pravidla a sluzi na generovanie novych neterminalov
-- pozn.: TNrule ~ terminals_nonterminal_rule
generateFromTNrule :: Rule -> Int -> [Rule]
generateFromTNrule [] _ = []
generateFromTNrule (ls:rs) num =
    if length (head rs) > 2 then
        [ls, head (head rs) : generateNonterm ls num (length $ head rs)] :
        generateFromTNrule [generateNonterm ls num (length $ head rs), tail (head rs)] num
    else
        [[ls, concat rs]]


-- generovanie novych pravidiel z pravidiel tvaru A->x
-- ciselny parameter je id pravidla a sluzi na generovanie novych neterminalov
-- pozn.: Trule ~ terminals_rule
generateFromTrule :: Rule -> Int -> [Rule]
generateFromTrule [] _ = []
generateFromTrule (ls:rs) num
    | rs == [""] = [[ls, "#"]]
    | otherwise = [ls, head (head rs) : generateNonterm ls num (length $ head rs)] :
                   generateFromTrule [generateNonterm ls num (length $ head rs), tail (head rs)] num


-- rozbalenie pravidiel podla vety 3.2
-- pomocou fukcie na generovanie novych pravidiel a pola pravidiel s ich id vzniknu nove pravidla
unfoldRules :: (Rule -> Int -> [Rule]) -> [(Rule, Int)] -> [Rule]
unfoldRules _ [] = []
unfoldRules f [x] = uncurry f x
unfoldRules f (x:xs) = uncurry f x ++ unfoldRules f xs

-- algoritmus prepisu PLG na gramatiku, ktora obsahuje iba pravidla tvaru A->aB alebo A->#
rewriteGrammar :: PLG -> PLG
rewriteGrammar (n,t,s,p) = do
    -- rozdelenie pravidiel na short_rules (pravidla tvaru A->aB, A->#) a ostatne
    let (short_rules, other) = divideList isShortRightRule p
    -- rozdelenie zvysnych pravidiel na TN pravidla a zvysne pravidla
    let (tn_rules, other2) = divideList isStringNonterminalRule other
    -- rozdelenie zvysnych pravidiel na T pravidla a jednoduche pravidla
    let (t_rules, simple_rules) = divideList isStringRule other2

    -- kazde TN a T pravidlo dostane svoje id (pre generovanie novych neterminalov)
    let annotated_tn_rules = zip tn_rules [1..] -- anotovane TN pravidla
    let annotated_t_rules = zip t_rules [length (zip tn_rules [1..])+1 ..] -- anotovane T pravidla

    -- rozbalenie do koncoveho tvaru pravidiel
    let generated_tn_rules = unfoldRules generateFromTNrule annotated_tn_rules
    let generated_t_rules = unfoldRules generateFromTrule annotated_t_rules

    -- nova mnozina neterminalov a pravidiel
    let new_n = addNewNonterminals (addNewNonterminals n generated_tn_rules) generated_t_rules
    let new_p = short_rules ++ generated_tn_rules ++ generated_t_rules

    -- rozsirenie, odstranenie jednodychych pravidiel
    let generated_simple_rules = removeSimpleRules new_p (computeAllNsets simple_rules n)
    let new_p' = short_rules ++ generated_tn_rules ++ generated_t_rules ++ generated_simple_rules

    -- odstrania sa aj duplikatne pravidla (pridane pri odstranovani jendoduchych pravidiel)
    let new_p = nub new_p'
    (new_n,t,s,new_p)


-- ROZSIRENIE, odstranenie jednoduchych pravidiel
-- spocitanie vsetkych mnozin N_A = {B | A ->* B}
-- vracia zoznam mnozin tvaru ["A", "B", "A"], co znamena N_A = {B,A}
computeAllNsets :: [Rule] -> [Nonterminal] -> [[Nonterminal]]
computeAllNsets _ [] = []
computeAllNsets [] _ = []
computeAllNsets rs (n:ns) = (n : computeNset rs [n]) : computeAllNsets rs ns


-- zostrojenie mnoziny N_A = {B | A ->* B}
computeNset :: [Rule] -> [Nonterminal] -> [Nonterminal]
computeNset rs n
    | eachIsElemOf (comp rs n) n = n
    | otherwise = computeNset rs (comp rs n)
    where comp rs n = foldl(\set r ->
            if leftSide r `elem` set  && isSimpleRule r then nub(rightSide r:set) else set) n rs


-- funkcia vracia zoznam neterminalov, ktorych mnozina obsahuje neterminal dany parametrom
getNsetsContaining :: [[Nonterminal]] -> Rule -> [Nonterminal]
getNsetsContaining ns r = map head (filter (\n -> leftSide r `elem` tail n) ns)


-- pomocna funkcia, vracia pravidla ktore maju na lavej strane dany neterminal
getRulesOfN :: [Rule] -> [Nonterminal] -> [Rule]
getRulesOfN (r:rs) ns
    | leftSide r `elem` ns = r : getRulesOfN rs ns
    | otherwise = getRulesOfN rs ns


-- pomocna fukncia, zlozi pravu stranu a neterminal do pravidla, pre ucely funkcie removeSimpleRules
mergeNwithRightSide :: Nonterminal -> String -> Rule
mergeNwithRightSide nonterminal rside = [rside, nonterminal]


-- hlavna cast algoritmu odstranenia jednoduchych pravidiel
-- vstupom je zoznam pravidiel a mnozin N_A
removeSimpleRules :: [Rule] -> [[Nonterminal]] -> [Rule]
removeSimpleRules [] _ = []
removeSimpleRules (r:rs) ns
    | not(isSimpleRule r) = map (mergeNwithRightSide (rightSide r))
        (getNsetsContaining ns r) ++ removeSimpleRules rs ns
    | otherwise = removeSimpleRules rs ns
