#include "GameMaker.h"
#include "GameException.h"
#include <iostream>
#include "GameUtils.h"
#include "PrintHandler.h"

GameMaker::~GameMaker()
{
	delete _playerA;
	delete _playerB;
}
/*@pre: assume players and board were set and validated*/
GameResult GameMaker::RunGame()
{
	// counters and flags
	int scorePlayerA = 0, scorePlayerB = 0;
	int remainingShipsA = MAX_SHIPS, remainingShipsB = MAX_SHIPS;
	bool movesRemainingA = true, movesRemainingB = true;

	// alternating variables for game loop
	auto currentPlayerDef = PLAYER_A;
	auto currentPlayer = _playerA;
	auto currentPlayerMovesRemaining = &movesRemainingA;
	auto currentShipsCntr = &remainingShipsA;
	auto opponentShipsCntr = &remainingShipsB;
	auto currentScore = &scorePlayerA;
	auto opponentScore = &scorePlayerB;

	PrintHandler::printInitialBoard(_board);

	// game loop
	while (remainingShipsA > 0 && remainingShipsB > 0 && (movesRemainingA || movesRemainingB))
	{
		PrintHandler::delay();

		// get attack from player and play it on the board
		auto attackPosition = currentPlayer->attack();
		*currentPlayerMovesRemaining = (attackPosition != ATTACK_END);
		auto attackResultInfo = _board.attack(attackPosition);
		auto attackResult = attackResultInfo.first;
		char attackedPiece = attackResultInfo.second;
		bool selfHit = GameBoard::isPlayerShip(currentPlayerDef, attackedPiece);
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players:
		_playerA->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
		_playerB->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);

		PrintHandler::printAttackResult(attackPosition, attackResult, attackedPiece, currentPlayerDef);

		// upon hit player gets another turn (if he didn't shoot himself)
		if (attackResult == AttackResult::Hit && !selfHit)
		{
			continue;
		}
		// upon sink player gets another turn (if he didn't shoot himself) + update ships counter and score
		if (attackResult == AttackResult::Sink)
		{
			auto shipsCntr = selfHit ? currentShipsCntr : opponentShipsCntr;
			auto score = selfHit ? opponentScore : currentScore;
			*shipsCntr = *shipsCntr - 1;
			*score = *score + GameBoard::getShipScore(attackedPiece);
			if (!selfHit)
				continue;
		}

		// switch players for next round
		currentPlayerDef = currentPlayerDef == PLAYER_A ? PLAYER_B : PLAYER_A;
		currentPlayer = currentPlayer == _playerA ? _playerB : _playerA;
		currentPlayerMovesRemaining = currentPlayerMovesRemaining == &movesRemainingA ? &movesRemainingB : &movesRemainingA;
		std::swap(currentShipsCntr, opponentShipsCntr);
		std::swap(currentScore, opponentScore);
	}


	//// print end game results // TODO: see what about it
	//PrintHandler::cleanOutput();
	//if (remainingShipsA == 0 || remainingShipsB == 0)
	//{
	//	std::cout << "Player " << (remainingShipsA == 0 ? 'B' : 'A') << " won" << std::endl;
	//}
	//std::cout << "Points:" << std::endl;
	//std::cout << "Player A: " << scorePlayerA << std::endl;
	//std::cout << "Player B: " << scorePlayerB << std::endl;


	//TODO: if a player crashes - give technical win to other player

	return GameResult(scorePlayerA, scorePlayerB);
}

