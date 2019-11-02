#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
private: 
	int depth;
	int max_depth;
	Move bestMove;
	
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);

	int searchMin(int a);
	int searchMax(int b);
};

#endif //STUDENTAI_H
