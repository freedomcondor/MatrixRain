#include<iostream>
#include<cmath>
#include <unistd.h> // for usleep in Unix
#include <chrono>   // for time, and for random seed
#include <vector>
using namespace std;

// ---------- terminal cursor and screen ----------------
int clear_screen()
{
	cout << "\033[2J";
	return 0;
}

int loc_print(int row, int col, char ch, int level)
{
	switch (level) {
		/*
		case 0 : cout << "\033[1;37m"; break;  // high light white
		case 1 : cout << "\033[0;37m"; break;  // white
		case 2 : cout << "\033[1m"; break;     // high light green
		case 3 : cout << "\033[1m"; break;     // high light green
		case 4 : cout << "\033[1m"; break;     // high light green
		default: cout << "\033[0m"; break;
		*/

		case 0 : cout << "\033[1;37m"; break;  // high light white
		case 1 : cout << "\033[0;37m"; break;  // white
		case 2 : cout << "\033[1m"; break;     // high light green
		case 3 : cout << "\033[0m"; break;     // green
		case 4 : cout << "\033[1;32m"; break;  // high light dark green
		default: cout << "\033[0;32m"; break;  // dark green
	}
	cout << "\033[" << row << ";" << col << "H" << ch;
	return 0;
}

// ---------- line stream ----------------
class Line
{
	private:
	int screen_width, screen_height, short_limit, long_limit;
	std::vector<bool>& log;
	int col, up_row, down_row;

	int hightlight_length;
	vector<char> last_chars;
	int step_time, step_time_left; // in u second
	int short_time_limit, long_time_limit;

	int new_row(bool init)
	{
		int a = floor(rand() * 1.0 / RAND_MAX * screen_width);
		while (log[a] == true)
		{
			a = floor(rand() * 1.0 / RAND_MAX * screen_width);
		}
		col = a;
		log[a] = true;

		if (init)
			down_row = -floor(rand() * 1.0 / RAND_MAX * screen_height);
		else
			down_row = 0;

		int length = rand() * 1.0 / RAND_MAX * (long_limit - short_limit) + short_limit;
		up_row = down_row - length;

		//# step time
		step_time = short_time_limit + floor(rand() * 1.0 / RAND_MAX * (long_time_limit - short_time_limit));
		step_time_left = step_time;
		return 0;
	}
	public:
	int getTimeLeft()
	{
		return step_time_left;
	}
	Line(int _screen_width, int _screen_height, int _short_limit, int _long_limit, int _short_time_limit, int _long_time_limit, std::vector<bool>& _log):
		screen_width(_screen_width),
		screen_height(_screen_height),
		long_limit(_long_limit),
		short_limit(_short_limit),
		hightlight_length(6),
		log(_log),
		col(0),
		long_time_limit(_long_time_limit),
		short_time_limit(_short_time_limit)
	{
		last_chars.resize(hightlight_length);
	}
	int step(int past_time)
	{
		step_time_left -= past_time;
		if (step_time_left <= 0)
		{
			step_time_left = step_time;
			step();
		}
		return 0;
	}
	int step()
	{
		if (col == 0)
			new_row(true);
		if (up_row > screen_height)
			new_row(false);

		up_row++;
		down_row++;

		for (int i=hightlight_length-1; i>=0; i--)
		{
			if ((down_row-i >= 1) &&
				(down_row-i <= screen_height))
			{
				if (i == 0)
				{
					char c = rand() * 1.0 / RAND_MAX * (126 - 32) + 32;
					last_chars[0] = c;
				}
				else
					last_chars[i] = last_chars[i-1];

				loc_print(down_row-i, col, last_chars[i], i);
			}
		}

		if (up_row > 0)
		{
			loc_print(up_row, col, ' ', -1);
			log[col] = false;
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
		//screen_width--;
		//screen_height--;
	}
	else
	{
		screen_width = 190;
		screen_height = 50;
	}

	clear_screen();

	std::vector<bool> log(screen_width, false);
	log[0] = true;

	int lineNumber = screen_width / 5;
	int line_short_limit = 20;
	int line_long_limit = screen_height * 1.5;
	int steptime_short_limit = 50000;
	int steptime_long_limit = 150000;

	// get time for random seed
	using std::chrono::duration_cast;
	using std::chrono::system_clock;
	using std::chrono::milliseconds;
	auto millisec_since_epoch =
		duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	srand(millisec_since_epoch);
	//srand(time(NULL));

	Line *line[lineNumber];
	for (int i = 0; i < lineNumber; i++)
		line[i] = new Line(screen_width, screen_height, line_short_limit, line_long_limit, steptime_short_limit, steptime_long_limit, log); // 传递 log

	while (1)
	{
		// find shortest time left
		int min_time_left = steptime_long_limit; // Initialize to maximum integer value
		for (int i = 0; i < lineNumber; i++)
			if (line[i]->getTimeLeft() < min_time_left)
				min_time_left = line[i]->getTimeLeft();

		for (int i = 0; i < lineNumber; i++)
			line[i]->step(min_time_left);
		cout << "\033[1;1H" << endl;

		usleep(min_time_left);
	}

	return 0;
}
