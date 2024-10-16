#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdbool.h>

#define ROWS 15
#define COLS 15
#define MAX_BOMBS 100 // Maximum number of bombs
#define BOMB_TIMER 3 // Time in seconds before bomb detonates
#define MAX_COPS 4 //max num of cops
#define COP_TIMER 1.5 //time in seconds before cop spawns 
#define MAX_LEN 9999


char wall = 254;
char bank = '$';
char portal = 177;
char bomb = 'o';
char maze[ROWS][COLS];
char mirroredmaze[ROWS][2 * COLS - 3];
bool visited[ROWS][2*COLS-3]={false};
int score;
int currentX = 1;
int currentY = 1;
char input; // Global variable to store input
char last_move;
int tick = 0;
char cop = 88;
int path_minus_one = 0;
bool pathFind(int x_cop, int y_cop, bool visited[ROWS][2*COLS-3],int x,int y);char ammo = 235;
char player = 2;
int currBombs = 4;
int lives = 4;
int color_picker = 0;
char color_str[10];
char life = 35;
void initializeMaze();
void printNewMaze();
int isValidMove(int x, int y);int hasUnvisitedNeighbor(int x, int y);
void shuffleDirections(int directions[], int size);
void generateMaze(int x, int y);
void gen_new_maze();
void gameOver();
void placeBombBehindPlayer();
void detonateBomb(int x, int y);
void spawnCop(int x, int y);
void updateMaze();
void checkCopSpawnTimer();
void copChaseAI(int i);
bool pathFind(int x_cop, int y_cop, bool visited[ROWS][2*COLS-3],int x,int y);
void checkBombTimers();
void newcity(int currentX, int currentY, char lastmove);




typedef struct {
    int x;
    int y;
    time_t placed_time;
} Bomb;
typedef struct{
    int x;
    int y;
    time_t spawn_time;
    int has_spawned;
    char dir;
    char prevdir;
}Cop;
typedef struct{
    int x;
    int y;
    
}node;
node cop_here;
Cop cops[MAX_COPS];
int num_cops = 0;
Bomb bombs[MAX_BOMBS];
int num_bombs = 0;

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
    int numBanks = 0; // Count the number of banks
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < 2 * COLS - 3; j++) {
            // Count the number of banks
            if (mirroredmaze[i][j] == bank) {
                numBanks++;
            }
        }
    }
    switch(color_picker%3){
        case 0:
        strcpy(color_str,"0;31");
        break;

        case 1:
        strcpy(color_str,"0;33");
        break;
        case 2:
        strcpy(color_str,"0;36");
        break;
        default:
        strcpy(color_str,"0;31");
        break;
    }

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < 2 * COLS - 3; j++) {
            if (mirroredmaze[i][j] == wall) {
                index += sprintf(buffer + index, "\033[%sm%c\033[0m ", color_str,mirroredmaze[i][j]);
            } else if (mirroredmaze[i][j] == bank) {
                index += sprintf(buffer + index, "\033[0;32m%c\033[0m ",mirroredmaze[i][j]);
            } else if (mirroredmaze[i][j] == bomb) {
                index += sprintf(buffer + index, "\033[0;33m%c\033[0m ", mirroredmaze[i][j]);
            }else if (mirroredmaze[i][j]==cop){
                index += sprintf(buffer + index, "\033[0;34m%c\033[0m ", mirroredmaze[i][j]);
            }else if (mirroredmaze[i][j]==ammo){
                index += sprintf(buffer + index, "\033[0;35m%c\033[0m ", mirroredmaze[i][j]);
            }else if (mirroredmaze[i][j]==player){
                index += sprintf(buffer + index, "\033[0;37m%c\033[0m ", mirroredmaze[i][j]);
            }
            else if (numBanks == 0) {
                index += sprintf(buffer + index, "\033[0;32m%c\033[0m ", mirroredmaze[i][j]);
            }
            else {
                index += sprintf(buffer + index, "%c ", mirroredmaze[i][j]);
            }

            if (index >= bufferSize - 100) {
                fwrite(buffer, 1, index, stdout);
                index = 0;
            }
        }
        buffer[index++] = '\n';
    }
    index += sprintf(buffer + index, "Bombs: ");
    for (int i = 0; i < currBombs; i++) {
        buffer[index++] = '#';
    }
    buffer[index++] = '\n';
    index += sprintf(buffer + index,"Lives: ");
    for(int i = 0;i<lives;i++)
    {
        buffer[index++] = life;
    }
     buffer[index++] = '\n';
    if (index > 0) {
        fwrite(buffer, 1, index, stdout);
    }
    //printf("%d",tick);
}

int isValidMove(int x, int y) {
    return x >= 1 && x < ROWS - 1 && y >= 1 && y < COLS - 1 && maze[x][y] == wall;
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
    for (int row = 0; row < ROWS; row++) {
        int count = COLS - 2;
        for (int i = 0; i < COLS - 2; i++) {
            mirroredmaze[row][i] = maze[row][i];
        }
        for (int j = COLS - 2; j < (2 * COLS - 2); j++) {
            strcpy(&mirroredmaze[row][j], &maze[row][count]);
            count--;
        }
    }
    mirroredmaze[(ROWS - 1) / 2][0] = portal;
    mirroredmaze[(ROWS - 1) / 2][2 * COLS - 4] = portal;
    mirroredmaze[(ROWS - 1) / 2 - 1][0] = portal;
    mirroredmaze[(ROWS - 1) / 2 - 1][2 * COLS - 4] = portal;
    mirroredmaze[(ROWS - 1) / 2 + 1][0] = portal;
    mirroredmaze[(ROWS - 1) / 2 + 1][2 * COLS - 4] = portal;

    // Place banks in quadrants
    // first quad
    int rand_row = rand() % ((ROWS - 1) / 2 + 1); // min+rand()%(max-min +1)
    int rand_col = rand() % (COLS - 1);
    while (mirroredmaze[rand_row][rand_col] == wall || mirroredmaze[rand_row][rand_col] == portal) {
        rand_row = rand() % ((ROWS - 1) / 2 + 1);
        rand_col = rand() % (COLS - 1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    // second quad
    rand_row = rand() % ((ROWS - 1) / 2 + 1);
    rand_col = (COLS - 2) + rand() % ((2 * COLS - 4) - (COLS - 2) + 1);
    while (mirroredmaze[rand_row][rand_col] == wall || mirroredmaze[rand_row][rand_col] == portal) {
        rand_row = rand() % ((ROWS - 1) / 2 + 1);
        rand_col = rand() % (COLS - 1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    // quad 3
    rand_row = (ROWS - 1) / 2 + rand() % ((ROWS - 1) - (ROWS - 1) / 2 + 1);
    rand_col = rand() % (COLS - 1);
    while (mirroredmaze[rand_row][rand_col] == wall || mirroredmaze[rand_row][rand_col] == portal) {
        rand_row = (ROWS - 1) / 2 + rand() % ((ROWS - 1) - (ROWS - 1) / 2 + 1);
        rand_col = rand() % (COLS - 1);
    }
    mirroredmaze[rand_row][rand_col] = bank;
    // quad 4
    rand_row = (ROWS - 1) / 2 + rand() % ((ROWS - 1) - (ROWS - 1) / 2 + 1);
    rand_col = (COLS - 2) + rand() % ((2 * COLS - 4) - (COLS - 2) + 1);
    while (mirroredmaze[rand_row][rand_col] == wall || mirroredmaze[rand_row][rand_col] == portal) {
        rand_row = (ROWS - 1) / 2 + rand() % ((ROWS - 1) - (ROWS - 1) / 2 + 1);
        rand_col = (COLS - 2) + rand() % ((2 * COLS - 4) - (COLS - 2) + 1);
    }
    mirroredmaze[rand_row][rand_col] = bank;

        int rand_row_ammo, rand_col_ammo;
    do {
        rand_row_ammo = rand() % ROWS;
        rand_col_ammo = rand() % (2 * COLS - 3);
    } while (mirroredmaze[rand_row_ammo][rand_col_ammo] != ' ');

    mirroredmaze[rand_row_ammo][rand_col_ammo] = ammo;
}
void gameOver()
{
    Sleep(1000);
    system("cls");
    printf("YOU HAVE LOST\n");
    printf("Cash Looted: %d$",score*100);


}

void placeBombBehindPlayer() {
    // Determine the position behind the player based on the last move
    int bombX = currentX;
    int bombY = currentY;

    switch (last_move) {
        case 'w':
            bombX++;
            break;
        case 's':
            bombX--;
            break;
        case 'a':
            bombY++;
            break;
        case 'd':
            bombY--;
            break;
    }

    if (mirroredmaze[bombX][bombY] != wall && mirroredmaze[bombX][bombY] != portal && currBombs>0 && mirroredmaze[bombX][bombY] != bomb) {
        currBombs--;
        if (mirroredmaze[bombX][bombY] == cop) {
            // Detonate the bomb immediately if a cop is present
            detonateBomb(bombX, bombY);
        } else {
            mirroredmaze[bombX][bombY] = bomb;

            // Add the bomb to the listf
            bombs[num_bombs].x = bombX;
            bombs[num_bombs].y = bombY;
            bombs[num_bombs].placed_time = time(NULL);
            num_bombs++;
        }
    }
}


void detonateBomb(int x, int y) {
    // Detonate the bomb at the given position
    mirroredmaze[x][y] = ' ';
    updateMaze();
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newX = x + i;
            int newY = y + j;
            if (newX >= 1 && newX < ROWS - 1 && newY >= 1 && newY < 2 * COLS - 4) {
                if (mirroredmaze[newX][newY] == bomb) {
                    // Detonate neighboring bomb recursively
                    detonateBomb(newX, newY);
                } else {
                    if(mirroredmaze[newX][newY] != bank) {
                        // Clear space
                        mirroredmaze[newX][newY] = ' ';
                    }
                    // Check if a cop is present at the location and remove it
                    for(int k = 0; k < num_cops; k++) {
                        if(cops[k].x == newX && cops[k].y == newY && cops[k].has_spawned) {
                            cops[k].x = -1; // Mark as removed
                            cops[k].y = -1;
                            mirroredmaze[newX][newY] = ' '; // Clear the cop from the maze
                        }
                    }
                }
            }
        }
    }
}




void spawnCop(int x, int y)
{
    if (mirroredmaze[x][y] != wall && mirroredmaze[x][y] != portal) {
        

        // Add the cop to the list
        cops[num_cops].x = x;
        cops[num_cops].y = y;
        cops[num_cops].spawn_time = time(NULL);
        cops[num_cops].has_spawned = 0;
        num_cops++;
    }
}
void updateMaze() {
    if (mirroredmaze[currentX][currentY] == bank) {
        score++;
        spawnCop(currentX,currentY);

    }
    if (mirroredmaze[currentX][currentY] == ammo) {
        currBombs += 2;
    }

    mirroredmaze[currentX][currentY] = player;
}
void checkCopSpawnTimer()
{
    time_t current_time = time(NULL);
    for(int i = 0;i<num_cops;i++)
    {
        double elapsed_time = difftime(current_time,cops[i].spawn_time);
        if(elapsed_time >= COP_TIMER && cops[i].has_spawned != 1)
        {
            cops[i].has_spawned = 1;

            mirroredmaze[cops[i].x][cops[i].y] = cop;

        }
        if(cops[i].has_spawned == 1&&cops[i].x>0)
        {
            copChaseAI(i);
        }
    }
}
void copChaseAI(int i)
{
     if(mirroredmaze[cops[i].x][cops[i].y]!=bank){
    mirroredmaze[cops[i].x][cops[i].y]=' ';}
    memset(visited,false,sizeof(visited));
    pathFind(cops[i].x,cops[i].y,visited,currentX,currentY);
    cops[i].x = cop_here.x;
    cops[i].y = cop_here.y;
     if(mirroredmaze[cops[i].x][cops[i].y]==bomb)
    {
        detonateBomb(cops[i].x,cops[i].y);
        mirroredmaze[cops[i].x][cops[i].y]=' ';
        
    }
    if(mirroredmaze[cops[i].x][cops[i].y]!=bank){ 
    mirroredmaze[cops[i].x][cops[i].y]=cop;}
   
    if (cops[i].x == currentX && cops[i].y == currentY) {
        lives--;
        currentX = 1;
        currentY = 1;
        if(lives==0)
        {
        lives--;
        gameOver();
        exit(0);
        }
        
    }


}
bool pathFind(int x_cop, int y_cop, bool visited[ROWS][2*COLS-3],int x,int y)
{
   // printf("%d %d\n",x,y);
    if(x==x_cop&&y==y_cop)
    {
         //debug
        //cop_here.x = x;
        //cop_here.y = y;
      //  printf("currently targetting %d %d\n",x,y);
        path_minus_one = 1;
        return true;
    }
    if(x>=ROWS||y>=2*COLS-3)
    {
        return false;
    }
    if(x<=0||y<=0)
    {
        return false;
    }
    if(visited[x][y]==true)
    {
        return false;
    }
    if(mirroredmaze[x][y]==wall||mirroredmaze[x][y]==portal)
    {
        return false;
    }
    visited[x][y]=true;
    if(pathFind(x_cop,y_cop,visited,x+1,y)==true)
    {
        if(path_minus_one==1)
        {
            cop_here.x = x;
            cop_here.y = y;
            path_minus_one =0;
        }
        return true;
    }
    if (pathFind(x_cop,y_cop,visited,x,y+1)==true)
    {
        if(path_minus_one==1)
        {
            cop_here.x = x;
            cop_here.y = y;
            path_minus_one =0;
        }
        return true;
    }
    if(pathFind(x_cop,y_cop,visited,x-1,y)==true)
    {
        if(path_minus_one==1)
        {
            cop_here.x = x;
            cop_here.y = y;
            path_minus_one =0;
        }
        return true;
    }
    if(pathFind(x_cop,y_cop,visited,x,y-1)==true)
    {
        if(path_minus_one==1)
        {
            cop_here.x = x;
            cop_here.y = y;
            path_minus_one =0;
        }
        return true;
    }
    return false;
}
void checkBombTimers() {
    time_t current_time = time(NULL);
    for (int i = 0; i < num_bombs; i++) {
        double elapsed_time = difftime(current_time, bombs[i].placed_time);
        if (elapsed_time >= BOMB_TIMER) { // Check if the time limit is reached
            detonateBomb(bombs[i].x, bombs[i].y);
            // Remove the detonated bomb from the list (if necessary)
            // Example: Shift elements in the array to remove the detonated bomb
            for (int j = i; j < num_bombs - 1; j++) {
                bombs[j] = bombs[j + 1];
            }
            num_bombs--;
            i--; // Adjust the loop counter after removing the bomb
        }
    }
}

void newcity(int currentX, int currentY, char lastmove){
    tick = 0;
    color_picker++;
    initializeMaze();
    srand((int)time(NULL));
    generateMaze(1, 1);
    gen_new_maze();
    system("cls");
    printNewMaze();

   
}

int main() {
    system("cls");      
    printf("######      ###      #      #    #   #          #    #    ######    #######   ######    #######\n");
    printf("#    #     #   #     # #    #    #  #           #    #    #            #      #            #   \n");
    printf("######    #     #    #  #   #    ##             ######    ######       #      ######       #   \n");
    printf("#    #    #######    #   #  #    # #            #    #    #            #           #       #   \n");
    printf("#    #    #     #    #    # #    #  #           #    #    #            #           #       #   \n");
    printf("######    #     #    #     ##    #   #          #    #    ######    #######   ######       #   \n\n");
    printf("This is the life! A bank robber in the 1930's, when those who relieved financial institutions of their moolah were hailed as heroes!\nAnd there are plenty of banks just ripe for the picking, several in each community and with your classy jalopy, getting from town to town is a lead-pipe cinch.\nBut there is one little problem, those coppers just don't seem to agree that hitting a bank is A-O.K., and they keep chasing you.\nLefty, get out the dynamite before those cops get us..");
    printf("\n\npress any key to continue");
    getch();
    system("cls");
    printf("Controls\n\n");
    printf("'w-a-s-d' - movement controls to navigate the cities\n");
    printf("'b' - Places a Bomb behind you\n\n");

    printf("press any key to continue");
    getch();
    system("cls");
    tick = 0;
    initializeMaze();
    srand((int)time(NULL)); // Seeding
    generateMaze(1, 1);
    gen_new_maze();
    mirroredmaze[currentX][currentY] = player;
   //system("cls");
    printNewMaze();
    score = 0;
    pthread_t input_tid;
    pthread_create(&input_tid, NULL, input_thread, NULL); // Create input thread
    last_move = ' '; // Initialize last move

    while (1) {
        //tick++;
        // Check bomb timers
        checkBombTimers();
        checkCopSpawnTimer();
        Sleep(200);
        char move = input; // Get the input from the global variable

        // Variable to track whether the move was valid
        int validmove = 0;
        int countbanks = 0;
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < 2 * COLS - 3; j++) {
                if (mirroredmaze[i][j] == bank) {
                    countbanks++;
                }
            }
        }
       switch (move) {
            case 'w':
                if (mirroredmaze[currentX - 1][currentY] == ' ' || mirroredmaze[currentX - 1][currentY] == bank || mirroredmaze[currentX - 1][currentY] == portal || mirroredmaze[currentX - 1][currentY] == ammo) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX - 1;
                    
                    validmove = 1;
                    last_move = 'w'; // Update last move
                }
                
                
                break;
            case 's':
                if (mirroredmaze[currentX + 1][currentY] == ' ' || mirroredmaze[currentX + 1][currentY] == bank || mirroredmaze[currentX + 1][currentY] == portal || mirroredmaze[currentX + 1][currentY] == ammo) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentX = currentX + 1;
                    //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    validmove = 1;
                    last_move = 's'; // Update last move
                }
                
                break;
            case 'a':
                if (mirroredmaze[currentX][currentY - 1] == ' ' || mirroredmaze[currentX][currentY - 1] == bank || mirroredmaze[currentX][currentY - 1] == portal || mirroredmaze[currentX][currentY - 1] == ammo) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY - 1;
                    if (currentY <= 0) {
                        if(countbanks == 0){
                            mirroredmaze[currentX][currentY] = portal;
                            currentY = 2 * COLS - 5;
                            mirroredmaze[currentX][currentY] = player;
                            for(int i=0;i<num_cops;i++)
                            {
                                cops[i].x = -1;
                            }
                            num_cops = 0;
                            newcity(currentX, currentY, last_move);
                        }
                        else{
                            currentY = 2 * COLS - 5;
                        }
                    }
                    
                    //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    validmove = 1;
                    last_move = 'a'; // Update last move
                }
               
                break;
            case 'd':
                if (mirroredmaze[currentX][currentY + 1] == ' ' || mirroredmaze[currentX][currentY + 1] == bank || mirroredmaze[currentX][currentY + 1] == portal || mirroredmaze[currentX][currentY + 1] == ammo) {
                    mirroredmaze[currentX][currentY] = ' ';
                    currentY = currentY + 1;
                    if (currentY >= 2 * COLS - 4) {
                        if(countbanks == 0){
                            mirroredmaze[currentX][currentY] = portal;
                            currentY = 1;
                            mirroredmaze[currentX][currentY] = player;
                            for(int i=0;i<num_cops;i++)
                            {
                                cops[i].x = -1;
                            }
                            num_cops = 0;
                            newcity(currentX, currentY, last_move);
                        }
                        else{
                            currentY = 1;
                        }
                    }
                    //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    validmove = 1;
                    last_move = 'd'; // Update last move
                }
                
                break;
            case 'b':
                placeBombBehindPlayer();
                validmove = 1;
                break;
            case 27: // Escape key
                printf("%d", score); // Debug statement
                exit(0);
        }
                        

        // If the move was invalid, continue in the last movement direction
        if (validmove == 0) {
            // Check bomb timers
        checkBombTimers();
            switch (last_move) {
                case 'w':
                    if (mirroredmaze[currentX - 1][currentY] == ' ' || mirroredmaze[currentX - 1][currentY] == bank || mirroredmaze[currentX - 1][currentY] == portal || mirroredmaze[currentX - 1][currentY] == ammo) {
                        mirroredmaze[currentX][currentY] = ' ';
                        currentX = currentX - 1;
                        //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    }
                    break;
                case 's':
                    if (mirroredmaze[currentX + 1][currentY] == ' ' || mirroredmaze[currentX + 1][currentY] == bank || mirroredmaze[currentX + 1][currentY] == portal || mirroredmaze[currentX + 1][currentY] == ammo) {
                        mirroredmaze[currentX][currentY] = ' ';
                        currentX = currentX + 1;
                       // updateMaze();
                        //system("cls");
                        //printNewMaze();
                    }
                    break;
                case 'a':
                    if (mirroredmaze[currentX][currentY - 1] == ' ' || mirroredmaze[currentX][currentY - 1] == bank || mirroredmaze[currentX][currentY - 1] == portal || mirroredmaze[currentX][currentY - 1] == ammo) {
                        mirroredmaze[currentX][currentY] = ' ';
                        currentY = currentY - 1;
                    if (currentY <= 0) {
                        if(countbanks == 0){
                            mirroredmaze[currentX][currentY] = portal;
                            currentY = 2 * COLS - 5;
                            mirroredmaze[currentX][currentY] = player;
                            newcity(currentX, currentY, last_move);
                        }
                        else{
                            currentY = 2 * COLS - 5;
                        }
                    }
                        //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    }
                    break;
                case 'd':
                    if (mirroredmaze[currentX][currentY + 1] == ' ' || mirroredmaze[currentX][currentY + 1] == bank || mirroredmaze[currentX][currentY + 1] == portal || mirroredmaze[currentX][currentY + 1] == ammo) {
                        mirroredmaze[currentX][currentY] = ' ';
                        currentY = currentY + 1;
                    if (currentY >= 2 * COLS - 4) {
                        if(countbanks == 0){
                            mirroredmaze[currentX][currentY] = portal;
                            currentY = 1;
                            mirroredmaze[currentX][currentY] = player;
                            newcity(currentX, currentY, last_move);
                        }
                        else{
                            currentY = 1;
                        }
                    }
                        //updateMaze();
                        //system("cls");
                        //printNewMaze();
                    }
                    break;

            }
                        
        }
                        updateMaze();
                        system("cls");
                        printNewMaze();



    }

    pthread_join(input_tid, NULL); // Wait for the input thread to finish
    return 0;
}
