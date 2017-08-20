//******************************************************************
// 
//  Generated by IDSL to IDL Translator
//  
//  File name: Test.idl
//  Source: Test.idsl
//  
//******************************************************************   
#ifndef ROBOCOMPTESTS_ICE
#define ROBOCOMPTESTS_ICE

module RoboCompTests{

	interface test{
		void  printmsg(string message);
	};

	interface outTest{
		int divide(int divident, int divisor, out int reminder);
		int sum(int num1, int num2);
		void substract(int num1, int num2, out int result);
		void printmsg(string message);
	};

	["cpp:comparable"]
	interface serviceTest{
		idempotent void srvTest(int id, out int idTest);
	};

	interface publishTest{
		void msgTest(int id);
	};
};

#endif