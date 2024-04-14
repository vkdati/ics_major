#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define ROWS 21
#define COLS 21
char wall = 254;
char bank = '$'; 
char portal = 177;
char maze[ROWS][COLS];
char mirroredmaze[ROWS][2*COLS - 3];
int score;
int currentX = 1;
int currentY = 1;
char input; // Global variable to store input
char last_move;

void *input_thread(void *arg) {
    while (1) {
        
        input = _getch(); // Get input character from the user
    }
    return NULL;
}

void initializeMaze() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            maze[i][j] = wall;
        }
    }
}

void printNewMaze() { 
    const int bufferSize = 4096;
    char buffer[bufferSize];
    int index = 0;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < 2*COLS-3; j++) {
            if (mirroredmaze[i][j] == wall) {
                index += sprintf(buffer + index, "\033[0;31m%c\033[0m ", mirroredmaze[i][j]);
            } else if (mirroredmaze[i][j] == bank) {
                index += sprintf(buffer + index, "\033[0;32m%c\033[0m ", mirroredmaze[i][j]);
            } else {
                index += sprintf(buffer + index, "%c ", mirroredmaze[i][j]);
            }

            if (index >= bufferSize - 100) {
                fwrite(buffer, 1, index, stdout);
                index = 0;
            }
        }
        buffer[index++] = '\n';
    }
    if (index > 0) {
        fwrite(buffer, 1, index, stdout);
    }
}

int isValidMove(int x, int y) {
    return x >= 1 && x < ROWS-1 && y >= 1 && y < COLS-1 && maze[x][y] == wall;
}

int hasUnvisitedNeighbor(int x, int y) {
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    for (int i = 0; i < 4; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (isValidMove(newX, newY)) {
            return 1;
        }
    }

    return 0;
}

void shuffleDirections(int directions[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int randIndex = rand() % (i + 1);
        int temp = directions[i];
        directions[i] = directions[randIndex];
        directions[randIndex] = temp;
    }
}

void generateMaze(int x, int y) {
   int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    maze[x][y] = ' ';

    int directions[] = {0, 1, 2, 3};
    shuffleDirections(directions, 4);

    for (int i = 0; i < 4; i++) {
        int newX = x + dx[directions[i]] * 2;
        int newY = y + dy[directions[i]] * 2;

        if (isValidMove(newX, newY) && hasUnvisitedNeighbor(newX, newY)) {
            maze[x + dx[directions[i]]][y + dy[directions[i]]] = ' ';
            generateMaze(newX, newY);
        }
    }
}

void gen_new_maze() {
   for(int row=0;row<ROWS;row++)
    {
        int count = COLS-2;
        for(int i=0;i<COLS-2;i++)
        {
            mirroredmaze[row][i] = maze[row][i];
        }
        for(int j = COLS-2;j<(2*COLS-2);j++)
        {
            strcpy(&mirroredmaze[row][j],&maze[row][count]);
            count--;
        }
    }
    mirroredmaze[(ROWS-1)/2][0]=portal;
    mirroredmaze[(ROWS-1)/2][2*COLS-4]=portal;
    mirroredmaze[(ROWS-1)/2-1][0]=portal;
    mirroredmaze[(ROWS-1)/2-1][2*COLS-4]=portal;  
    mirroredmaze[(ROWS-1)/2+1][0]=portal;
    mirroredmaze[(ROWS-1)/2+1][2*COLS-4]=portal;
//first quad
    int rand_row = rand()%((ROWS-1)/2+1); //min+rand()%(max-min +1)
    int rand_col = rand()%(COLS-1);
    while(mirroredmaze[rand_row][rand_col]==wall||mirroredmaze[rand_row][rand_col]==portal)
    {
     rand_row = rand()%((ROWS-1)/2+1); 
     rand_col = rand()%(COLS-1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    //second quad
    rand_row = rand()%((ROWS-1)/2+1); 
    rand_col = (COLS-2)+rand()%((2*COLS-4)-(COLS-2)+1);
    while(mirroredmaze[rand_row][rand_col]==wall||mirroredmaze[rand_row][rand_col]==portal)
    {
     rand_row = rand()%((ROWS-1)/2+1); 
     rand_col = rand()%(COLS-1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    //quad 3
    rand_row = (ROWS-1)/2+rand()%((ROWS-1)-(ROWS-1)/2 +1);
    rand_col = rand()%(COLS-1);
     while(mirroredmaze[rand_row][rand_col]==wall||mirroredmaze[rand_row][rand_col]==portal)
    {
    rand_row = (ROWS-1)/2+rand()%((ROWS-1)-(ROWS-1)/2 +1);
    rand_col = rand()%(COLS-1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    //quad 4
    rand_row = (ROWS-1)/2+rand()%((ROWS-1)-(ROWS-1)/2 +1);
    rand_col = (COLS-2)+rand()%((2*COLS-4)-(COLS-2)+1);
    while(mirroredmaze[rand_row][rand_col]==wall||mirroredmaze[rand_row][rand_col]==portal)
    {
    rand_row = (ROWS-1)/2+rand()%((ROWS-1)-(ROWS-1)/2 +1);
    rand_col = (COLS-2)+rand()%((2*COLS-4)-(COLS-2)+1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
}

void updateMaze() {
    if(mirroredmaze[currentX][currentY] ==  bank)
    {
        score++;
    }

    mirroredmaze[currentX][currentY] = '#';
}

int main() {
    initializeMaze();
    srand((int)time(NULL)); //seeding
    generateMaze(1, 1);
    gen_new_maze();
    mirroredmaze[currentX][currentY] = '#';
    system("cls"); 
    printNewMaze();
    score = 0;
    pthread_t input_tid;
    pthread_create(&input_tid, NULL, input_thread, NULL); // Create input thread
    last_move = input;
    int validmove = 0;
    while (1) {
        Sleep(150);
        char move = input; // Get the input from the global variable

       switch (move) {
            case 'w':
                
                if (mirroredmaze[currentX - 1][currentY] == ' ' || mirroredmaze[currentX - 1][currentY] == bank || mirroredmaze[currentX - 1][currentY] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX - 1;
                    updateMaze();
                    system("cls"); 
                    printNewMaze();
                    validmove = 1;
                    last_move = 'w'; // Update last move
                }
                else{
                    validmove =0;
                }
                break;
            case 's':
                
                if (mirroredmaze[currentX + 1][currentY] == ' ' || mirroredmaze[currentX + 1][currentY] == bank || mirroredmaze[currentX + 1][currentY] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX + 1;
                    updateMaze();
                    system("cls");
                    printNewMaze();
                     validmove = 1;
                     last_move = 's'; // Update last move
                }
                else{
                    validmove =0;
                }
                break;
            case 'a':
                
                if (mirroredmaze[currentX][currentY - 1] == ' ' || mirroredmaze[currentX][currentY - 1] == bank || mirroredmaze[currentX][currentY - 1] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY - 1;
                    if (currentY <= 0) {
                        currentY = 2 * COLS - 5; // To move across border correctly
                    }
                    updateMaze();
                    system("cls");
                    printNewMaze();
                     validmove = 1;
                     last_move = 'a'; // Update last move
                }
                else{
                    validmove =0;
                }
                break;
            case 'd':
                
                if (mirroredmaze[currentX][currentY + 1] == ' ' || mirroredmaze[currentX][currentY + 1] == bank || mirroredmaze[currentX][currentY + 1] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY + 1;
                    if (currentY >= 2 * COLS - 4) {
                        currentY = 1;
                    }
                    updateMaze();
                    system("cls");
                    printNewMaze();
                     validmove = 1;
                     last_move = 'd'; // Update last move
                }
                else{
                    validmove =0;
                }
                break;
            case 27: // Escape key
                printf("%d", score); // Debug statement
                exit(0);
        }

        // If no new input, continue in the last movement direction
        if(validmove==0)
        {
        switch (last_move) {
            case 'w':
                if (mirroredmaze[currentX - 1][currentY] == ' ' || mirroredmaze[currentX - 1][currentY] == bank || mirroredmaze[currentX - 1][currentY] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX - 1;
                    updateMaze();
                    system("cls"); 
                    printNewMaze();
                }
                break;
            case 's':
                if (mirroredmaze[currentX + 1][currentY] == ' ' || mirroredmaze[currentX + 1][currentY] == bank || mirroredmaze[currentX + 1][currentY] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX + 1;
                    updateMaze();
                    system("cls");
                    printNewMaze();
                }
                break;
            case 'a':
                if (mirroredmaze[currentX][currentY - 1] == ' ' || mirroredmaze[currentX][currentY - 1] == bank || mirroredmaze[currentX][currentY - 1] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY - 1;
                    if (currentY <= 0) {
                        currentY = 2 * COLS - 5; // To move across border correctly
                    }
                    updateMaze();
                    system("cls");
                    printNewMaze();
                }
                break;
            case 'd':
                if (mirroredmaze[currentX][currentY + 1] == ' ' || mirroredmaze[currentX][currentY + 1] == bank || mirroredmaze[currentX][currentY + 1] == portal) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY + 1;
                    if (currentY >= 2 * COLS - 4) {
                        currentY = 1;
                    }
                    updateMaze();
                    system("cls");
                    printNewMaze();
                }
                break;
        }
        }
    }

    pthread_join(input_tid, NULL); // Wait for the input thread to finish
    return 0;
}
