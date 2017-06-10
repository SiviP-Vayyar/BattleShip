#include "GameMaker.h"
#include <iostream>
#include "GameUtils.h"
#include "PrintHandler.h"

GameMaker::~GameMaker()
{
	delete _playerA;
	delete _playerB;
}

#define TECH_LOSS_CURR_PLAYER   (currentPlayerDef == PLAYER_A) ? (result.scoreB = maxScoreB) : (result.scoreA = maxScoreA); break;
#define TECH_LOSS_A				result.scoreB = maxScoreB; break;
#define TECH_LOSS_B				result.scoreB = maxScoreA; break;

/*@pre: assume players and board were set and validated*/
GameResult GameMaker::RunGame()
{
	GameResult result;

	// counters and flags
	int remainingShipsA = MAX_SHIPS, remainingShipsB = MAX_SHIPS;
	bool movesRemainingA = true, movesRemainingB = true;

	// alternating variables for game loop
	auto currentPlayerDef = PLAYER_A;
	auto currentPlayer = _playerA;
	auto currentPlayerMovesRemaining = &movesRemainingA;
	auto currentShipsCntr = &remainingShipsA;
	auto opponentShipsCntr = &remainingShipsB;
	auto currentScore = &result.scoreA;
	auto opponentScore = &result.scoreB;
	auto currentPlayerTime = &result.timeA;

	//general parameters
	Coordinate attackPosition(-1,-1,-1);
	int maxScoreA = _board.GetMaxScore(PLAYER_A);
	int maxScoreB = _board.GetMaxScore(PLAYER_B);

	// game loop
	while (remainingShipsA > 0 && remainingShipsB > 0 && (movesRemainingA || movesRemainingB))
	{
		// get attack from player and play it on the board
		__try
		{
			std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
			attackPosition = currentPlayer->attack();
			std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
			*currentPlayerTime += (end - begin).count() / 10.0e6;
		}
		__except(TRUE)
		{
			TECH_LOSS_CURR_PLAYER
		}

		*currentPlayerMovesRemaining = !(attackPosition == ATTACK_END);
		if(*currentPlayerMovesRemaining)
		{
			if(!_board.isInBoard(attackPosition))// Not in board -> technical lost
			{
				TECH_LOSS_CURR_PLAYER //TODO: add log for technical loss
			}
		}
		else
		{
			TECH_LOSS_CURR_PLAYER //TODO: add log for technical loss
		}

		auto attackResultInfo = _board.boardAttack(attackPosition);
		auto attackResult = attackResultInfo.first;
		char attackedPiece = attackResultInfo.second;
		bool selfHit = GameBoard::isPlayerShip(currentPlayerDef, attackedPiece);

		// notify the players - only if not ATTACK_END:
		if (*currentPlayerMovesRemaining)
		{
			__try
			{
				std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
				_playerA->notifyOnAttackResult(currentPlayerDef, attackPosition, attackResult);
				std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
				result.timeA += (end - begin).count() / 10.0e6;
			}
			__except(TRUE)
			{
				std::cout << "_playerA->notifyOnAttackResult(" << currentPlayerDef << ", (" << attackPosition.row << ", " << attackPosition.col << ", " << attackPosition.depth << "), " << static_cast<int>(attackResult) << ");" << std::endl;
				TECH_LOSS_A
			}// if a player crashes - give technical win to other player			
			__try
			{
				std::chrono::time_point<std::chrono::system_clock> begin = std::chrono::system_clock::now();
				_playerB->notifyOnAttackResult(currentPlayerDef, attackPosition, attackResult);
				std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
				result.timeB += (end - begin).count() / 10.0e6;
			}
			__except(TRUE)
			{
				std::cout << "_playerB->notifyOnAttackResult(" << currentPlayerDef << ", (" << attackPosition.row << ", " << attackPosition.col << ", " << attackPosition.depth << "), " << static_cast<int>(attackResult) << ");" << std::endl;
				TECH_LOSS_B
			}// if a player crashes - give technical win to other player	
		}

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
		currentPlayerTime = currentPlayerTime == &result.timeA ? &result.timeB : &result.timeA;
		std::swap(currentShipsCntr, opponentShipsCntr);
		std::swap(currentScore, opponentScore);
	}

	return result;
}

