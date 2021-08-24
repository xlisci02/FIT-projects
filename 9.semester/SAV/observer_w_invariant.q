//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*

*/
A[] Observer.idle imply x<=3

/*

*/
E<> Observer.idle and x>2

/*

*/
A[] Observer.taken imply (x>=2 and x<=3)

/*

*/
A[] not deadlock
