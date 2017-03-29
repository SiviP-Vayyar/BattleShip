#include "Player.h"


Player::Player()
{
	_myBoard = GameBoard();
	_opponentBoard = GameBoard();

}

Player::~Player()
{
	
}


void Player::setBoard(const char** board, int numRows, int numCols)
{
	_myBoard.setBoard(board, numRows, numCols);

}

std::pair<int, int> Player::attack()
{
	
	return std::make_pair(1, 1); //TODO: implelemnt
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	
}