#include "stdafx.h"
#include "Material.hpp"

int ReadIntegerFromFile(std::ifstream& stream)
{
	int result = 0;
	stream >> result;
	return result;
}

float ReadFloatFromFile(std::ifstream& stream)
{
	float result = 0.0f;
	stream >> result;
	return result;
}

BYTE ReadStringFromFile(std::ifstream& stream, char* token)
{
	BYTE sz = 0;
	stream >> sz;

	const size_t length = static_cast<size_t>(sz);

	//char* local_token = new char[length + 1];
	//ZeroMemory(local_token, length + 1);
	stream.readsome(token, length + 1);

	//CopyMemory(token, local_token, length);
	//delete local_token;

	return sz;
}

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

DESC_HANDLE operator+(const DESC_HANDLE& handle, const size_t increment)
{
	DESC_HANDLE result = DESC_HANDLE(handle);
	result.ptr += increment;
	return result;
}

DESC_HANDLE operator+(DESC_HANDLE&& handle, const size_t increment)
{
	DESC_HANDLE result = std::forward<DESC_HANDLE>(handle);
	result.ptr += increment;
	return result;
}

DESC_HANDLE& operator+=(DESC_HANDLE& handle, const size_t increment)
{
	handle.ptr += increment;
	return handle;
}

