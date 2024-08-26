// g++ -c .\Particle_Sim.cpp -ID:\Workspace\SFML-2.6.1\include
// g++ .\Particle_Sim.o -o PS -LD:\Workspace\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system

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

int BLANK = 0;
int SAND = 1;
int WATER = 2;
int WOOD = 3;

int N = 300;
int M = 300;

int WIDTH = 900;
int HEIGHT = 900;

double CELL_SIZE_I = (double)HEIGHT/N;
double CELL_SIZE_J = (double)WIDTH/M;
int FPS = 120;

// TYPES

#define cord pair<int,int>
struct particle_t
{
    int state;
    bool changed;
}typedef particle_t;

vector<vector<particle_t>> grid(N, vector<particle_t>(M));

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

    if(grid[x][y].state == 0 || grid[x][y].changed) return;

    cord bc = {x+1,y};
    cord bl = {x+1,y-1};
    cord br = {x+1,y+1};
    
    cord cl = {x,y-1};
    cord cr = {x,y+1};

    cord tc = {x-1,y};
    cord tl = {x-1,y-1};
    cord tr = {x-1,y+1};

    cord next = from;
    vector<cord> rotation;
    vector<int> moveables;
    if(grid[x][y].state == SAND){
        rotation = {bc,bl,br};
        moveables = {WATER,BLANK};
    }else if(grid[x][y].state == WATER){
        rotation = {bc,bl,br,cl,cr};
        moveables = {BLANK};
    }else if(grid[x][y].state == WOOD){
        rotation = {};
        moveables = {};
    }

    for(int i = 0; i < rotation.size(); i++){
        if(next != from) break;
        cord valid = rotation[i];
        if(inBounds(valid.first,valid.second)){
            for(int j = 0; j < moveables.size(); j++){
                int moveable = moveables[j];
                if(grid[valid.first][valid.second].state == moveable) next = valid;
            }
        }
    }

    int tmp = grid[x][y].state;
    grid[x][y].state = grid[next.first][next.second].state;
    grid[next.first][next.second].state = tmp;
    grid[next.first][next.second].changed = true;
}
void getNextIteration(){
    for(int i = N-1; i >= 0 ; i--) for(int j = 0; j < M; j++) grid[i][j].changed = false;
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
    if(state == SAND) rect.setFillColor(Color::Yellow);
    if(state == WATER) rect.setFillColor(Color::Blue);
    if(state == WOOD) rect.setFillColor(Color(200,100,0));
    return rect;
}
int characterToState(char c){
    if(c == '#') return 1;
    return 0;
}
void gridDisplay(vector<vector<particle_t>>& grid, RenderWindow& window){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            if(grid[i][j].state == 0) continue;
            RectangleShape rect = stateToRect(grid[i][j].state);
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
void setUpGridRandom(vector<vector<particle_t>>& grid){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            // Depends on rule variant
            int rand = getRandom(0,100);
            if(rand > 80) grid[i][j].state = SAND;
            if(rand > 90) grid[i][j].state = WATER;
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
    for(int i = M/3; i < M*2/3; i++){
        grid[N*2/3][i].state = WOOD;
    }

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