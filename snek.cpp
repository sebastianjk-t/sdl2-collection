#include <SDL.h>
#include <vector>
#include <queue>

using namespace std;

vector<vector<int>> addBerry(vector <vector <int>>& grid, bool isPortal)
{
	srand(time(NULL));

	vector<vector<int>> portal;
	int head[2] = {rand() % (int) (grid.size() - 2) + 1, rand() % (int) (grid[0].size() - 2) + 1};
	
	while (grid[head[0]][head[1]]) // while chosen cell isn't free, rerandomise
	{
		head[0] = rand() % (grid.size() - 2) + 1;
		head[1] = rand() % (grid[0].size() - 2) + 1;
	}
	
	portal.push_back({head[0], head[1]});
		
	grid[head[0]][head[1]] = 2 + isPortal;
	
	if (isPortal) // if berry is portal, repeat and add to portal vector
	{
		while (grid[head[0]][head[1]])
		{
			head[0] = rand() % (grid.size() - 2) + 1;
			head[1] = rand() % (grid[0].size() - 2) + 1;
		}
		
		portal.push_back({head[0], head[1]});
			
		grid[head[0]][head[1]] = 3;
	}
	
	return portal;
}

void render(vector <vector <int>>& grid, int scale, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	
	SDL_Rect rect;
	
	rect.w = scale;
	rect.h = scale;
	rect.x = 0;
	
	for (int x = 0; x < grid.size(); x++)
	{
		rect.y = 0;

		for (int y = 0; y < grid[x].size(); y++)
		{
			if (grid[x][y])
			{
				switch (grid[x][y])
				{
					case 1:
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
						break;
						
					case 2:
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
						break;
						
					case 3:
						SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
				}

				SDL_RenderFillRect(renderer, &rect);
			}
			
			rect.y += scale;
		}
		
		rect.x += scale;
	}
	
	SDL_RenderPresent(renderer);
}

int main()
{
	int size[2] = {16, 16};
	int scale = 768 / max(size[0], size[1]);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Renderer* renderer = SDL_CreateRenderer(SDL_CreateWindow("snek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size[0] * scale, size[1] * scale, 0), -1, 0);
	SDL_Event event;

	int head[2] = {size[0] / 2 - 1, size[1] / 2 - 1};
	int dir[2] = {1, 0};
	int lvl = 1, portalTimer = 0;
	const Uint8* keys;

	vector <vector <int>> grid(size[0], vector<int>(size[1], 0)), portal;
	queue <vector <int>> snek;

	grid[head[0]][head[1]] = 1;
	snek.push({head[0], head[1]});

	addBerry(grid, false);

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
					
					if (!grid[x / scale][y / scale])
						grid[x / scale][y / scale] = 2;
					
					break;
					
				case SDL_KEYDOWN:

					keys = SDL_GetKeyboardState(NULL);

					if (keys[SDL_SCANCODE_ESCAPE])
						return 0;
			
					if (!dir[0]) // if not moving sideways, get left / right input
					{
						if ((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) - (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]))
						{
							dir[0] = (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) - (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]);
							dir[1] = 0;
						}
					}
					else // get up / down input
					{	
						if ((keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) - (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]))
						{
							dir[0] = 0;
							dir[1] = (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) - (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]);
						}
					}
			}
		}

		// wrap around

		if ((head[0] = (snek.back()[0] + dir[0]) % size[0]) < 0)
			head[0] = size[0] - 1;
		
		if ((head[1] = (snek.back()[1] + dir[1]) % size[1]) < 0)
			head[1] = size[1] - 1;

		// tail shortening
		
		if (grid[head[0]][head[1]] == 2) // if reached berry, respawn (keeping tail)
			portal = addBerry(grid, snek.size() >= pow(2, lvl) - 1);
		else // shorten tail
		{
			grid[snek.front()[0]][snek.front()[1]] = 0;
			snek.pop();
		}
			
		// head lengthening

		if (grid[head[0]][head[1]] == 1) // if hit itself, quit
			return 0;
		else if (grid[head[0]][head[1]] == 3) // if reached portal, teleport
		{
			portalTimer = snek.size() + 2;
			lvl++;
			
			if (portal[0][0] == head[0] && portal[0][1] == head[1])
			{
				grid[portal[1][0] + dir[0]][portal[1][1] + dir[1]] = 1;
				snek.push({portal[1][0] + dir[0], portal[1][1] + dir[1]});
			}
			else
			{
				grid[portal[0][0] + dir[0]][portal[0][1] + dir[1]] = 1;
				snek.push({portal[0][0] + dir[0], portal[0][1] + dir[1]});
			}
		}
		else // lengthen appropriately
		{
			grid[head[0]][head[1]] = 1;
			snek.push({head[0], head[1]});
		}

		if (portalTimer) // if portal is open, decrement timer
		{
			portalTimer--;

			if (!portalTimer) // if reached 0, respawn berry and destroy portal
			{
				addBerry(grid, false);
				
				grid[portal[0][0]][portal[0][1]] = 0;
				grid[portal[1][0]][portal[1][1]] = 0;
			}
		}
		
		render(grid, scale, renderer);
		SDL_Delay(125);
	}
}
