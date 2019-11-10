#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include <random>
#include <chrono>
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
private: 
	const int MAX = 20000;
	const int MIN = -20000;

	int depth;
	int max_depth;
	bool stop;
	Move bestMove;
	chrono::_V2::system_clock::time_point move_start;
	chrono::_V2::system_clock::time_point game_start;
	
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);

	int searchMin(int a);
	int searchMax(int b);
};

#endif //STUDENTAI_H
