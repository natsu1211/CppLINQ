#include "CppLINQ.h"
#include <iostream>
#include <string>

int main()
{
	using namespace	LL;
	struct PetOwner
	{
		PetOwner() = default;
		PetOwner(const std::string &name, const std::vector<std::string> &pets, const std::vector<int> &num)
			:name_(name), pets_(pets), num_(num) {};
		std::string name_;
		std::vector<std::string> pets_;
		std::vector<int> num_;
	};
	std::vector<PetOwner> vp{ PetOwner("higa", std::vector<std::string>{std::string("scruffy"), std::string("sam")}, std::vector<int>{10,20,30}), PetOwner("ronen", std::vector<std::string>{std::string("walker"), std::string("sugar")}, std::vector<int>{40,50,60}) };
	auto s = from(vp).select_many([](auto i) {return i.pets_; });

	std::vector<std::vector<int>> a{ std::vector<int>{1,2},std::vector<int>{4,5,6,7},std::vector<int>{8,10,11,12} };
	std::vector<int> a1{ 1, 2, 3, 4, 5, 6 };
	std::vector<int> a3{};
	std::vector<float> a2{ 123.456f };
	std::vector<int> a4{ 1,2,2,3,4,4,10 };
	auto b = from(a1);
	auto c = from(a1).skip(2);
	auto e = b.concat(c);
	auto f = from(a4).distinct().linq_union(from(a4));
	auto g = from(a3).default_if_empty();
	//auto b = from(a).select_many([](auto i) {return i; }).to_vector();
	//auto d = from(a2).cast<std::string>();
	//for(auto ele:b)
	//std::cout << ele << std::endl;
	//for (auto ele : e)
		//std::cout << ele << std::endl;

	for (auto ele : s)
	{
		std::cout << ele << std::endl;
	}
		

	//std::cout << c.last_or_default([](auto i) {return i < 0; }) << std::endl;
}