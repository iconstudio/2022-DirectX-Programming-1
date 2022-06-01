#pragma once

template<typename _Singleton>
class WeakSingleton
{
protected:
	constexpr WeakSingleton(_Singleton& singleton);
	virtual ~WeakSingleton();

	_Singleton& mySystem;
};
