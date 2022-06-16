#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define LEVEL 2
#define POP 0

typedef struct board* Board;

struct board {
	int grid[9];
	int level;
	int playable;
	int position;
	Board boards[9];
	Board parent;
};

Board generate_board_rec(int lev, int pos, Board par);

Board generate_board() {
	return generate_board_rec(0, 0, NULL);
}


Board generate_board_rec(int lev, int pos, Board par) {
	Board ret = (Board)malloc(sizeof(struct board));
	ret->level = lev;
	ret->position = pos;
	ret->playable = 1;
	ret->parent = par;
	for (int i = 0; i < 9; i++)
		ret->grid[i] = 0;

	if (lev < LEVEL)
		for (int i = 0; i < 9; i++)
			ret->boards[i] = generate_board_rec(lev + 1, i, ret);

	return ret;
}


int check_winner(Board checked) {
	for (int i = 0; i < 3; i++) {
		int horizontal = 0;
		int vertical = 0;

		for (int j = 0; j < 3; j++) {
			horizontal += checked->grid[i * 3 + j];
			vertical += checked->grid[i + j * 3];
		}

		switch (horizontal) {
			case 3:
				return 1;
			case -3:
				return -1;
		}

		switch (vertical) {
			case 3:
				return 1;
			case -3:
				return -1;
		}
	}

	switch (checked->grid[0] + checked->grid[4] + checked->grid[8]) {
		case 3:
			return 1;
		case -3:
			return -1;
	}
	switch (checked->grid[2] + checked->grid[4] + checked->grid[6]) {
		case 3:
			return 1;
		case -3:
			return -1;
	}

	return 0;
}


int check_playable(Board checked) {
	for (int i = 0; i < 9; i++)
		if (checked->grid[i] == 0)
			return 1;
	return 0;
}


Board play(Board played, int pos, int player) {
	played->grid[pos] = player;

	played->playable = check_playable(played);

	if (check_winner(played) && played->level) {
		played->playable = 0;
		Board ret = play(played->parent, played->position, player);
		if (played->level > 1)
			return ret;
	}

	return played;
}


void print_sym(int x, int line, int size) {
	if (x) {
		if (line < (size - 1) / 2)
			printf("%*s\\%*s/%*s", line, "", size - 2 * (line + 1), "", line, "");
		else if (line == (size - 1) / 2)
			printf("%*sX%*s", (size - 1) / 2, "", (size - 1) / 2, "");
		else
			printf("%*s/%*s\\%*s", size - line - 1, "", 2 * line - size, "", size - line - 1, "");
	} else {
		if (line == 0)
			printf("%*s_%*s", (size - 1) / 2, "", (size - 1) / 2, "");
		else if (line < (size + 1) / 2)
			printf("%*s/%*s\\%*s", (size - 2 * line - 1) / 2, "", 2 * line - 1, "", (size - 2 * line - 1) / 2, "");
		else if (line < size - 1)
			printf("%*s\\%*s/%*s", line - (size + 1) / 2, "", 2 * (size - line) - 1, "", line - (size + 1) / 2, "");
		else
			printf("%*s\\_/%*s", (size - 3) / 2, "", (size - 3) / 2, "");
	}
} //I just want to express what a shitshow writing this function turned out to be, this was absolute hell



void print_board_rec(Board printed, Board playable, int line);

void print_board(Board printed, Board playable) {
	for (int i = 0; i < (2 * pow(3, LEVEL + 1) - 1); i++) {
		print_board_rec(printed, playable, i);
		printf("\n");
	}
}

void print_board_rec(Board printed, Board playable, int line) {
	int size = 2 * pow(3, LEVEL - printed->level + 1) - 1;
	if (line == (size - 2) / 3 || line == 2 * (size - 2) / 3 + 1) {
		for (int i = 0; i < (size - 2) / 3; i++)
			printf("-");
		if (printed == playable)
			printf("@");
		else
			printf("+");
		for (int i = 0; i < (size - 2) / 3; i++)
			printf("-");
		if (printed == playable)
			printf("@");
		else
			printf("+");
		for (int i = 0; i < (size - 2) / 3; i++)
			printf("-");
	} else if (printed->level < LEVEL) {
		if (line < (size - 2) / 3) {
			for (int i = 0; i < 3; i++) {
				if (printed->grid[i])
					print_sym(printed->grid[i] == 1, line, (size - 2) / 3);
				else
					print_board_rec(printed->boards[i], playable, line);
				if (i != 2)
					printf("|");
			}
		} else if ((size - 2) / 3 < line && line < 2 * (size - 2) / 3 + 1) {
			for (int i = 3; i < 6; i++) {
				if (printed->grid[i])
					print_sym(printed->grid[i] == 1, line - (size + 1) / 3, (size - 2) / 3);
				else
					print_board_rec(printed->boards[i], playable, line - (size + 1) / 3);
				if (i != 5)
					printf("|");
			}
		} else {
			for (int i = 6; i < 9; i++) {
				if (printed->grid[i])
					print_sym(printed->grid[i] == 1, line - 2 * ((size - 2) / 3 + 1), (size - 2) / 3);
				else
					print_board_rec(printed->boards[i], playable, line - 2 * ((size - 2) / 3 + 1));
				if (i != 8)
					printf("|");
			}
		}
	} else {
		int lev;
		if (line == 0)
			lev = 0;
		else if (line == 2)
			lev = 3;
		else if (line == 4)
			lev = 6;

		for (int i = lev; i < lev + 3; i++) {
			switch (printed->grid[i]) {
				case 1:
					printf("X");
					break;
				case 0:
					printf(" ");
					break;
				case -1:
					printf("O");
					break;
			}

			if (i != lev + 2)
				printf("|");
		}
	}
} //I thought the last function was hell, how naive I was. I will never try printing 2D projects in a TUI ever again


int main() {
	int place_arr[LEVEL + 1];
	for (int i = 0; i < LEVEL + 1; i++)
		place_arr[i] = -1;
	Board full_board = generate_board();
	Board play_board;
	system("clear");
	print_board(full_board, full_board);
	int turn = 1;
	int full_arr = 0;

	int winner = 0;
	int choice = 0;
	int * pc = &choice;

	while (!(winner)) {
		scanf("%d", pc);
		play_board = full_board;
		full_arr = 1;
		for (int i = 0; i < LEVEL + 1; i++) {
			if (place_arr[i] == -1) {
				place_arr[i] = choice;

				if (i < LEVEL) {
					full_arr = 0;
					play_board = play_board->boards[place_arr[i]];
				} else if (play_board->grid[choice]) {
					place_arr[i] = -1;
					full_arr = 0;
				}
				break;
			}
			if (i < LEVEL)
				if (!(play_board->boards[place_arr[i]]->playable)) {
					full_arr = 0;
					for (int j = i; j < LEVEL + 1; j++)
						place_arr[j] = -1;
					break;
			}


			play_board = play_board->boards[place_arr[i]];
		}

		if (full_arr) {
			printf("\n");
			Board no_pop = play(play_board, place_arr[LEVEL], turn);
			if (POP) {
				for (int i = 0; i < LEVEL; i++)
					place_arr[i] = place_arr[i + 1];
				place_arr[LEVEL] = -1;
			} else {
				if (LEVEL) {
					int lev = no_pop->level;
					place_arr[lev - 1] = place_arr[lev];
					for (int i = lev; i < LEVEL + 1; i++)
						place_arr[i] = -1;
				} else {
					place_arr[0] = -1;
				}

			}

			turn *= -1;
			winner = check_winner(full_board);

			play_board = full_board;
			for (int i = 0; i < LEVEL && place_arr[i] != -1; i++) {
				if (play_board->boards[place_arr[i]]->playable)
					play_board = play_board->boards[place_arr[i]];
				else
					break;
			}
		}
		system("clear");
		print_board(full_board, play_board);
	}

	char winning_player;
	switch (winner) {
		case -1:
			winning_player = 'O';
			break;
		case 1:
			winning_player = 'X';
			break;
	}
	printf("%c has won the game!\n", winning_player);
	return 0;
}

