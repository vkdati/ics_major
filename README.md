  
README
  
ICS - Minor Project - 
THE 2048 GAME

Name : Vasu Kodati
Roll no. : B23EE1079
Email : b23ee1079@iitj.ac.in


Introduction:

This project aims to implement the ‘2048’ game in C, whose goal is to merge tiles which contain the powers of two in a 4x4 grid and
 eventually get the 2048 tile. You lose if there are no empty tiles and no valid pairs that can be merged. 
 Adjacent tiles of power n that are equal may be merged to give a n+1 power tile. This program is terminal based.
 The project objectives are as follows - 
To give the user a simple version of the 2048 game in command line
To keep track of the score and display it to the user

Game Loop - 
Use the ‘WASD’ keys to slide and match adjacent tiles
Get 2048 without having the tiles fill up with no valid combinations

EXECUTION INSTRUCTIONS - 
1.Unzip this folder 
2.Navigate to the directory that you have stored the code in and compile it   - 
	gcc 2048_proj.c -o 2048_proj    
	Run this in the command line
3. Run the program exe - 
    .\2048_proj
	
	Alternatively use a IDE such as VSCode and run it directly there.
	
Prerequisites - 
1.GCC Compiler or a similar C compiler
2.A windows system as the code uses libraries such as conio.h and functions such as Sleep().


Implementation Details - 
Language used - C
Key Features - 
Grid based board, implemented using 2D array 
User i/o in terminal
Game State i.e win lose conditions
Random number insertion after turn
Correct transformation of array upon input
Structures used - 
Array to store and display game grid
Functions to modularise the code
Command Line user interface



SPECIAL THANKS - 
I would like to express my gratitude to Dr.Mayank Vatsa and all the teaching assistance of the ICS Course for their support and guidance during this project.

	


  
  