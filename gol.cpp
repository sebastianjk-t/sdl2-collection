#include <SDL.h>
#include <vector>

using namespace std;

void randomise(vector <vector <bool>>& grid)
{
    srand(time(NULL));
    
    for (int x = 0; x < grid.size(); x++)
    {
        for (int y = 0; y < grid[x].size(); y++)
            grid[x][y] = rand() % 2;
    }
}

void update(vector <vector <bool>>& grid)
{
    int sum;
    vector <vector <bool>> prev = grid;
    
    for (int x = 0; x < grid.size(); x++)
    {
        for (int y = 0; y < grid[0].size(); y++)
        {
            sum = 0;

            // check left adjacent cells

            if (x) // if not left edge
            {
                if (y)
                    sum += prev[x - 1][y - 1];

                sum += prev[x - 1][y];

                if (y < grid[0].size() - 1)
                    sum += prev[x - 1][y + 1];
            }

            // check above and below

            if (y)
                sum += prev[x][y - 1];

            if (y < grid[0].size() - 1)
                sum += prev[x][y + 1];

            // check right adjacent cells

            if (x < grid.size() - 1) // if not right edge
            {
                if (y)
                    sum += prev[x + 1][y - 1];

                sum += prev[x + 1][y];

                if (y < grid[0].size() - 1)
                    sum += prev[x + 1][y + 1];
            }

            // revive / kill dependent on sum and previous state

            if (prev[x][y])
            {
                if (sum != 2 && sum != 3)
                    grid[x][y] = false;
            }
            else if (sum == 3)
                    grid[x][y] = true;
        }
    }
}

void render(vector <vector <bool>> grid, int scale, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect;
    rect.w = scale;
    rect.h = scale;
    rect.x = 0;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    for (vector <bool> col : grid)
    {
        rect.y = 0;
        
        for (bool cell : col)
        {
            if (cell)
                SDL_RenderFillRect(renderer, &rect);
            
            rect.y += scale;
        }
        
        rect.x += scale;
    }
    
    SDL_RenderPresent(renderer);
}

int main()
{
    int size[2] = {32, 32};
    int scale = 768 / max(size[0], size[1]);
    int delay = 125;
    const Uint8* keys;

    vector <vector <bool>> grid(size[0], vector <bool>(size[1], false));
    randomise(grid);
	
    SDL_Init(SDL_INIT_VIDEO);
	SDL_Renderer* renderer = SDL_CreateRenderer(SDL_CreateWindow("game of life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size[0] * scale, size[1] * scale, 0), -1, 0);
	SDL_Event event;
    
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
                    grid[x / scale][y / scale] = true;
                    break;
                    
                case SDL_KEYDOWN:

                    keys = SDL_GetKeyboardState(NULL);
                    
                    if (keys[SDL_SCANCODE_ESCAPE])
						return 0;
                    
                    if (keys[SDL_SCANCODE_SPACE])
                        randomise(grid);

                    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
                        delay *= 2;
                    else if ((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) && delay > 1)
                        delay /= 2;
            }
        }

        render(grid, scale, renderer);
        update(grid);

		SDL_Delay(delay);
    }
}
