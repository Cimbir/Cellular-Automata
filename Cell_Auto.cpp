#include <iostream>
#include <vector>
#include <stdlib.h>
#include <random>
#include <chrono>

using namespace std;

// UNCHANGEABLE CONSTANTS

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
vector<int> offsets = {1, 0, -1, -1, 1, -1, 0, 1, 1};

// CHANGEABLE CONSTANTS

int STATE_AMOUNT = 2;

int N = 5;
int M = 5;

// TYPES

#define cord pair<int,int>

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
void getNextIteration(vector<vector<int>>& before, vector<vector<int>>& after){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            after[i][j] = getStateByRules({i,j}, before);
        }
    }
}

// DISPLAY

char stateToCharacter(int state){
    if(state == 1) return '#';
    return ' ';
}
int characterToState(char c){
    if(c == '#') return 1;
    return 0;
}
void consoleDisplay(vector<vector<int>>& grid){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < M; j++){
            cout << stateToCharacter(grid[i][j]);
        }
        cout << endl;
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

    vector<vector<int>> before(N, vector<int>(M));
    vector<vector<int>> after(N, vector<int>(M));

    vector<string> initialSetup = {
        "#    ",
        "  #  ",
        "  ## ",
        "  #  ",
        "     "
    };
    setUpGrid(initialSetup, before);

    while(true){
        getNextIteration(before,after);
        consoleDisplay(after);
        _sleep(1000);
        getNextIteration(after,before);
        consoleDisplay(before);
        _sleep(1000);   
    }

    return 0;
}