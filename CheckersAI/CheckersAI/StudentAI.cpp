#include "StudentAI.h"

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
	time_taken = chrono::seconds{0};
}

StudentAI::~StudentAI()
{
	cout << "Average times for each depth level:\n";
	for (int i = 0; i < 20; ++i)
	{
		cout << "Depth " << i << ":\n";
		cout << "  Occurrances: " << times[i].size() << endl;
		chrono::duration<double> avg = chrono::seconds{ 0 };
		chrono::duration<double> largest = chrono::seconds{ 0 };
		chrono::duration<double> sd = chrono::seconds{ 0 };
		for (std::list<chrono::duration<double>>::iterator it = times[i].begin(); it != times[i].end(); ++it)
		{
			avg += *it;
			largest = max(largest, *it);
		}
		avg /= times[i].size();
		for (std::list<chrono::duration<double>>::iterator it = times[i].begin(); it != times[i].end(); ++it)
		{
			sd += *it - avg;
		}
		sd /= times[i].size();
		cout << "  Average: " << (times[i].size() > 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(avg).count(): 0) << endl;
		cout << "  Max: " << (times[i].size() > 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(largest).count() : 0) << endl;
		cout << "  SD: " << (times[i].size() > 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(sd).count() : 0) << endl;
		cin.get(); 
	}

}

// 49.11 avg moves in a checkers game
// 24.56 per color
// 8 min time limit = 480 seconds
// ~19.54 second limit per move
Move StudentAI::GetMove(Move move)
{
	move_start = chrono::high_resolution_clock::now();
	if (move.seq.empty())
    {
        player = 1;
		//cout << "\nI am player one\n";
    } else {
        board.makeMove(move,player == 1?2:1);
		//cout << "\nI am player " << player << "\n";
    }

	vector<vector<Move> > moves = board.getAllPossibleMoves(player);

	if (time_taken > chrono::seconds{ GAME_TIME })
	{
		int i = rand() % (moves.size());
		vector<Move> checker_moves = moves[i];
		int j = rand() % (checker_moves.size());
		Move res = checker_moves[j];
		board.makeMove(res,player);
		auto move_finish = chrono::high_resolution_clock::now();
		time_taken += move_finish - move_start;
		return res;
	}
	bestMove = moves[0][0];
	if (moves.size() == 1 && moves[0].size() == 1)
	{
		board.makeMove(bestMove, player);
		auto move_finish = chrono::high_resolution_clock::now();
		time_taken += move_finish - move_start;
		return bestMove;
	}
	depth = 0;
	max_depth = 0;
	stop = false;
	while (!stop)
	{
		cout << "Starting search with depth " << max_depth << "...\n";
		searchMax(MAX);
		cout << "Search with depth " << max_depth << " returned " << (stop ? "unsuccessfully with " : "successfully with new ") << "best move: " << bestMove.toString() << "\n";
		auto level_finish = chrono::high_resolution_clock::now();
		if (!stop && max_depth < 20)
		{
			times[max_depth].push_back(level_finish - move_start);
		}
		max_depth++;
	}
	board.makeMove(bestMove, player);
	auto move_finish = chrono::high_resolution_clock::now();
	time_taken += move_finish - move_start;
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
	{
		/*if (depth <= 3) 
		{
			cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
			cout << "Calculated value = " << (player == 1 ? 1 : -1) * (board.whiteCount - board.blackCount) << "\n";
		}*/
		return (player == 1 ? 1 : -1) * (board.whiteCount - board.blackCount);
	}
	auto move_finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = move_finish - move_start;
	if (elapsed >= chrono::seconds{ TURN_TIME })
	{
		stop = true;
		return 0;
	}
	int alpha = a;
	int beta = MAX;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	if (player == board.isWin(player))
	{
		cout << "Found loss during searchMin!\n";
		cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
		return MIN;
	}
	else if (board.isWin(player) != 0)
	{
		cout << "Found win during searchMin!\n";
		cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
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
				//cout << "Out of time. Stopping.\n";
				board.Undo();
				player = (player == 1 ? 2 : 1);
				return 0;
			}
			if (alpha > b)
			{
				//cout << "Depth = " << depth - 1 << ": Alpha (" << alpha << ") > b (" << b << "). Trimming.\n";
				board.Undo();
				player = (player == 1 ? 2 : 1);
				--depth;
				return b;
			}
			else if (b < beta)
			{
				//cout << "Depth = " << depth - 1 << ": b (" << b << ") < Beta (" << beta << "). Updating.\n";
				beta = b;
			}
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
	{
		/*if (depth <= 3)
		{
			cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
			cout << "Calculated value = " << (player == 1 ? -1 : 1) * (board.whiteCount - board.blackCount) << "\n";
		}*/
		return (player == 1? -1 : 1) * (board.whiteCount - board.blackCount);
	}
	auto move_finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = move_finish - move_start;
	if (elapsed >= chrono::seconds{ TURN_TIME })
	{
		stop = true;
		return 0;
	}
	int alpha = MIN;
	int beta = b;
	vector<vector<Move> > moves = board.getAllPossibleMoves(player);
	if (player == board.isWin(player) || board.isWin(player) == -1)
	{
		cout << "Found win during searchMax!\n"; 
		cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
		return MAX;
	}
	else if (board.isWin(player) != 0)
	{
		cout << "Found loss during searchMax!\n";
		cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
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
				//cout << "Out of time. Stopping.\n";
				board.Undo();
				player = (player == 1 ? 2 : 1);
				return 0;
			}
			if (a > beta)
			{
				//cout << "Depth = " << depth - 1 << ": a (" << a << ") > Beta (" << beta << "). Trimming.\n";
				board.Undo();
				player = (player == 1 ? 2 : 1);
				--depth;
				return a;
			}
			else if (a > alpha)
			{
				//cout << "Depth = " << depth - 1 << ": a (" << a << ") > Alpha (" << alpha << "). Updating.\n";
				alpha = a;
				if (depth == 1)
				{
					cout << "New Best Value: " << a << " | " << "Updating newBest from " << newBest.toString() << " to " << moves[i][j].toString() << "\n";
					newBest = moves[i][j];
				}
			}
			board.Undo();
			player = (player == 1 ? 2 : 1);
			--depth;
		}
	}
	if (depth == 0) bestMove = newBest;	// depth 0 will always return here
	return alpha;
}

