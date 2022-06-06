#pragma once

template<typename _Singleton>
class WeakSingleton
{
protected:
	WeakSingleton(_Singleton& singleton);
	virtual ~WeakSingleton();

	_Singleton& mySystem;
};

template<typename _Singleton>
WeakSingleton<_Singleton>::WeakSingleton(_Singleton& singleton)
	: mySystem(singleton)
{}

template<typename _Singleton>
WeakSingleton<_Singleton>::~WeakSingleton()
{}
