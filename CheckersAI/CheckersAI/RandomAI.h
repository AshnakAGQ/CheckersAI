#ifndef RANDOMAI_H
#define RANDOMAI_H
#include "AI.h"
#include "Board.h"
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class RandomAI :public AI
{
public:
	Board board;
	RandomAI(int col, int row, int p);
	virtual Move GetMove(Move board);
};

#endif //RANDOMAI_H
