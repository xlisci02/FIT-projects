-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Helpers(
    toChar,
    splitWithComma,
    mergeWithComma,
    eachIsElemOf,
    divideList,
    usage,
    badGrammar,
    fileError
) where

import Data.List.Split(splitOn)
import Data.List(intercalate)
import Types

-- pomocne retazce pre vypis pri chybe
usage = "Usage: ./plg-2-nka option [file]"
badGrammar = "An error occured while parsing the grammar. Incorrect grammar | grammar input format."
fileError = "File exception has been raised. Specified file probably does not exist."


-- ak je v retazci iba jeden znak funkcia ho vrati, inak vrati NUL znak
toChar :: String -> Char
toChar [] = '\0'
toChar xs
    | length xs == 1 = head xs
    | otherwise = '\0'


-- rozdelenie retazca na zoznam retazcov pomocou oddelovaca ","
splitWithComma :: String -> [String]
splitWithComma line
    | null line = []
    | otherwise = splitOn "," line


-- spatne spojenie retazcov do jedneho, medzi retazce sa prida oddelovac ","
mergeWithComma :: [String] -> String
mergeWithComma = intercalate ","


-- funkcia vracia True pri volani s parametrami A,B ak kazdy prvok A sa nachadza aj v B, inak False
eachIsElemOf :: (Eq a) => [a] -> [a] -> Bool
eachIsElemOf xs ref_xs = all (`elem` ref_xs) xs


-- rozdelenie zoznamu do dvojice zoznamov podla predikatu
divideList :: (a -> Bool) -> [a] -> ([a], [a])
divideList f [] = ([],[])
divideList f (x:xs)
    | f x = (x:y,z)
    | otherwise = (y,x:z)
    where (y,z) = divideList f xs
