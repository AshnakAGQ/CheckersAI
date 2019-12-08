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

/*
StudentAI::~StudentAI()
{
	cout << "Game Time: " << time_taken.count() << endl;
	cout << "Average times for each depth level:\n";
	for (int i = 0; i < 10; ++i)
	{
		cout << "Depth " << i << ":\n";
		cout << "  Occurrances: " << times[i].size() << endl;
		double avg = 0;
		double largest =  0;
		double sd = 0;
		for (std::list<chrono::duration<double>>::iterator it = times[i].begin(); it != times[i].end(); ++it)
		{
			double value = it->count();
			avg += value;
			largest = max(largest, value);
		}
		avg /= times[i].size();
		for (std::list<chrono::duration<double>>::iterator it = times[i].begin(); it != times[i].end(); ++it)
		{
			double value = it->count();
			sd += value - avg;
		}
		sd /= times[i].size();
		cout << "  Average: " << (times[i].size() > 0 ? avg: 0) << endl;
		cout << "  Max: " << (times[i].size() > 0 ? largest : 0) << endl;
		cout << "  SD: " << (times[i].size() > 0  ? sd : 0) << endl;
		cin.get(); 
	}

}
*/

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

	auto searchStart = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsedTime;
	chrono::duration<double> lastSearchTime;
	do 
	{
		searchStart = chrono::high_resolution_clock::now();
		//cout << "Starting search with depth " << max_depth << "...\n";
		searchMax(MAX);
		max_depth++;
		lastSearchTime = chrono::high_resolution_clock::now() - searchStart;
		elapsedTime = chrono::high_resolution_clock::now() - move_start;
	} while (!stop && elapsedTime + lastSearchTime < chrono::seconds{ TURN_TIME });	// Skips search if doing the search at previous depth would put over time

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
		cout << "Enemy Player = " << (player == 1 ? "Black" : "White") << endl;
		int Heuristic = (player == 1 ? -1 : 1) * heuristic() - 3;
		cout << "Calculated value = " << Heuristic << "\n";
		return Heuristic;
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
	if (player == board.isWin(player == 1 ? 2 : 1))
	{
		cout << "Player = " << (player == 1 ? "Black" : "White") << " found loss during searchMin! (" << board.isWin(player == 1 ? 2 : 1) << ")\n";
		//cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
		return MIN;
	}
	else if (board.isWin(player == 1 ? 2 : 1) != 0)
	{
		cout << "Player = " << (player == 1 ? "Black" : "White") << " found win during searchMin! (" << board.isWin(player == 1 ? 2 : 1) << ")\n";
		//cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
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
		cout << "Player = " << (player == 1 ? "Black" : "White") << endl;
		int Heuristic = (player == 1? 1 : -1) * heuristic() + 3;
		cout << "Calculated value = " << Heuristic << "\n";
		return Heuristic;
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

	if (player == board.isWin(player == 1 ? 2 : 1) || board.isWin(player == 1 ? 2 : 1) == -1)
	{
		cout << "Player = " << (player == 1 ? "Black" : "White") << " found win during searchMax! (" << board.isWin(player == 1 ? 2 : 1) << ")\n";
		//cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
		return MAX;
	}
	else if (board.isWin(player == 1 ? 2 : 1) != 0)
	{
		cout << "Player = " << (player == 1 ? "Black" : "White") << " found loss during searchMax! (" << board.isWin(player == 1 ? 2 : 1) << ")\n";
		//cout << "Player = " << (player == 1 ? "Black" : "White") << "| Black Pieces: " << board.blackCount << " White Pieces: " << board.whiteCount << "\n";
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
					//cout << "New Best Value: " << a << " | " << "Updating newBest from " << newBest.toString() << " to " << moves[i][j].toString() << "\n";
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

// Scores in favor of Black
int StudentAI::heuristic()
{
	cout << "KINGS: ";
	int value = 0;
	for (int i = 0; i < board.row; ++i)
	{
		for (int j = 0; j < board.col; ++j)
		{
			if (!board.board[i][j].isKing) 
			{
				if (board.board[i][j].color == "B")
					value += (100 + (max(board.row, board.col) + 1) / 2 - min({ board.row - 1 - i, i - 0, board.col - 1 - j, j - 0 }));
				else if (board.board[i][j].color == "W")
					value -= (100 + (max(board.row, board.col) + 1) / 2 - min({ board.row - 1 - i, i - 0, board.col - 1 - j, j - 0 }));
			}
			else if (board.board[i][j].color == "B")
			{
				// Check for trapped king
				vector<Move> moves = board.board[i][j].getPossibleMoves(&board);
				bool canMove = false;	
				for (Move move : moves)
				{
					if (move.isCapture())
					{
						canMove = true;
						break;
					}

					bool possibleMove = true;

					if ((move.seq[1].x > 0 && move.seq[1].y > 0 && board.board[move.seq[1].x - 1][move.seq[1].y - 1].toString() == "W")							// NW is King
						&& ((move.seq[1].x + 1 == i && move.seq[1].y + 1 == j)																					// and (SE is Self
						|| (move.seq[1].x < board.row - 1 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x + 1][move.seq[1].y + 1].color == ".")))	// or SE is empty)
						possibleMove = false;

					if ((move.seq[1].x > 0 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x - 1][move.seq[1].y + 1].toString() == "W")				// NE is King
						&& ((move.seq[1].x + 1 == i && move.seq[1].y - 1 == j)																					// and (SW is Self
						|| (move.seq[1].x < board.row - 1 && move.seq[1].y > 0 && board.board[move.seq[1].x + 1][move.seq[1].y - 1].color == ".")))				// or SW is empty)
						possibleMove = false;

					if ((move.seq[1].x < board.row - 1 && move.seq[1].y > 0 && board.board[move.seq[1].x + 1][move.seq[1].y - 1].color == "W")					// SW is enemy
						&& ((move.seq[1].x - 1 == i && move.seq[1].y + 1 == j)																					// and (NE is Self
						|| (move.seq[1].x > 0 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x - 1][move.seq[1].y + 1].color == ".")))				// or NE is empty)
						possibleMove = false;

					if ((move.seq[1].x < board.row - 1 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x + 1][move.seq[1].y + 1].color == "W")		// SE is enemy
						&& ((move.seq[1].x - 1 == i && move.seq[1].y - 1 == j)																					// and (NW is Self
						|| (move.seq[1].x > 0 && move.seq[1].y > 0 && board.board[move.seq[1].x - 1][move.seq[1].y - 1].color == ".")))							// or NE is empty)
						possibleMove = false;

					if (possibleMove)
					{
						canMove = true;
						break;
					}
				}
				if (!canMove) {
					cout << "b" << endl;
					board.showBoard();
				}
				else
				{
					cout << "B" << endl;
					board.showBoard();
				}
				value += (100 + (canMove ? 50 : 0) + (max(board.row, board.col) + 1) / 2 - min({ board.row - 1 - i, i - 0, board.col - 1 - j, j - 0 }));
			}
			else if (board.board[i][j].color == "W")
			{
				// Check for trapped king
				vector<Move> moves = board.board[i][j].getPossibleMoves(&board);
				bool canMove = false;
				for (Move move : moves)
				{
					if (move.isCapture())
					{
						canMove = true;
						break;
					}

					bool possibleMove = true;

					if ((move.seq[1].x > 0 && move.seq[1].y > 0 && board.board[move.seq[1].x - 1][move.seq[1].y - 1].color == "B")									// NW is King
						&& ((move.seq[1].x + 1 == i && move.seq[1].y + 1 == j)																						// and (SE is Self
							|| (move.seq[1].x < board.row - 1 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x + 1][move.seq[1].y + 1].color == ".")))	// or SE is empty)
						possibleMove = false;

					if ((move.seq[1].x > 0 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x - 1][move.seq[1].y + 1].color == "B")						// NE is King
						&& ((move.seq[1].x + 1 == i && move.seq[1].y - 1 == j)																						// and (SW is Self
							|| (move.seq[1].x < board.row - 1 && move.seq[1].y > 0 && board.board[move.seq[1].x + 1][move.seq[1].y - 1].color == ".")))				// or SW is empty)
						possibleMove = false;

					if ((move.seq[1].x < board.row - 1 && move.seq[1].y > 0 && board.board[move.seq[1].x + 1][move.seq[1].y - 1].toString() == "B")					// SW is enemy
						&& ((move.seq[1].x - 1 == i && move.seq[1].y + 1 == j)																						// and (NE is Self
							|| (move.seq[1].x > 0 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x - 1][move.seq[1].y + 1].color == ".")))				// or NE is empty)
						possibleMove = false;

					if ((move.seq[1].x < board.row - 1 && move.seq[1].y < board.col - 1 && board.board[move.seq[1].x + 1][move.seq[1].y + 1].toString() == "B")		// SE is enemy
						&& ((move.seq[1].x - 1 == i && move.seq[1].y - 1 == j)																						// and (NW is Self
							|| (move.seq[1].x > 0 && move.seq[1].y > 0 && board.board[move.seq[1].x - 1][move.seq[1].y - 1].color == ".")))							// or NE is empty)
						possibleMove = false;

					if (possibleMove)
					{
						canMove = true;
						break;
					}
				}

				if (!canMove) {
					cout << "w" << endl;
					board.showBoard();
				}
				else
				{
					cout << "W" << endl;
					board.showBoard();
				}
				value -= (100 + (canMove ? 50 : 0) + (max(board.row, board.col) + 1) / 2 - min({ board.row - 1 - i, i - 0, board.col - 1 - j, j - 0 }));
			}

		}
	}
	cout << endl;
	return value;
}

