#include "StudentAI.h"

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
	game_start = chrono::high_resolution_clock::now();
}

// 49.11 avg moves in a checkers game
// 8 min time limit = 480 seconds
// ~9.774 second limit per move
Move StudentAI::GetMove(Move move)
{
	move_start = chrono::high_resolution_clock::now();
	if (move.seq.empty())
    {
        player = 1;
		cout << "\nI am player one\n";
    } else {
        board.makeMove(move,player == 1?2:1);
		cout << "\nI am player " << player << "\n";
    }

	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	auto current_time = chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = current_time - game_start;

	if (elapsed > chrono::seconds{470})
	{
		int i = rand() % (moves.size());
		vector<Move> checker_moves = moves[i];
		int j = rand() % (checker_moves.size());
		Move res = checker_moves[j];
		board.makeMove(res,player);
		return res;
	}
	bestMove = moves[0][0];
	if (moves.size() == 1 && moves[0].size() == 1)
	{
		board.makeMove(bestMove, player);
		auto move_finish = chrono::high_resolution_clock::now();
		elapsed = move_finish - move_start;
		std::cout << "Elapsed time: " << elapsed.count() << " s\n";
		return bestMove;
	}
	depth = 0;
	max_depth = 0;
	stop = false;
	while (!stop)
	{
		searchMax(MAX);
		max_depth++;
	}
	board.makeMove(bestMove, player);
	auto move_finish = chrono::high_resolution_clock::now();
	elapsed = move_finish - move_start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
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
	auto move_finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = move_finish - move_start;
	if (elapsed >= chrono::seconds{9})
	{
		stop = true;
		return 0;
	}
	int alpha = a;
	int beta = MAX;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	if (player == board.isWin(player))
	{
		return MIN;
	}
	if (board.isWin(player) != 0)
	{
		return MAX;
	}
	for (int i = 0; i < moves.size(); ++i)
	{
		for (int j = 0; j < moves[i].size(); ++j)
		{
			board.makeMove(moves[i][j], player);
			player = (player == 1 ? 2 : 1);
			++depth;
			int b = searchMax(beta);
			if (stop)
			{
				board.Undo();
				player = (player == 1 ? 2 : 1);
				return 0;
			}
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
	auto move_finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = move_finish - move_start;
	if (elapsed >= chrono::seconds{ 9 })
	{
		stop = true;
		return 0;
	}
	int alpha = MIN;
	int beta = b;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	if (player == board.isWin(player))
	{
		return MAX;
	}
	if (board.isWin(player) != 0)
	{
		return MIN;
	}
	Move newBest = moves[0][0];
	for (int i = 0; i < moves.size(); ++i)
	{
		for (int j = 0; j < moves[i].size(); ++j)
		{
			board.makeMove(moves[i][j], player);
			++depth;
			player = (player == 1 ? 2 : 1);
			int a = searchMin(alpha);
			if (stop)
			{
				board.Undo();
				player = (player == 1 ? 2 : 1);
				return 0;
			}
			if (a > beta)
			{
				board.Undo();
				player = (player == 1 ? 2 : 1);
				--depth;
				return a;
			}
			else if (a > alpha)
			{
				alpha = a;
				if (depth == 0) newBest = moves[i][j];
			}
			board.Undo();
			player = (player == 1 ? 2 : 1);
			--depth;
		}
	}
	if (depth == 0) bestMove = newBest;	// depth 0 will always return here
	return alpha;
}

