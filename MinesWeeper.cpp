#include <iostream>
#include <conio.h>
#include <time.h>
#include <random>

using namespace std;

const char HIDEN_CHAR = '*'; // show when a cell status is HIDEN
const char SELECTED_CHAR = '^'; // show when a cell status is SELECTED
const char BOMB_CHAR = '+'; // show when a cell contain bomb
const char NORMAL_CHAR = '-'; // show when a cell is empty
const int GAME_ENV_ROW = 30;
const int GAME_ENV_COL = 30;
const int bomb_count = 20; // number of bomb
const int name_size = 15;

enum GameCellType
{
	SAFE,
	BOMB
};

enum GameCellStatus
{
	HIDEN,
	PRESENTE,
	SELECTED
};

enum GameState
{
	GAME_INITIALIZE,
	USER_INFO_INPUT,
	USER_INPUT,
	GAME_OVER,
	GAME_WIN,
	GAME_END,
	SHOW_ALL,
	SHOW_OPENED,
	SHOW_WITH_NEIGBORE
};

struct Player
{
	char name[name_size];
	int points;
};

struct GameCell
{
	GameCellType cellType;
	GameCellStatus cellStatus;
	unsigned int noOfNeighborBomb; // number of bomb inside this cell
};

GameCell GameEnv[GAME_ENV_ROW][GAME_ENV_COL];


void game_initialize(const int bombCount, Player* player);
int counter_neighbor_bomb(const int ROW, const int COL);
void game_board_initialize();
void get_player_info(Player* player);
bool isValid(const int row, const int col); // check row and col number truely placed in game environment
void print_game_env(const GameState& g_state, const int ROW, const int COL);
int action_open_cell(const int row, const int col, GameState& g_state);
int apply_to_neighbor(const int ROW, const int COL);
void get_input(int& row, int& col, GameCellStatus& cellAction);
int apply_cell_status(const int ROW, const int COL, const GameCellStatus cell_status, GameState& game_state);
void action_game_over();
void action_game_win();
void action_game_end(Player* player);

int main()
{
	Player player;
	GameState game_state = GameState::GAME_INITIALIZE;
	GameCellStatus cell_state = GameCellStatus::HIDEN;
	int r, c;
	bool again = false;
	while (true)
	{
		switch (game_state)
		{
		case GAME_INITIALIZE: { // initialize game plan

			game_initialize(bomb_count, &player);

			if (again)
			{ // if player retry game
				game_state = GameState::USER_INPUT;
			}
			else
			{
				game_state = GameState::USER_INFO_INPUT;
			}

			break;
		}
		case USER_INFO_INPUT: { // input user informations
			get_player_info(&player);
			game_state = GameState::USER_INPUT;
			break;
		}
		case USER_INPUT: { // input location and action to apply
			get_input(r, c, cell_state);
			player.points += apply_cell_status(r, c, cell_state, game_state);

			if (player.points == (GAME_ENV_ROW * GAME_ENV_COL - bomb_count))
			{
				game_state = GameState::GAME_WIN;
			}
			break;
		}
		case GAME_OVER: { // player game over
			action_game_over();
			game_state = GameState::GAME_END;
			break;
		}
		case GAME_WIN: { // player game winner
			action_game_win();
			game_state = GameState::GAME_END;
			break;
		}
		case SHOW_ALL: { // show all information at end of a game
			print_game_env(game_state, r, c);
			char ch;

			cout << player.name << " you are achive " << player.points << " points" << endl;
			cout << "You want to play again ?! (y, n)  ";
			cin >> ch;
			if (ch == 'y' || ch == 'Y')
			{
				again = true;
				game_state = GameState::GAME_INITIALIZE;
			}
			else
			{
				return 0; // the end game
			}
			break;
		}
		case SHOW_OPENED: { // show game environment
			print_game_env(game_state, r, c);
			game_state = GameState::USER_INPUT;
			break;
		}
		case SHOW_WITH_NEIGBORE: { // show game environment when an cell contain zero and all neighbor open
			print_game_env(game_state, r, c);
			game_state = GameState::USER_INPUT;

			break;
		}
		case GAME_END: { // at end of game show gamer points
			action_game_end(&player);
			game_state = GameState::SHOW_ALL;

			break;
		}
		default:
			break;
		}

	}

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

void game_initialize(const int bombCount, Player* player)
{
	srand(time(0));
	int b_count = 0;
	int r, c;

	game_board_initialize();

	while (b_count < bombCount)
	{
		r = rand() % GAME_ENV_ROW;
		c = rand() % GAME_ENV_COL;

		if (GameEnv[r][c].cellType != GameCellType::BOMB)
		{
			GameEnv[r][c].cellType = GameCellType::BOMB;
			b_count++;
		}
	}

	for (int i = 0; i < GAME_ENV_ROW; i++)
	{
		for (int j = 0; j < GAME_ENV_COL; j++)
		{
			GameEnv[i][j].noOfNeighborBomb = counter_neighbor_bomb(i, j);
		}
	}

	//for (int i = 0; i < GAME_ENV_ROW; i++)
	//{
	//	for (int j = 0; j < GAME_ENV_COL; j++)
	//	{
	//		cout << GameEnv[i][j].noOfNeighborBomb << " ";
	//	}
	//	cout << endl;
	//}
	//for (int i = 0; i < GAME_ENV_ROW; i++)
	//{
	//	for (int j = 0; j < GAME_ENV_COL; j++)
	//	{
	//		if (GameEnv[i][j].cellType == GameCellType::BOMB)
	//		{
	//			cout << "b" << " ";
	//		}
	//		else
	//		{
	//			cout << "p ";
	//		}
	//	}
	//	cout << endl;
	//}
	player->points = 0;
}

int counter_neighbor_bomb(const int ROW, const int COL)
{
	int count = 0;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0 || !isValid(ROW + i, COL + j))
			{
				continue;
			}

			if (GameEnv[ROW + i][COL + j].cellType == GameCellType::BOMB)
			{
				count++;
			}
		}
	}
	return count;
}

void game_board_initialize()
{
	for (int i = 0; i < GAME_ENV_ROW; i++)
	{
		for (int j = 0; j < GAME_ENV_COL; j++)
		{
			GameEnv[i][j].cellStatus = GameCellStatus::HIDEN;
			GameEnv[i][j].cellType = GameCellType::SAFE;
			GameEnv[i][j].noOfNeighborBomb = 0;
		}
	}
}

void get_player_info(Player* player)
{
	cout << "Please enter your name : ";
	cin.getline(player->name, name_size);
}

bool isValid(const int row, const int col)
{
	// Returns true if row number and column number 
	// is in range 
	return (row >= 0) && (row < GAME_ENV_ROW) &&
		(col >= 0) && (col < GAME_ENV_COL);
}

void print_game_env(const GameState& g_state, const int ROW, const int COL)
{
	cout << "   ";
	for (int i = 0; i < GAME_ENV_COL; i++)
	{
		cout << i << " ";
		if (i <= 9)
		{
			cout << " ";
		}
	}
	cout << endl;
	for (int i = 0; i < GAME_ENV_ROW; i++)
	{
		cout << i << " ";
		if (i <= 9)
		{
			cout << " ";
		}

		for (int j = 0; j < GAME_ENV_COL; j++)
		{
			if (GameEnv[i][j].cellStatus == GameCellStatus::PRESENTE)
			{
				if (GameEnv[i][j].cellType == GameCellType::BOMB)
				{
					// show a character for bomb cell
					cout << BOMB_CHAR << " ";
				}
				else
				{
					/*if (g_state == GameState::SHOW_OPENED && ( i == ROW ) && ( j == COL ) )
					{*/
					cout << GameEnv[i][j].noOfNeighborBomb << " ";
					//}
					//else
					//{
					//	cout << NORMAL_CHAR << " ";
					//}
				}
			}
			else if (GameEnv[i][j].cellStatus == GameCellStatus::HIDEN)
			{
				cout << HIDEN_CHAR << " ";
			}
			else
			{
				cout << SELECTED_CHAR << " ";
			}


			cout << " ";

		}

		cout << endl;
	}

}

int action_open_cell(const int row, const int col, GameState& g_state)
{
	if (isValid(row, col))
	{
		if (GameEnv[row][col].cellType == GameCellType::BOMB)
		{
			g_state = GameState::GAME_OVER;
			return 0;
		}
		else
		{
			if (GameEnv[row][col].noOfNeighborBomb == 0)
			{
				int p = apply_to_neighbor(row, col);
				g_state = GameState::SHOW_WITH_NEIGBORE;
				return p;
			}
			else
			{
				GameEnv[row][col].cellStatus = GameCellStatus::PRESENTE;
				g_state = GameState::SHOW_OPENED;
				return 1;
			}
		}
	}
}

int apply_to_neighbor(const int ROW, const int COL)
{
	if (!isValid(ROW, COL))
	{
		return 0;
	}

	int p = 0;
	int q;
	if (GameEnv[ROW][COL].cellStatus != GameCellStatus::PRESENTE)
	{
		if (GameEnv[ROW][COL].noOfNeighborBomb == 0)
		{
			p++;
			GameEnv[ROW][COL].cellStatus = GameCellStatus::PRESENTE;


			for (int i = -1; i < 2; i++)
			{
				p += apply_to_neighbor(ROW - 1, COL + i);
				p += apply_to_neighbor(ROW + 1, COL + i);
			}
			p += apply_to_neighbor(ROW, COL - 1);
			p += apply_to_neighbor(ROW, COL + 1);

			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					if (i == 0 && j == 0 || !isValid(ROW + i, COL + j))
					{
						continue;
					}

					if (GameEnv[ROW + i][COL + j].cellStatus != GameCellStatus::PRESENTE)
					{
						GameEnv[ROW + i][COL + j].cellStatus = GameCellStatus::PRESENTE;
						p++;
					}
				}
			}
		}
	}

	return p;
}

void get_input(int& row, int& col, GameCellStatus& cellAction)
{
	do
	{
		do
		{
			cout << "Please Enter an Location (x, y) >>> " << endl;
			cout << "  x = ";
			cin >> row;
			cout << "  y = ";
			cin >> col;
			system("cls");
		} while (!isValid(row, col));

		if (GameEnv[row][col].cellStatus == GameCellStatus::PRESENTE)
		{
			cout << "This selection is presented. Please select another." << endl;;
		}
		else
		{
			break;
		}
	} while (true);

	int sel;
	char ch;
	bool help = true;

	while (help)
	{

		do
		{
			cout << "You are select location (" << row << ", " << col << "). Please select an action >>> " << endl;
			cout << "  1. Open selectet location ( press 1 key on keyboard ) " << endl;
			cout << "  2. Mark as selected location ( press 2 key on keyboard ) " << endl;
			cout << "Choose your selection action : ";
			cin >> sel;
			system("cls");
		} while (sel != 1 && sel != 2);

		if (sel == 1)
		{
			if (GameEnv[row][col].cellStatus == GameCellStatus::SELECTED)
			{
				cout << "You Want To Open an Selected Cell (y or n)? ";
				cin >> ch;

				if (ch == 'y' || ch == 'Y')
				{
					help = false;
				}
				else
				{
					continue;
				}
			}
			help = false;
			cellAction = GameCellStatus::PRESENTE;
		}
		else
		{
			help = false;
			cellAction = GameCellStatus::SELECTED;
		}
	}

}

int apply_cell_status(const int ROW, const int COL, const GameCellStatus cell_status, GameState& game_state)
{
	int p = 0;
	if (cell_status == GameCellStatus::SELECTED)
	{
		GameEnv[ROW][COL].cellStatus = cell_status;
		game_state = GameState::SHOW_OPENED;
	}
	else
	{
		p = action_open_cell(ROW, COL, game_state);
	}
	return p;
}

void action_game_over()
{
	cout << "^^^^^^^^^^^^BOMB^^^^^^^^^^^^^^^^^^" << endl;
	cout << "^^^^^^^^^Game Over^^^^^^^^^^^^^^^^" << endl;

}

void action_game_win()
{
	cout << "^^^^^^^^^^^Vectory^^^^^^^^^^^" << endl;
	cout << "^^^^^^^^You Are WINNER^^^^^^^" << endl;
}

void action_game_end(Player* player)
{
	int point = 0;

	for (int i = 0; i < GAME_ENV_ROW; i++)
	{
		for (int j = 0; j < GAME_ENV_COL; j++)
		{
			if (GameEnv[i][j].cellStatus == GameCellStatus::PRESENTE && GameEnv[i][j].cellType != GameCellType::BOMB)
			{
				point++;
			}
			else
			{
				GameEnv[i][j].cellStatus = GameCellStatus::PRESENTE;
			}
		}
	}

	player->points = point;
}
