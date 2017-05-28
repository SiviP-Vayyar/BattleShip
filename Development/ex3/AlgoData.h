#pragma once

#include <windows.h>
#include "GameMaker.h"

typedef IAlgo*(*GetAlgoFuncType)();

struct AlgoData
{
	std::string name;
	std::string algoFile;
	GetAlgoFuncType GetPlayer;
	HINSTANCE handle;
};
