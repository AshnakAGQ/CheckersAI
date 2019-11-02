#include "StudentAI.h"
#include <random>

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
	max_depth = 2;
}

Move StudentAI::GetMove(Move move)
{
    if (move.seq.empty())
    {
        player = 1;
    } else {
        board.makeMove(move,player == 1?2:1);
    }
    vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	bestMove = moves[0][0];
	depth = 0;
	searchMax(INT_MAX);
	board.makeMove(bestMove, player);
	return bestMove;


    /*int i = rand() % (moves.size());
    vector<Move> checker_moves = moves[i];
    int j = rand() % (checker_moves.size());
    Move res = checker_moves[j];
    board.makeMove(res,player);
    return res;*/


}

// Min passes back Beta to Max, which accepts it as Alpha
// Max passes back Alpha to Min, which accepts it as Beta
// Min passes in Beta
// Max passes in Alpha
int StudentAI::searchMin(int a)
{
	if (depth == max_depth)
		return (player == 1 ? -1 : 1) * board.whiteCount - board.blackCount;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	for (int i = 0; i < moves.size(); ++i)
	{
		for (int j = 0; j < moves[i].size(); ++j)
		{
			board.makeMove(moves[i][j], player);
			player = (player == 1 ? 2 : 1);
			++depth;
			int b = searchMax(beta);
			if (alpha > b)
			{
				board.Undo();
				player = (player == 1 ? 2 : 1);
				--depth;
				return b;
			}
			else if (b < beta) beta = b;
			board.Undo();
			player = (player == 1 ? 2 : 1);
			--depth;
		}
	}
	return beta;
}

int StudentAI::searchMax(int b)
{
	if (depth == max_depth)
		return (player == 1 ? -1 : 1) * board.whiteCount - board.blackCount;
	int alpha = INT_MIN;
	int beta = INT_MAX;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	for (int i = 0; i < moves.size(); ++i)
	{
		for (int j = 0; j < moves[i].size(); ++j)
		{
			board.makeMove(moves[i][j], player);
			++depth;
			player = (player == 1 ? 2 : 1);
			int a = searchMin(alpha);
			if (a < beta)
			{
				board.Undo();
				player = (player == 1 ? 2 : 1);
				--depth;
				return a;
			}
			else if (a > alpha)
			{
				alpha = a;
				if (depth == 0) bestMove = moves[i][j];
			}
			board.Undo();
			player = (player == 1 ? 2 : 1);
			--depth;
		}
	}
	return alpha;
}

