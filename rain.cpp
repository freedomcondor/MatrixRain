//#include<stdio.h>
#include<iostream>
#include<math.h>
#include <unistd.h>
using namespace std;

// ---------- terminal cursor and screen ----------------
int clear_screen()
{
	cout << "\033[2J";
	return 0;
}

int loc_print(int row, int col, char ch)
{
	cout << "\033[" << row << ";" << col << "H" << ch;
	return 0;
}

// ---------- line stream ----------------
class Line
{
	private:
	int screen_width, screen_height, short_limit, long_limit;
	bool** log;
	int col, up_row, down_row;
	int new_row()
	{
		int a = floor(rand() * 1.0 / RAND_MAX * screen_width);
		while ((*log)[a] == true)
		{
			a = floor(rand() * 1.0 / RAND_MAX * screen_width);
		}
		col = a;
		(*log)[a] = true;
		//down_row = -floor(rand() * 1.0 / RAND_MAX * screen_height);
		down_row = 0;
		int length = rand() * 1.0 / RAND_MAX * (long_limit - short_limit) + short_limit;
		up_row = down_row - length;
		return 0;
	}
	public:
	Line(int _screen_width, int _screen_height, int _short_limit, int _long_limit, bool** _log):
		screen_width(_screen_width),
		screen_height(_screen_height),
		long_limit(_long_limit),
		short_limit(_short_limit),
		log(_log),
		col(0)
	{
	}
	int step()
	{
		if (col == 0)
			new_row();
		if (up_row > screen_height)
			new_row();

		up_row++;
		down_row++;

		if ((down_row >= 1) && 
		    (down_row <= screen_height))
		{
			char c = rand() * 1.0 / RAND_MAX * (126 - 32) + 32;
			loc_print(down_row, col, c);
		}
		if (up_row > 0)
		{
			loc_print(up_row, col, ' ');
			(*log)[col] = false;
		}
		return 0;
	}
};

int main(int argc, char** argv)
{
	// get screen width and height from command line parameters
	int screen_width, screen_height;
	if (argc == 3)
	{
		sscanf(argv[1], "%d", &screen_width);
		sscanf(argv[2], "%d", &screen_height);
		screen_width--;
		screen_height--;
		screen_height--;
	}
	else
	{
		screen_width = 190;
		screen_height = 50;
	}

	clear_screen();

	bool* log = (bool*)malloc(sizeof(bool)*screen_width);
	log[0] = true;

	int lineNumber = screen_width / 5;
	int line_short_limit = 20;
	int line_long_limit = screen_height * 1.5;

	srand(time(NULL));
	Line *line[lineNumber];
	for (int i = 0; i < lineNumber; i++)
		line[i] = new Line(screen_width, screen_height, line_short_limit, line_long_limit, &log);

	for (int j = 0; j < 100000; j++)
	{
		for (int i = 0; i < lineNumber; i++)
			line[i]->step();
		cout << endl;
		usleep(100000);
	}

	free(log);

	return 0;
}