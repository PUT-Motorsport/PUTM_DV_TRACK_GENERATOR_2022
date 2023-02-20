#pragma once

#include <string>
#include <map>

template < class T1, class T2, class T3 >
struct Triple
{
	T1 item1;
	T2 item2;
	T3 item3;

	Triple <T1, T2, T3>& operator = (const Triple <T1, T2, T3> tuple)
	{
		item1 = tuple.item1;
		item2 = tuple.item2;
		item3 = tuple.item3;
	}
};

template < class T1, class T2, class T3, class T4 >
struct Quadraple
{
	T1 item1;
	T2 item2;
	T3 item3;
	T4 item4;

	Quadraple <T1, T2, T3, T4>& operator = (const Quadraple <T1, T2, T3, T4> tuple)
	{
		item1 = tuple.item1;
		item2 = tuple.item2;
		item3 = tuple.item3;
		item4 = tuple.item4;
	}
};

template < class T1, class T2, class T3, class T4, class T5 >
struct Quintuple
{
	T1 item1;
	T2 item2;
	T3 item3;
	T4 item4;
	T5 item5;

	Quintuple <T1, T2, T3, T4, T5>& operator = (const Quintuple <T1, T2, T3, T4, T5> tuple)
	{
		item1 = tuple.item1;
		item2 = tuple.item2;
		item3 = tuple.item3;
		item4 = tuple.item4;
		item5 = tuple.item5;
	}
};