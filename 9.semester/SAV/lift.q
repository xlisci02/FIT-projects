//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*

*/
A[] not ((Door0.Open and Door1.Open) or (Door1.Open and Door2.Open) or (Door0.Open and Door2.Open))

/*
Whether all doors are closed, or max one of them is open.
*/
A[] (Door0.Open and Door1.Closed and Door2.Closed ) or \
  (Door0.Closed and Door1.Open and Door2.Closed ) or\
  (Door0.Closed and Door1.Closed and Door2.Open ) or \
  (Door0.Closed and Door1.Closed and Door2.Closed )

/*

*/
E<> Door1.Open and time > 3

/*

*/
E<> Door2.Open and time > 6

/*
We cannot get to first floor in less than 3 time units.
*/
E<> Door1.Open and time <= 3

/*
We cannot get to second floor in less than 6 time units.
*/
E<> Door2.Open and time <= 6

/*

*/
E<> Door2.Open\


/*

*/
E<> Door1.Open

/*

*/
E<> Door0.Open

/*

*/
E<> Cabin.Floor2

/*

*/
E<> Cabin.Floor1

/*

*/
E<> Cabin.Floor0

/*

*/
A[] not deadlock
