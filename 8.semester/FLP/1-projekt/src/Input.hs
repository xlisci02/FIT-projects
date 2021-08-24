-- projekt: plg-2-nka
-- autor: Matus Liscinsky
-- login: xlisci02
-- mail: xlisci02@stud.fit.vutbr.cz
-- datum: 2.4.2020


module Input (
    parseInput,
    readContent
) where

import Data.List(nub)
import Types
import Helpers
import Grammar


-- prevod vstupu do vnutornej reprezentacie gramatiky
-- duplikatne neterminaly, terminaly aj pravidla su odstranene
parseInput :: String -> IO PLG
parseInput contents =
    if length (lines contents) >= 3 then do
        let n = nub (splitWithComma $ head (lines contents))
        let t = nub (splitWithComma $ lines contents !! 1)
        let s = toChar $ lines contents !! 2
        let p = nub (parseRules $ drop 3 $ lines contents)
        return (n,t,s,p)
    else
        error badGrammar


-- nacitanie vstupnych dat zo suboru alebo stdin
readContent :: [String] -> IO String
readContent args
    | length args == 1 = getContents
    | length args == 2 = readFile (last args)
    | otherwise  = error usage
