//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*
Fischer's mutual exclusion protocol.
*/
//NO_QUERY

/*
Mutex requirement.
*/
A[] forall (i:id_t) forall (j:id_t) P(i).cs && P(j).cs imply i == j

/*
The system is deadlock free.
*/
A[] not deadlock

/*
Whenever P(1) requests access to the critical section it will eventually enter the wait state.
*/
P(1).req --> P(1).wait

/*
not satisfied, we can be in wait state forever
*/
P(1).req --> P(1).cs
