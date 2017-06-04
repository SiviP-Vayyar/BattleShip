#pragma once

#include <windows.h>
#include "GameMaker.h"
#include <mutex>

typedef IAlgo*(*GetAlgoFuncType)();

struct AlgoData
{
	std::string name;
	std::string algoFile;
	HINSTANCE handle;
	mutable std::mutex get_player_lock;
	GetAlgoFuncType GetPlayerUnsafe;	// not thread safe
	IAlgo* GetPlayer() const			// thread safe wrapper
	{
		std::lock_guard<std::mutex> guard(this->get_player_lock);
		return this->GetPlayerUnsafe();
	}

	AlgoData() = default;
	AlgoData(const AlgoData& other) 
	: name(other.name), algoFile(other.algoFile), handle(other.handle) { GetPlayerUnsafe = other.GetPlayerUnsafe; }
	AlgoData& operator=(const AlgoData& other)
	{
		name = other.name;
		algoFile = other.algoFile;
		handle = other.handle;
		GetPlayerUnsafe = other.GetPlayerUnsafe;
		return *this;
		// TODO: mutex is not copied - consider a static mutex vector and pass refrences for copies
	}
};
