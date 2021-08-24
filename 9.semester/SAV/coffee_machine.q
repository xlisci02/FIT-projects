//This file was generated from (Commercial) UPPAAL 4.0.15 rev. CB6BB307F6F681CB, November 2019

/*

*/
E<> Process.B and y==0

/*

*/
E<> Process.C and x >1

/*

*/
A[] Process.D imply not (y<1)

/*

*/
E[] Process.A

/*

*/
E<> Process.B imply Process.D

/*

*/
A[] Process.D imply not Process.A

/*

*/
E<> Process.D

/*

*/
E<> Process.C

/*

*/
E<> Process.B

/*

*/
E<> Process.A
