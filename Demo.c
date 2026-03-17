#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#define WIDTH 20
#define HEIGHT 20
#define MAX_LENGTH 100

int snakeX[MAX_LENGTH], snakeY[MAX_LENGTH];
int snakeLength = 3;
int foodX, foodY;
int dirX = 1, dirY = 0;
int gameOver = 0;
int score = 0;

void setup() {
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;
    snakeX[1] = WIDTH / 2 - 1;
    snakeY[1] = HEIGHT / 2;
    snakeX[2] = WIDTH / 2 - 2;
    snakeY[2] = HEIGHT / 2;
    
    foodX = rand() % WIDTH;
    foodY = rand() % HEIGHT;
}

void draw() {
    system("clear");
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == snakeX[0] && y == snakeY[0]) printf("@");
            else if (x == foodX && y == foodY) printf("*");
            else {
                int isBody = 0;
                for (int i = 1; i < snakeLength; i++) {
                    if (x == snakeX[i] && y == snakeY[i]) {
                        printf("o");
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody) printf(" ");
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int getch() {
    return getchar();
}

void input() {
    if (kbhit()) {
        int key = getch();
        if (key == 27) { // escape sequence for arrow keys
            if (kbhit()) {
                getch(); // skip '['
                int arrow = getch();
                if (arrow == 'A' && dirY == 0) { dirX = 0; dirY = -1; } // up
                if (arrow == 'B' && dirY == 0) { dirX = 0; dirY = 1; } // down
                if (arrow == 'C' && dirX == 0) { dirX = 1; dirY = 0; } // right
                if (arrow == 'D' && dirX == 0) { dirX = -1; dirY = 0; } // left
            }
        } else {
            if (key == 'a' && dirX == 0) { dirX = -1; dirY = 0; }
            if (key == 'd' && dirX == 0) { dirX = 1; dirY = 0; }
            if (key == 'w' && dirY == 0) { dirX = 0; dirY = -1; }
            if (key == 's' && dirY == 0) { dirX = 0; dirY = 1; }
            if (key == 'q') gameOver = 1;
        }
    }
}

void update() {
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    
    snakeX[0] += dirX;
    snakeY[0] += dirY;
    
    // Wrap around edges to make playground infinite
    if (snakeX[0] < 0) snakeX[0] = WIDTH - 1;
    if (snakeX[0] >= WIDTH) snakeX[0] = 0;
    if (snakeY[0] < 0) snakeY[0] = HEIGHT - 1;
    if (snakeY[0] >= HEIGHT) snakeY[0] = 0;
    
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
            gameOver = 1;
    }
    
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        score += 10;
        foodX = rand() % WIDTH;
        foodY = rand() % HEIGHT;
    }
}

int main() {
    srand(time(0));
    setup();
    
    // Set terminal to raw mode for immediate input
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
    
    while (!gameOver) {
        draw();
        input();
        update();
        usleep(200000);
    }
    
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);
    
    printf("Game Over! Final Score: %d\n", score);
    return 0;
}