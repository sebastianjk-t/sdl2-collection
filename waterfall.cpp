#include <fstream>
#include <SDL.h>
#include <vector>
#include <string>

using namespace std;

void resetSDL(vector <string>& hill, int& scale, SDL_Window*& window, SDL_Renderer*& renderer)
{
	scale = 768 / max(hill.size(), hill[0].length());

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	
	window = SDL_CreateWindow("waterfall", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, hill.size() * scale, hill[0].length() * scale, 0);
	renderer = SDL_CreateRenderer(window, -1, 0);
}

void makeHill(vector <string>& hill, vector <vector <int>>& water, int& scale, SDL_Window*& window, SDL_Renderer*& renderer)
{
	ifstream fin("hill.txt");
	string line;

	hill.clear();
	water.clear();
	
	for (int y = 0; getline(fin, line); y++) // read in line (horizontal)
	{
		for (int x = 0; x < line.length(); x++) // add each character to different column
		{
			if (!y)
				hill.push_back(string(1, line[x]));
			else
				hill[x] += line[x];

			if (line[x] == '*')
				water.push_back({x, y});
		}
	}

	resetSDL(hill, scale, window, renderer);
}

void resetHill(vector <string>& hill, int& scale, SDL_Window*& window, SDL_Renderer*& renderer)
{
	srand(time(NULL));

	int size[2] = {rand() % 17 + 8, rand() % 17 + 8};
	
	hill.clear();
	hill.resize(size[0], string(size[1], '.'));

	for (int x = 0; x < hill.size(); x++)
	{
		for (int y = 0; y < hill[x].length(); y++)
		{
			if (!x || x == hill.size() - 1 || !y || y == hill[x].length() - 1 || !(rand() % 8))
				hill[x][y] = 'o';
		}
	}

	resetSDL(hill, scale, window, renderer);
}

void resetWaterfall(vector<string>& hill)
{
	for (int x = 0; x < hill.size(); x++)
		for (int y = 0; y < hill[0].size(); y++)
			if (hill[x][y] == '*')
				hill[x][y] = '.';
}

void updateWater(vector<string>& hill, vector <vector <int>>& water)
{
	vector <vector <int>> newWater;

	for (vector<int> pos : water)
	{
		hill[pos[0]][pos[1]] = '*';
		
		if (hill[pos[0]][pos[1] + 1] == '.') // if free cell below
			newWater.push_back({pos[0], pos[1] + 1}); // add to water vector
		else if (hill[pos[0]][pos[1] + 1] == 'o') // below cell is filled
		{
			if (hill[pos[0] - 1][pos[1]] == '.') // if free cell to the left
				newWater.push_back({pos[0] - 1, pos[1]});
			
			if (hill[pos[0] + 1][pos[1]] == '.') // if free cell to the right
				newWater.push_back({pos[0] + 1, pos[1]});
		}
	}
	
	water = newWater;
	newWater.clear();
}

void render(vector<string>& hill, int scale, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	SDL_Rect rect;
	rect.w = scale;
	rect.h = scale;
	rect.x = 0;
	
	for (int x = 0; x < hill.size(); x++)
	{
		rect.y = 0;

		for (int y = 0; y < hill[x].length(); y++)
		{
			switch (hill[x][y])
			{
				case 'o':
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					break;
				case '.':
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					break;
				case '*':
					SDL_SetRenderDrawColor(renderer, 0, 128 + 127 * ((float) (hill[0].length() - y + rand() % 2) / (float) hill[0].length()), 255, 255);
			}
			
			SDL_RenderFillRect(renderer, &rect);
			
			rect.y += scale;
		}
		
		rect.x += scale;
	}
	
	SDL_RenderPresent(renderer);
}

int main()
{
	vector <string> hill;
	vector <vector <int>> water;

	int scale;
	int delay = 125;
	const Uint8* keys;
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Event event;

	makeHill(hill, water, scale, window, renderer);
	
	while (true)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					return 0;

				case SDL_MOUSEBUTTONDOWN:

					int x, y;
					SDL_GetMouseState(&x, &y);
					water.push_back({x / scale, y / scale});
					break;
					
				case SDL_KEYDOWN:
					
					keys = SDL_GetKeyboardState(NULL);
					
					if (keys[SDL_SCANCODE_ESCAPE])
						return 0;
					
					if (keys[SDL_SCANCODE_SPACE])
					{
						resetHill(hill, scale, window, renderer);
						water.clear();
					}
					
					if (keys[SDL_SCANCODE_BACKSPACE])
					{
						resetWaterfall(hill);
						water.clear();
					}

					if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
						delay *= 2;
					else if ((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) && delay > 1)
						delay /= 2;

			}
		}
		
		updateWater(hill, water);
		render(hill, scale, renderer);
		SDL_Delay(delay);
	}
}
