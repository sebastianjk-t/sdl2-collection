#include <SDL.h>
#include <vector>
#include <numeric>

using namespace std;

void render(vector<int> vec, int curr, bool flipped, int scale, SDL_Renderer* renderer)
{
	SDL_Rect rect;
	rect.w = scale;
	rect.x = 0;
	rect.y = 0;
	
	for (int i = 0; i < vec.size(); i++)
	{
		if (!flipped)
			rect.y = ((vec.size() * 2 - vec[i]) * scale) / 2;
		
		rect.h = (vec[i] * scale) / 2;
		
		if (i == curr)
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		else
			SDL_SetRenderDrawColor(renderer, 255 - (vec[i] * 255 / vec.size()), 0, vec[i] * 255 / vec.size(), 255);
		
		SDL_RenderFillRect(renderer, &rect);
		
		rect.x += rect.w;
	}
}

int main()
{
	int size = 64;
	int scale = 768 / size;
	float delay = 125;
	const Uint8* keys;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Renderer* renderer = SDL_CreateRenderer(SDL_CreateWindow("insertion vs 'selection'", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size * scale, size * scale, 0), -1, 0);
	SDL_Event event;
	
	vector<int> insertion(size);
	iota(insertion.begin(), insertion.end(), 1);
	random_shuffle(insertion.begin(), insertion.end());
	int i = 1, j = i; // index trackers for insertion
	
	vector<int> selection = insertion;
	int s = 0, min = s, t = s + 1; // index trackers for selection
	
	while (true)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					return 0;
					
				case SDL_KEYDOWN:

					keys = SDL_GetKeyboardState(NULL);
					
					if (keys[SDL_SCANCODE_ESCAPE])
						return 0;
					
					if (keys[SDL_SCANCODE_SPACE]) // randomise
						random_shuffle(insertion.begin(), insertion.end());
					
					if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S]) // resort
						iota(insertion.begin(), insertion.end(), 1);
					
					if (keys[SDL_SCANCODE_S]) // reverse
						reverse(insertion.begin(), insertion.end());
			
					if (keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S]) // reset trackers
					{
						i = 1;
						j = i;
						
						selection = insertion;
						s = 0;
						min = s;
						t = s + 1;
					}
					
					if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
						delay *= 2;
					else if ((keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) && delay > 1)
						delay /= 2;
			}
		}
		
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// run and render single step of insertion sort
		
		if (i < size)
		{
			if (j > 0 && insertion[j] < insertion[j - 1])
			{
				swap(insertion[j], insertion[j - 1]);
				j--;
			}
			else
			{
				i++;
				j = i;
			}
			
			render(insertion, j, true, scale, renderer);
		}
		else
			render(insertion, -1, true, scale, renderer);
		
		// run and render single step of selection sort

		if (s < size - 1)
		{
			if (t < size && selection[min] > s + 1) // improved selection
			{
				if (selection[t] <= selection[min])
					min = t;
				
				render(selection, t, false, scale, renderer);
				t++;
			}
			else
			{
				swap(selection[min], selection[s]);
				render(selection, s, false, scale, renderer);
				
				s++;
				min = s;
				t = s;
			}
		}
		else
			render(selection, -1, false, scale, renderer);
		
		SDL_RenderPresent(renderer);
		SDL_Delay(delay);
	}
}
