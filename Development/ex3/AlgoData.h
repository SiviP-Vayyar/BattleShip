#pragma once

#include <windows.h>
#include "GameMaker.h"
#include <mutex>

typedef IAlgo*(*GetAlgoFuncType)();
#define GAME_MODE_REUSE false

struct AlgoData
{
	std::string name;
	std::string algoFile;
	HINSTANCE handle;
	mutable std::mutex get_player_lock;
	GetAlgoFuncType GetPlayerUnsafe;	// not thread safe
	IAlgo* AcquirePlayer() const		// thread safe wrapper
	{
		//std::lock_guard<std::mutex> guard(this->get_player_lock);
		get_player_lock.lock();
		if (!_instance)
		{
			_instance = GetPlayerUnsafe();
		}
		return _instance;
	}
	void ReleasePlayer() const
	{
		if (modeReusePlayers)
		{
			ClearPlayer();
		}
		get_player_lock.unlock();
	}
	void ClearPlayer() const
	{
		delete _instance;
		_instance = nullptr;
	}

	AlgoData() = default;
	//AlgoData(const AlgoData& other) 
	//: name(other.name), algoFile(other.algoFile), handle(other.handle) { GetPlayerUnsafe = other.GetPlayerUnsafe; }
	//AlgoData& operator=(const AlgoData& other)
	//{
	//	name = other.name;
	//	algoFile = other.algoFile;
	//	handle = other.handle;
	//	GetPlayerUnsafe = other.GetPlayerUnsafe;
	//	return *this;
	//	// TODO: mutex is not copied - consider a static mutex vector and pass refrences for copies
	//}

private:
	mutable IAlgo* _instance = nullptr;
	bool modeReusePlayers = GAME_MODE_REUSE;
};
