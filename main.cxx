#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>

const int BLOCK_SIZE = 30;
const int GRID_WIDTH = 25;
const int GRID_HEIGHT = 20;

struct Block { int x, y; };

int main(int argc, char* argv[]) {
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window* window = SDL_CreateWindow("Zmeyka v1.0", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
        0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    std::vector<Block> snake = {{GRID_WIDTH/2, GRID_HEIGHT/2}};
    int dx = 1, dy = 0;
    Block food = {rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
    int score = 0;
    bool gameOver = false;
    
    bool running = true;
    Uint32 lastUpdate = SDL_GetTicks();
    const Uint32 UPDATE_DELAY = 200;
    
    SDL_Event event;
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            // *** ПРАВИЛЬНОЕ УПРАВЛЕНИЕ 4 НАПРАВЛЕНИЯ ***
            if (event.type == SDL_FINGERDOWN && !gameOver) {
                float touchX = event.tfinger.x;
                float touchY = event.tfinger.y;
                
                // Тап слева = влево, справа = вправо, сверху = вверх, снизу = вниз
                if (touchX < 0.4f) dx = -1, dy = 0;                    // ЛЕВО
                else if (touchX > 0.6f) dx = 1, dy = 0;                  // ПРАВО  
                else if (touchY < 0.4f) dx = 0, dy = -1;                 // ВВЕРХ
                else dx = 0, dy = 1;                                     // ВНИЗ
            }
            
            // Рестарт при тапе после game over
            if (event.type == SDL_FINGERDOWN && gameOver) {
                snake = {{GRID_WIDTH/2, GRID_HEIGHT/2}};
                dx = 1; dy = 0;
                food = {rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
                score = 0;
                gameOver = false;
            }
        }
        
        if (currentTime - lastUpdate > UPDATE_DELAY && !gameOver) {
            Block tail = snake.back();
            
            // Сдвигаем тело
            for (int i = snake.size() - 1; i > 0; i--) {
                snake[i] = snake[i-1];
            }
            
            // Новая голова
            snake[0].x += dx;
            snake[0].y += dy;
            
            // *** ТЕЛЕПОРТ ЧЕРЕЗ КРАЯ ЭКРАНА ***
            if (snake[0].x < 0) snake[0].x = GRID_WIDTH - 1;
            if (snake[0].x >= GRID_WIDTH) snake[0].x = 0;
            if (snake[0].y < 0) snake[0].y = GRID_HEIGHT - 1;
            if (snake[0].y >= GRID_HEIGHT) snake[0].y = 0;
            
            // *** СМЕРТЬ: столкновение с собой ***
            for (size_t i = 1; i < snake.size(); i++) {
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    gameOver = true;
                    break;
                }
            }
            
            // Еда
            if (snake[0].x == food.x && snake[0].y == food.y) {
                snake.push_back(tail);
                do {
                    food = {rand() % GRID_WIDTH, rand() % GRID_HEIGHT};
                } while (food.x == snake[0].x && food.y == snake[0].y);
                score += 10;
            }
            
            lastUpdate = currentTime;
        }
        
        // Отрисовка
        SDL_SetRenderDrawColor(renderer, 5, 5, 20, 255);
        SDL_RenderClear(renderer);
        
        if (!gameOver) {
            // Еда
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            SDL_Rect foodRect = {food.x * BLOCK_SIZE, food.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &foodRect);
            
            // Змейка
            for (size_t i = 0; i < snake.size(); i++) {
                int green = 220 - i * 12;
                SDL_SetRenderDrawColor(renderer, 0, green, 60, 255);
                SDL_Rect rect = {snake[i].x * BLOCK_SIZE, snake[i].y * BLOCK_SIZE, 
                               BLOCK_SIZE, BLOCK_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        } else {
            // ЭКРАН ПРОИГРЫША (красный)
            SDL_SetRenderDrawColor(renderer, 100, 20, 20, 255);
            SDL_Rect gameOverRect = {0, 0, GRID_WIDTH * BLOCK_SIZE, GRID_HEIGHT * BLOCK_SIZE};
            SDL_RenderFillRect(renderer, &gameOverRect);
        }
        
        // Счёт
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < score/10; i++) {
            SDL_Rect star = {20 + i*25, 20, 20, 20};
            SDL_RenderFillRect(renderer, &star);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}