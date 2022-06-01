#include "pch.hpp"
#include "WeakSingleton.hpp"

template<typename _Singleton>
constexpr WeakSingleton<_Singleton>::WeakSingleton(_Singleton& singleton)
	: mySystem(singleton)
{}

template<typename _Singleton>
WeakSingleton<_Singleton>::~WeakSingleton()
{}
