#include <SDL.h>
#include <vector>
#include <queue>

using namespace std;

void resetMap(vector <vector <vector <vector <bool>>>>& map, int size[2], int mapSize[2], int pos[2], int screen[2])
{
	map.clear();
	map.resize(mapSize[0], vector <vector <vector <bool>>>(mapSize[1], vector <vector <bool>>(size[0], vector<bool>(size[1], true))));

	for (int col = 0; col < mapSize[0]; col++)
	{
		for (int row = 0; row < mapSize[1]; row++)
		{
			for (int x = 1; x < size[0] - 1; x++)
			{
				for (int y = 1; y < size[1] - 1; y++)
					map[col][row][x][y] = !(rand() % 4);
			}
			
			if (col) // if not left edge
			{
				map[col][row][0][size[1] / 2] = false;
				map[col][row][0][size[1] / 2 - 1] = false;
			}
			else if (col < mapSize[0] - 1) // if not right edge
			{
				map[col][row][size[0] - 1][size[1] / 2] = false;
				map[col][row][size[0] - 1][size[1] / 2 - 1] = false;
			}
			
			if (row) // if not top edge
			{
				map[col][row][size[0] / 2][0] = false;
				map[col][row][size[0] / 2 - 1][0] = false;
			}
			else if (row < mapSize[1] - 1) // if not bottom edge
			{
				map[col][row][size[0] / 2][size[1] - 1] = false;
				map[col][row][size[0] / 2 - 1][size[1] - 1] = false;
			}
		}
	}
	
	map[screen[0]][screen[1]][pos[0]][pos[1]] = false;
}

void addEnemies(vector <vector <int>>& enemies, vector <vector <bool>> screen, int pos[2], int n)
{
	srand(time(NULL));

	enemies.clear();
	
	for (int i = 0; i < n; i++)
	{
		enemies.push_back({1 + (rand() % (int) (screen.size() - 2)), 1 + (rand() % (int) (screen[0].size() - 2)), 256});
	
		while ((enemies.back()[0] == pos[0] && enemies.back()[1] == pos[1]) || screen[enemies.back()[0]][enemies.back()[1]])
		{
			enemies.back()[0] = 1 + (rand() % (screen.size() - 2));
			enemies.back()[1] = 1 + (rand() % (screen[0].size() - 2));
		}
	}
}

void renderScreen(vector <vector <bool>> screen, int scale, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_Rect rect;

	rect.w = scale;
	rect.h = scale;
	
	rect.x = 0;

	for (int x = 0; x < screen.size(); x++)
	{	
		rect.y = 0;

		for (int y = 0; y < screen[0].size(); y++)
		{
			if (screen[x][y])
				SDL_RenderFillRect(renderer, &rect);
			
			rect.y += scale;
		}
		
		rect.x += scale;
	}
}

void visualiseBFS(vector<int>& proj, vector <vector <bool>> screen, vector <vector <int>> visited, int scale, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	
	renderScreen(screen, scale, renderer);

	SDL_Rect rect;

	rect.w = scale;
	rect.h = scale;
	
	for (int i = 0; i < visited.size(); i++)
	{
		rect.x = visited[i][0] * scale;
		rect.y = visited[i][1] * scale;
		
		SDL_SetRenderDrawColor(renderer, 0, min(511 - (i + 32) * 4, 255), min((i + 32) * 4, 255), 255);
		SDL_RenderFillRect(renderer, &rect);
	}
	
	rect.x = proj[0] * scale;
	rect.y = proj[1] * scale;
	
	SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void bfs(vector<int>& proj, vector <vector <int>> enemies, vector <vector <bool>> screen, SDL_Renderer* renderer, int scale = 0)
{
	if (enemies.empty())
		return;
		
	for (vector<int> enemy : enemies)
		if (proj[0] == enemy[0] && proj[1] == enemy[1])
			return;
	
	queue <vector <int>> queue;
	vector <vector <int>> visited;
	vector <vector <bool>> seen(screen.size(), vector <bool>(screen[0].size(), false));
	vector <vector <vector <int>>> parent(screen.size(), vector <vector<int>>(screen[0].size(), {-1, -1}));

	queue.push({proj[0], proj[1]});
	seen[proj[0]][proj[1]] = true;

	vector <int> curr, next;
	bool found = false;

	while (!queue.empty() && !found)
	{
		curr = queue.front();
		queue.pop();

		visited.push_back(curr);
		
		if (scale) // if set, visualise bfs
		{
			visualiseBFS(proj, screen, visited, scale, renderer);
			SDL_RenderPresent(renderer);
			SDL_Delay(125);
		}
		
		// check left

		if (curr[0] > 0 && !screen[curr[0] - 1][curr[1]] && !seen[curr[0] - 1][curr[1]])
		{
			next = curr;
			next[0]--;
			
			parent[next[0]][next[1]] = curr;
			
			for (vector<int> enemy : enemies)
				if (next[0] == enemy[0] && next[1] == enemy[1])
					found = true;
			
			if (found)
				break;
			
			queue.push(next);
			seen[next[0]][next[1]] = true;
		}
		
		// check right

		if (curr[0] < screen.size() - 1 && !screen[curr[0] + 1][curr[1]] && !seen[curr[0] + 1][curr[1]])
		{
			next = curr;
			next[0]++;
			
			parent[next[0]][next[1]] = curr;
			
			for (vector<int> enemy : enemies)
				if (next[0] == enemy[0] && next[1] == enemy[1])
					found = true;
			
			if (found)
				break;
			
			queue.push(next);
			seen[next[0]][next[1]] = true;
		}

		// check up
		
		if (curr[1] > 0 && !screen[curr[0]][curr[1] - 1] && !seen[curr[0]][curr[1] - 1])
		{
			next = curr;
			next[1]--;
			
			parent[next[0]][next[1]] = curr;
			
			for (vector<int> enemy : enemies)
				if (next[0] == enemy[0] && next[1] == enemy[1])
					found = true;
			
			if (found)
				break;
			
			queue.push(next);
			seen[next[0]][next[1]] = true;
		}

		// check down
		
		if (curr[1] < screen[0].size() - 1 && !screen[curr[0]][curr[1] + 1] && !seen[curr[0]][curr[1] + 1])
		{
			next = curr;
			next[1]++;
			
			parent[next[0]][next[1]] = curr;
			
			for (vector<int> enemy : enemies)
				if (next[0] == enemy[0] && next[1] == enemy[1])
					found = true;
			
			if (found)
				break;
			
			queue.push(next);
			seen[next[0]][next[1]] = true;
		}
	}
	
	if (!found)
		return;
	
	if (scale) // if set, visualise bfs
	{
		while (!queue.empty())
		{
			curr = queue.front();
			queue.pop();
	
			visited.push_back(curr);
			
			visualiseBFS(proj, screen, visited, scale, renderer);
			SDL_RenderPresent(renderer);
			SDL_Delay(125);
		}
		
		visualiseBFS(proj, screen, visited, scale, renderer);
		
		SDL_Rect rect;

		rect.w = scale;
		rect.h = scale;

		rect.x = next[0] * scale;
		rect.y = next[1] * scale;
		
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);

		SDL_RenderPresent(renderer);
		SDL_Delay(250);
	}
	
	// backtrack and set direction

	while (parent[next[0]][next[1]][0] != proj[0] || parent[next[0]][next[1]][1] != proj[1])
		next = parent[next[0]][next[1]];
	
	proj[2] = next[0] - proj[0];
	proj[3] = next[1] - proj[1];
}

void render(vector <vector <bool>> screen, int pos[2], int dir[2], vector <vector <int>> enemies, vector <vector <int>> projs, int health, int maxHealth, int hitTimer, int swordTimer, int scale, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	
	SDL_RenderClear(renderer);

	SDL_Rect rect;
	rect.w = scale;
	rect.h = scale;

	renderScreen(screen, scale, renderer);
	
	// render enemies
	
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	
	for (vector <int> enemy : enemies)
	{
		rect.x = enemy[0] * scale;
		rect.y = enemy[1] * scale;
		
		SDL_RenderFillRect(renderer, &rect);
	}
	
	// render link
	
	rect.x = pos[0] * scale;
	rect.y = pos[1] * scale;

	SDL_SetRenderDrawColor(renderer, 255 * (!!hitTimer), 255, 255 * (!!hitTimer), 255);
	SDL_RenderFillRect(renderer, &rect);
	
	if (dir[0])
		rect.w = scale / 4;
	else
		rect.h = scale / 4;
	
	rect.x += scale * 0.75 * (dir[0] == 1);
	rect.y += scale * 0.75 * (dir[1] == 1);

	SDL_SetRenderDrawColor(renderer, 255, 223 + 32 * (!!hitTimer), 255 * (!!hitTimer), 255);
	SDL_RenderFillRect(renderer, &rect);
	
	// render sword
	
	SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
	
	if (swordTimer)
	{
		if (dir[0])
		{
			rect.w = scale;
			rect.h = scale / 4;
			rect.x = pos[0] * scale;
			rect.y = pos[1] * scale + 3 * scale / 8;
		}
		else
		{
			rect.w = scale / 4;
			rect.h = scale;
			rect.x = pos[0] * scale + 3 * scale / 8;
			rect.y = pos[1] * scale;
		}
		
		rect.x += dir[0] * scale;
		rect.y += dir[1] * scale;

		SDL_RenderFillRect(renderer, &rect);
	}

	// render projectiles
	
	for (int i = 0; i < projs.size(); i++)
	{
		rect.w = scale / 4;
		rect.h = scale / 4;
		rect.x = projs[i][0] * scale + 3 * scale / 8;
		rect.y = projs[i][1] * scale + 3 * scale / 8;
		
		SDL_RenderFillRect(renderer, &rect);
	}
	
	// render ui
	
	rect.w = scale / 2;
	rect.h = scale / 2;
	
	rect.x = scale / 4;
	rect.y = scale / 4;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	
	for (int i = 0; i < maxHealth; i++)
	{
		if (i >= health)
			SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);

		SDL_RenderFillRect(renderer, &rect);
		rect.x += scale;
	}

	SDL_RenderPresent(renderer);
}

int main()
{
	int size[2] = {16, 11}, pos[2] = {size[0] / 2 - 1, size[1] / 2 - 1}, dir[2] = {1, 0};
	int mapSize[2] = {4, 4}, screen[2] = {mapSize[0] / 2 - 1, mapSize[1] / 2 - 1};
	int newPos[2], newDir[2], newScreen[2];
	int scale = 768 / max(size[0], size[1]);

	vector <vector <vector <vector <bool>>>> map;
	resetMap(map, size, mapSize, pos, screen);
		
	vector <vector <int>> enemies, projs;
	addEnemies(enemies, map[screen[0]][screen[1]], pos, 2);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Renderer* renderer = SDL_CreateRenderer(SDL_CreateWindow("lelda", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size[0] * scale, size[1] * scale, 0), -1, 0);;
	SDL_Event event;

	int maxHealth = 8, health = 4;
	int hitTimer = 0, swordTimer = 0;
	bool visualise = true;
	const Uint8* keys;
	
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
					
					map[screen[0]][screen[1]][x / scale][y / scale] = !map[screen[0]][screen[1]][x / scale][y / scale];
					
					break;
					
				case SDL_KEYDOWN:
					
					keys = SDL_GetKeyboardState(NULL);
					
					if (keys[SDL_SCANCODE_ESCAPE])
						return 0;
					
					if (keys[SDL_SCANCODE_SPACE])
					{
						if (!map[screen[0]][screen[1]][pos[0] + dir[0]][pos[1] + dir[1]])
						{
							if (health == maxHealth)
								projs.push_back({pos[0] + dir[0], pos[1] + dir[1], dir[0], dir[1], 8});
							else
								swordTimer = 8;
						}
						
						break;
					}
					
					newDir[0] = (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) - (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]);
					newDir[1] = (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) - (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]);
					
					if (newDir[0] || newDir[1])
					{
						if (newDir[0] && newDir[1]) // if diagonal
						{
							if (dir[0])
							{
								dir[0] = 0;
								dir[1] = newDir[1];
							}
							else
							{
								dir[0] = newDir[0];
								dir[1] = 0;
							}
						}
						else
						{
							dir[0] = newDir[0];
							dir[1] = newDir[1];
						}
						
						newPos[0] = pos[0] + dir[0];
						newPos[1] = pos[1] + dir[1];
						
						newScreen[0] = screen[0] + (newPos[0] >= size[0]) - (newPos[0] < 0);
						newScreen[1] = screen[1] + (newPos[1] >= size[1]) - (newPos[1] < 0);

						if (newScreen[0] >= 0 && newScreen[0] < mapSize[0] && newScreen[1] >= 0 && newScreen[1] < mapSize[1]) // if screen is valid
						{
							// if leaving screen, loop around to other size

							if (newPos[0] >= size[0])
								newPos[0] = 0;
							else if (newPos[0] < 0)
								newPos[0] = size[0] - 1;
							
							if (newPos[1] >= size[1])
								newPos[1] = 0;
							else if (newPos[1] < 0)
								newPos[1] = size[1] - 1;
							
							if (!map[newScreen[0]][newScreen[1]][newPos[0]][newPos[1]]) // if position is available
							{
								// if bumping into enemy, retreat and take damage

								for (int i = 0; i < enemies.size(); i++)
								{
									if (pos[0] + dir[0] == enemies[i][0] && pos[1] + dir[1] == enemies[i][1])
									{
										newPos[0] = pos[0];
										newPos[1] = pos[1];
										
										newScreen[0] = screen[0];
										newScreen[1] = screen[1];
										
										health--;
										
										if (!health)
											return 0;
										
										hitTimer = 8;
									}
								}
								
								// set position and screen

								pos[0] = newPos[0];
								pos[1] = newPos[1];
								
								if (newScreen[0] != screen[0] || newScreen[1] != screen[1])
								{
									screen[0] = newScreen[0];
									screen[1] = newScreen[1];
									
									addEnemies(enemies, map[screen[0]][screen[1]], pos, 2);
									projs.clear();
									visualise = true;
								}
							}
						}
					}
			}
		}
		
		// enemy ai
		
		for (vector <int>& enemy : enemies)
		{
			if (!enemy[2]) // if timer is 0
			{
				if (rand() % 2)
				{
					newPos[0] = enemy[0] - 1 + 2 * (rand() % 2);
					newPos[1] = enemy[1];
				}
				else
				{
					newPos[0] = enemy[0];
					newPos[1] = enemy[1] - 1 + 2 * (rand() % 2);
				}

				// if position is valid
				
				if (newPos[0] >= 0 && newPos[0] < size[0] && newPos[1] >= 0 && newPos[1] < size[1] && !map[screen[0]][screen[1]][newPos[0]][newPos[1]])
				{
					if (newPos[0] == pos[0] && newPos[1] == pos[1]) // if bumped into player, retreat and deal damage
					{
						health--;
						
						if (!health)
							return 0;
						
						hitTimer = 8;
					}
					else
					{
						enemy[0] = newPos[0];
						enemy[1] = newPos[1];
					}
					
					enemy[2] = 256;
				}
			}
			else // decrement timer
				enemy[2]--;
		}
		
		// check if sword has hit enemy
		
		if (swordTimer)
		{
			for (int i = 0; i < enemies.size(); i++)
			{
				if (pos[0] + dir[0] == enemies[i][0] && pos[1] + dir[1] == enemies[i][1])
				{
					enemies.erase(enemies.begin() + i);
					i--;
					
					if (health < maxHealth) // if hit, gain health
						health++;
					
					break;
				}
			}

			swordTimer--;
		}

		// check if projectile has hit enemy
		
		for (int i = 0; i < projs.size(); i++)
		{
			for (int j = 0; j < enemies.size(); j++)
			{
				if (projs[i][0] == enemies[j][0] && projs[i][1] == enemies[j][1])
				{
					projs.erase(projs.begin() + i);
					enemies.erase(enemies.begin() + j);
					i--;

					if (health < maxHealth) // if hit, gain health
						health++;
					
					break;
				}
			}
		}
		
		// projectiles ai
		
		for (int i = 0; i < projs.size(); i++)
		{
			if (!projs[i][4])
			{
				bfs(projs[0], enemies, map[screen[0]][screen[1]], renderer, scale * visualise); // update direction via bfs
				
				projs[i][0] += projs[i][2];
				projs[i][1] += projs[i][3];
				
				// if off screen or hit object, erase, else reset timer

				if (projs[i][0] < 0 || projs[i][0] >= size[0] || projs[i][1] < 0 || projs[i][1] >= size[1] || map[screen[0]][screen[1]][projs[i][0]][projs[i][1]])
					projs.erase(projs.begin() + i);
				else
					projs[i][4] = 8;

				visualise = false;
			}
			else
				projs[i][4]--;
		}
	
		render(map[screen[0]][screen[1]], pos, dir, enemies, projs, health, maxHealth, hitTimer, swordTimer, scale, renderer);

		if (hitTimer)
			hitTimer--;
	}
}
