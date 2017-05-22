#include "GameMaker.h"
#include <iostream>
#include "GameUtils.h"
#include "PrintHandler.h"

GameMaker::~GameMaker()
{
	delete _playerA;
	delete _playerB;
}

#define TECH_LOSS_CURR_PLAYER   (currentPlayerDef == PLAYER_A) ? (scorePlayerB = maxScoreB) : (scorePlayerA = maxScoreA); break;
#define TECH_LOSS_A				scorePlayerB = maxScoreB; break;
#define TECH_LOSS_B				scorePlayerA = maxScoreA; break;

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

	//general parameters
	std::pair<int, int> attackPosition;
	int maxScoreA = _board.GetMaxScore(PLAYER_A);
	int maxScoreB = _board.GetMaxScore(PLAYER_B);

	PrintHandler::printInitialBoard(_board);

	// game loop
	while (remainingShipsA > 0 && remainingShipsB > 0 && (movesRemainingA || movesRemainingB))
	{
		PrintHandler::delay();

		// get attack from player and play it on the board
		__try
		{
			attackPosition = currentPlayer->attack();
		}
		__except(TRUE)
		{
			TECH_LOSS_CURR_PLAYER
		}

		*currentPlayerMovesRemaining = (attackPosition != ATTACK_END);
		if(*currentPlayerMovesRemaining)
		{
			if(!_board.isInBoard(attackPosition.first, attackPosition.second))// Not in board -> technical lost
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
		int row = attackPosition.first, col = attackPosition.second;

		// notify the players - only if not ATTACK_END:
		if (*currentPlayerMovesRemaining)
		{
			__try
			{
				_playerA->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
			}
			__except(TRUE)
			{
				std::cout << "_playerA->notifyOnAttackResult(" << currentPlayerDef << ", " << row << ", " << col << ", " << static_cast<int>(attackResult) << ");" << std::endl;
				TECH_LOSS_A
			}// if a player crashes - give technical win to other player			
			__try
			{
				_playerB->notifyOnAttackResult(currentPlayerDef, row, col, attackResult);
			}
			__except(TRUE)
			{
				std::cout << "_playerB->notifyOnAttackResult(" << currentPlayerDef << ", " << row << ", " << col << ", " << static_cast<int>(attackResult) << ");" << std::endl;
				TECH_LOSS_B
			}// if a player crashes - give technical win to other player	
		}

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

	GameResult result(scorePlayerA, scorePlayerB);
	PrintHandler::PrintSingleGameWinner(result);
	return result;
}

