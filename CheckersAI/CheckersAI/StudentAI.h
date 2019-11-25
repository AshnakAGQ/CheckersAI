#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#include <random>
#include <chrono>
#include <list>
#include <array>
#include <algorithm>
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
private: 
	const int MAX = 20000;		 
	const int MIN = -20000;
	const int GAME_TIME = 1200;
	const int TURN_TIME = 60;

	int depth;
	int max_depth;
	bool stop;
	Move bestMove;
	chrono::steady_clock::time_point move_start;
	chrono::duration<double> time_taken;

	std::array<std::list<chrono::duration<double>>, 20> times;
	
public:
    Board board;
	StudentAI(int col, int row, int p);
	~StudentAI();
	virtual Move GetMove(Move board);

	int searchMin(int a);
	int searchMax(int b);
};

#endif //STUDENTAI_H
