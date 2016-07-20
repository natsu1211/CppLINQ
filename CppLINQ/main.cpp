#include "CppLINQ.h"
#include <iostream>

int main()
{
	using namespace	LL;
	std::vector<int> a{ 1,2,3,4,5,6,7,8,9,10 };
	auto b = from(a).skip(3).where([](int i) {return i % 2 == 1; });
	for(auto ele:b)
	std::cout << ele << std::endl;
}