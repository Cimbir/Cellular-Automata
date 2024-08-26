// g++ -c .\Basic_Sand_And_Water.cpp -ID:\Workspace\SFML-2.6.1\include
// g++ .\Basic_Sand_And_Water.o -o BSAW -LD:\Workspace\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>

using namespace std;
using namespace sf;

// UNCHANGEABLE CONSTANTS

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
vector<int> offsets = {1, 0, -1, -1, 1, -1, 0, 1, 1};

// CHANGEABLE CONSTANTS

// 0 - blank
// 1 - sand
// 2 - water
int STATE_AMOUNT = 3;

int N = 300;
int M = 300;

int WIDTH = 900;
int HEIGHT = 900;

double CELL_SIZE_I = (double)HEIGHT/N;
double CELL_SIZE_J = (double)WIDTH/M;
int FPS = 60;

vector<vector<int>> grid(N, vector<int>(M));

// TYPES

#define cord pair<int,int>

// SIMPLE FUNCTIONS

int getRandom(int from, int to){
    return uniform_int_distribution<int>(from,to)(rng);
}
bool inBounds(int i, int j){
    return i >= 0 && j >= 0 && i < N && j < M;
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
void moveParticle(cord from){
    int x = from.first;
    int y = from.second;

    cord bc = {x+1,y};
    cord bl = {x+1,y-1};
    cord br = {x+1,y+1};
    
    cord cl = {x,y-1};
    cord cr = {x,y+1};

    cord tc = {x-1,y};
    cord tl = {x-1,y-1};
    cord tr = {x-1,y+1};

    cord next = from;
    if(grid[x][y] == 1){
        if(inBounds(bc.first,bc.second) && grid[bc.first][bc.second] != 1) next = bc;
        else if(inBounds(bl.first,bl.second) && grid[bl.first][bl.second] != 1) next = bl;
        else if(inBounds(br.first,br.second) && grid[br.first][br.second] != 1) next = br;
    }else if(grid[x][y] == 2){
        if(inBounds(bc.first,bc.second) && grid[bc.first][bc.second] < 1) next = bc;
        else if(inBounds(bl.first,bl.second) && grid[bl.first][bl.second] < 1) next = bl;
        else if(inBounds(br.first,br.second) && grid[br.first][br.second] < 1) next = br;
        else if(inBounds(cr.first,cr.second) && grid[cr.first][cr.second] < 1) next = cr;
        else if(inBounds(cl.first,cl.second) && grid[cl.first][cl.second] < 1) next = cl;
    }

    int tmp = grid[x][y];
    grid[x][y] = grid[next.first][next.second];
    grid[next.first][next.second] = tmp;
}
void getNextIteration(){
    for(int i = N-1; i >= 0 ; i--){
        for(int j = 0; j < M; j++){
            
            moveParticle({i,j});

        }
    }
}

// DISPLAY

RectangleShape stateToRect(int state){
    RectangleShape rect;
    rect.setSize(Vector2f(CELL_SIZE_J,CELL_SIZE_I));
    // Add color for different states
    if(state == 1) rect.setFillColor(Color::Yellow);
    if(state == 2) rect.setFillColor(Color::Blue);
    return rect;
}
int characterToState(char c){
    if(c == '#') return 1;
    return 0;
}
void gridDisplay(vector<vector<int>>& grid, RenderWindow& window){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            if(grid[i][j] == 0) continue;
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
            int rand = getRandom(0,100);
            if(rand > 80) grid[i][j] = 1;
            if(rand > 90) grid[i][j] = 2;
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

    setUpGridRandom(grid);

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

        gridDisplay(grid, window);
        getNextIteration();

        // end the current frame
        window.display();
    }

    return 0;
}