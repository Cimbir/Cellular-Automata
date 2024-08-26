// g++ -c .\Disp_Cell_Auto.cpp -ID:\Workspace\SFML-2.6.1\include
// g++ .\Disp_Cell_Auto.o -o DCA -LD:\Workspace\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace sf;

// UNCHANGEABLE CONSTANTS

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
vector<int> offsets = {1, 0, -1, -1, 1, -1, 0, 1, 1};

// CHANGEABLE CONSTANTS

int THREAD_AMOUNT = 40;

int STATE_AMOUNT = 2;

int N = 300;
int M = 300;

int WIDTH = 1000;
int HEIGHT = 1000;

double CELL_SIZE_I = (double)HEIGHT/N;
double CELL_SIZE_J = (double)WIDTH/M;
int FPS = 60;

vector<vector<int>> before(N, vector<int>(M));
vector<vector<int>> after(N, vector<int>(M));

// TYPES

#define cord pair<int,int>
struct args_t{
    int from;
    int to;
}typedef args_t;

// SIMPLE FUNCTIONS

int getRandom(int from, int to){
    return uniform_int_distribution<int>(from,to)(rng);
}

// GENERAL FUNCTIONS

vector<cord> getNeighbors(cord from){
    vector<cord> res;
    for(int i = 0; i < 8; i++){
        int nx = (from.first+offsets[i]+N)%N;
        int ny = (from.second+offsets[i+1]+M)%M;
        res.push_back({nx,ny});
    }
    return res;
}
int getStateByRules(cord from, vector<vector<int>>& grid){
    vector<cord> neighbors = getNeighbors(from);
    int liveAm = 0;
    for(int i = 0; i < neighbors.size(); i++) liveAm += grid[neighbors[i].first][neighbors[i].second]; 

    if(grid[from.first][from.second]) return liveAm == 2 || liveAm == 3;
    else return liveAm == 3;
}
void* getNextIterationWorker(void* arg){
    args_t* a = (args_t*)arg; 
    for(int i = a->from; i <= a->to; i++){
        for(int j = 0; j < M; j++){
            after[i][j] = getStateByRules({i,j}, before);
        }
    }
    return 0;
}
void getNextIteration(){
    pthread_t workers[THREAD_AMOUNT];
    args_t args[THREAD_AMOUNT];

    for(int i = 0; i < THREAD_AMOUNT; i++){
        args[i].from = i * N / THREAD_AMOUNT;
        args[i].to = (i+1) * N / THREAD_AMOUNT - 1;
        pthread_create(&workers[i],0,getNextIterationWorker,&args[i]);
    }
    for(int i = 0; i < THREAD_AMOUNT; i++){
        pthread_join(workers[i],0);
    }
}

// DISPLAY

RectangleShape stateToRect(int state){
    RectangleShape rect;
    rect.setSize(Vector2f(CELL_SIZE_J,CELL_SIZE_I));
    // Add color for different states
    if(state == 0) rect.setFillColor(Color::Black);
    return rect;
}
int characterToState(char c){
    if(c == '#') return 1;
    return 0;
}
void gridDisplay(vector<vector<int>>& grid, RenderWindow& window){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            RectangleShape rect = stateToRect(grid[i][j]);
            rect.setPosition(j*CELL_SIZE_J, i*CELL_SIZE_I);
            window.draw(rect);
        }
    }
}

// HELPER

void setUpGrid(vector<string>& setup, vector<vector<int>>& grid){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            grid[i][j] = characterToState(setup[i][j]);
        }
    }
}
void copyGrid(vector<vector<int>>& to, vector<vector<int>>& from){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            to[i][j] = from[i][j];
        }
    }
}
void setUpGridRandom(vector<vector<int>>& grid){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            // Depends on rule variant
            grid[i][j] = getRandom(0,STATE_AMOUNT-1);
        }
    }
}

int main(){
    // Test | 5X5
    // vector<string> initialSetup = {
    //     "#    ",
    //     "  #  ",
    //     "  ## ",
    //     "  #  ",
    //     "     "
    // };
    // setUpGrid(initialSetup, before);

    setUpGridRandom(before);

    // create the window
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "My window");
    window.setFramerateLimit(FPS);

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(Color::Black);

        // draw everything here...
        // window.draw(...);

        gridDisplay(before, window);
        getNextIteration();
        copyGrid(before,after);

        // end the current frame
        window.display();
    }

    return 0;
}