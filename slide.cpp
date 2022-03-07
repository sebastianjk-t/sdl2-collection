#include <SDL.h>
#include <cmath>
#include <vector>
#include <numeric>

using namespace std;

void randomise(vector <vector <int>>& grid)
{
    for (int x = 0; x < grid.size(); x++)
    {
        iota(grid[x].begin(), grid[x].end(), 0);
        random_shuffle(grid[x].begin(), grid[x].end());
    }
}

void reset(vector <vector <int>>& grid, int size[2], int& scale, SDL_Renderer*& renderer, SDL_Window*& window)
{
    srand(time(NULL));

    size[0] = rand() % 4 + 3;
    size[1] = rand() % 4 + 3;

    scale = 768 / (max(size[0], size[1]) + 2);

    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    window = SDL_CreateWindow("slide", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * (size[0] + 2), scale * (size[1] + 2), 0);
    renderer = SDL_CreateRenderer(window, -1, 0);

    grid.clear();
    grid.resize(size[0], vector <int>(size[1]));
    randomise(grid);
}

bool hasWon(vector <vector <int>> grid)
{
    bool won = true;

    for (int y = 0; y < grid[0].size(); y++)
    {
        for (int x = 0; x < grid.size(); x++)
        {
            if (grid[x][y] != y) // if not correct colour (rainbow order)
                won = false;
        }
    }

    return won;
}

void render(vector <vector <int>> grid, int pos[2], int scale, SDL_Renderer* renderer)
{
    SDL_Rect rect;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    rect.w = scale;
    rect.h = scale;
    rect.x = scale * (pos[0] + 1);
    rect.y = scale * (pos[1] + 1);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    rect.w = (scale * 3) / 4;
    rect.h = (scale * 3) / 4;
    rect.x = (scale * 9) / 8;
    
    for (vector <int> col : grid)
    {
        rect.y = (scale * 9) / 8;

        for (int cell : col)
        {
            SDL_SetRenderDrawColor(renderer, 255 * (cell <= 1 || cell == 5), 255 * (cell >= 1 && cell <= 3), 255 * (cell >= 3 && cell <= 5), 255);
            SDL_RenderFillRect(renderer, &rect);
            
            rect.y += scale;
        }
        
        rect.x += scale;
    }
    
    SDL_RenderPresent(renderer);
}

int main()
{
    int pos[2] = {0, 0};
    int size[2];
    int scale;
    const Uint8* keys;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event event;
    
    vector <vector <int>> grid;
    reset(grid, size, scale, renderer, window);

    render(grid, pos, scale, renderer);
    
    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;

            if (event.type == SDL_MOUSEBUTTONDOWN) // select cell
            {
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (x >= scale && x < (size[0] + 1) * scale && y >= scale && y < (size[1] + 1) * scale)
                {
                    pos[0] = (x / scale) - 1;
                    pos[1] = (y / scale) - 1;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                keys = SDL_GetKeyboardState(NULL);

                if (keys[SDL_SCANCODE_ESCAPE])
                    return 0;

                if (keys[SDL_SCANCODE_SPACE])
                    reset(grid, size, scale, renderer, window);
                
                if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) // move reft
                {
                    int temp = grid[0][pos[1]];
                    
                    for (int x = 0; x < size[0] - 1; x++)
                        grid[x][pos[1]] = grid[x + 1][pos[1]];
                    
                    grid[size[0] - 1][pos[1]] = temp;

                    if (pos[0])
                        pos[0]--;
                    else
                        pos[0] = size[0] - 1;
                }
                else if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) // move right
                {
                    int temp = grid[size[0] - 1][pos[1]];
                    
                    for (int x = size[0] - 1; x > 0; x--)
                        grid[x][pos[1]] = grid[x - 1][pos[1]];
                    
                    grid[0][pos[1]] = temp;

                    if (pos[0] < size[0] - 1)
                        pos[0]++;
                    else
                        pos[0] = 0;
                }
                else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) // move up
                {
                    grid[pos[0]].push_back(grid[pos[0]][0]);
                    grid[pos[0]].erase(grid[pos[0]].begin());
                    
                    if (pos[1])
                        pos[1]--;
                    else
                        pos[1] = size[1] - 1;
                }
                else if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) // move down
                {  
                    grid[pos[0]].insert(grid[pos[0]].begin(), grid[pos[0]].back());
                    grid[pos[0]].pop_back();
                    
                    if (pos[1] < size[1] - 1)
                        pos[1]++;
                    else
                        pos[1] = 0;
                }
            }

            render(grid, pos, scale, renderer);

            if (hasWon(grid))
                reset(grid, size, scale, renderer, window);
        }
    }
}