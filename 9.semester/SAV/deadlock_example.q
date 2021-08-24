//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*

*/
A[] (Process.C and y >= 15) imply deadlock

/*

*/
E<> Process.C and y >= 15

/*

*/
A[] (Process.B and y > 5 ) imply deadlock

/*

*/
E<> Process.B and y > 5

/*
Prove, that we cannot reach the state, where the automaton is in the location A and y clock are greater or equal to 15.
*/
A[] not (Process.A and y>=15)

/*

*/
A[] (Process.A and x > 10) imply deadlock

/*

*/
E<> (Process.A and x > 10) 

/*
Confirmation of a deadlock possibility.
*/
E<> deadlock

/*
The system run may lead to deadlock.
*/
A[] not deadlock

/*
Ask if we can reach the state C.
*/
E<> Process.C

/*
Ask if we can reach the state B.
*/
E<> Process.B

/*
Ask if we can reach the state A.
*/
E<> Process.A
