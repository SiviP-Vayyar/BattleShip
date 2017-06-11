#pragma once

#include <windows.h>
#include "GameMaker.h"
#include <mutex>

typedef IAlgo*(*GetAlgoFuncType)();
#define GAME_MODE_REUSE true

struct AlgoData
{
	std::string name;
	std::string algoFile;
	HINSTANCE handle;
	mutable std::mutex get_player_lock;
	mutable std::mutex match_lock;
	GetAlgoFuncType GetPlayerUnsafe;	// not thread safe
	std::shared_ptr<IAlgo> GetPlayer() const		// thread safe wrapper
	{
		std::lock_guard<std::mutex> guard(this->get_player_lock);

		if(!GAME_MODE_REUSE)
		{
			return std::shared_ptr<IAlgo>(GetPlayerUnsafe());
		}
		else
		{
			if(!_instance)
			{
				_instance = std::shared_ptr<IAlgo>(GetPlayerUnsafe());
			}
			return _instance;
		}
	}

	AlgoData() = default;
	AlgoData(const AlgoData&& other) = delete;
	AlgoData(const AlgoData& other) = delete;
	AlgoData& operator=(const AlgoData& other) = delete;
	AlgoData& operator=(const AlgoData&& other) = delete;

private:
	mutable std::shared_ptr<IAlgo> _instance = nullptr;
};
