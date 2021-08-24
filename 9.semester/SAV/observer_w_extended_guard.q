//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*
deadlock, if x > 3 we never got to Observer.taken location, i.e. we cannot perform any action
*/
A[] x>3 imply not Observer.taken

/*

*/
A[] Observer.taken imply (x>=2 and x<=3)

/*

*/
A[] not deadlock

/*

*/
E<> deadlock
