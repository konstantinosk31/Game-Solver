/*
    Copyright 2021 Konstantinos Kritharidis
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.
    
    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
    
    Contact details:
    Konstantinos Kritharidis (konstantinosk31) <konstantinosk31@gmail.com>
*/

/*
More information on the readme file.
*/

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define fi first
#define se second
#define pb push_back
#define pf push_front
#define all(v) (v).begin(), (v).end()
#define rall(v) (v).rbegin(), (v).rend()

typedef long long ll;
typedef pair<int, int> ii;

#define state vector<int>

const int MAXNUM = 500;

bool disjoint = false;

int mex(vector<int> s); //minimum excluded value in a vector of non-negative integers

void printx(state x); //prints a game state

class game{ //represents a simple game
    protected:
        vector<int> SG; //contains the Sprague-Grundy values for all game states

        vector< state > moves; //contains all possible moves (ways to go from one state to another)
        vector< state > term; //contains all terminal states (if a player is to play at a terminal state he/she loses)

        vector< state > X; //contains all possible game states
        
        void generateX(state x0){ //generates all possible game states
            //printx(x0);
            for(auto move : moves){ //we try each possible move from the current state
                state x = x0; //the current state
                bool valid = true; //to check if the new state we get is valid or not
                for(int i = 0; i < x.size(); i++){
                    x[i] += move[i]; //we get the new state by applying the move to the current state
                    if(x[i] < 0) valid = false; //invalid move
                }
                if(!valid) continue; //the new state is invalid, so we don't add it
                if(find(all(X), x) != X.end()) continue; //if we've already added that state, we don't add it again
                X.pb(x); //we store the new possible state in X
                if(find(all(term), x) != term.end()) break; //if the move is to a terminal state, there are no moves from there
                generateX(x); //we generate the moves from the new state we got by applying the move
            }
        }

        int id(state x){ //returns the state's "id" to use as an index
            auto it = find(all(X), x); //iterator pointing to the state x in X
            assert(it != X.end()); //makes sure that x belongs in X
            return distance(X.begin(), it); //returns the position of x in X
        }

    public:
        int VARMAX; //due to memory and time restrictions, the max size of variables for possible moves is restricted
        int size; //state size

        void setVARMAX(){ //sets maximum variable size for a non disjoint game of a certain size
            VARMAX = (int)pow(int(2*MAXNUM/size), (double)2/size); //the complexity for non disjoint games is exponential wrt to the size of the state
        }

        game(){} //constructor for creating a new game with unknown size

        game(int size) : size(size){ //constructor for creating a new game with definite size
            setVARMAX(); //sets the maximum variable size according to game size
        }

        void printX(){ //prints all possible game states
            for(auto x : X) printx(x); //we print each state x in X
        }

        virtual void initX(state x0){ //initializes the game states vector
            X.pb(x0); //we add the initial state x0
            generateX(x0); //generate all possible game states reachable from x0 and store them in X
        }

        virtual vector< state > F(state x0){ //creates a vector with all states that we can go to in 1 move from our current state
            vector< state > f; //in f we store the followers of x0
            for(auto move : moves){ //we try all moves from state x0
                state x = x0;
                for(int i = 0; i < x.size(); i++) x[i] += move[i]; //we get each new state following x
                if(find(all(X), x) != X.end()) f.pb(x); //if x is a valid state (exists in X), we add it to the followers of x0
            }
            return f; //we return the vector f
        }

        void printF(state x0){ //prints all states that we can go to in 1 move from our current state
            vector< state > f = F(x0); //get a vector of all states immediately following x0
            for(auto x : f) printx(x); //we print each state in the vector
        }
        
        virtual int initg(state x0){ //initializes Sprague-Grundy array
            SG.resize(X.size(), -1); //make the SG array have as many places as X (one value for each state)
            return g(x0); //we return the SG value of the initial state, computing as a prerequisite the SG values of all following states
        }

        virtual int g(state x){ //calculates sprague-grundy value of a state
            if(SG[id(x)] != -1) return SG[id(x)]; //if we've already calculated the SG value of the state, we just return it and don't calculate it again
            if(F(x).empty()){ //if the state is terminal
                SG[id(x)] = 0; //SG value of terminal state is 0
                return SG[id(x)]; //we return the SG value of the state
            }
            vector<int> v;
            for(state y : F(x)) v.pb(g(y)); //v contains all SG values of the followers of x
            SG[id(x)] = mex(v); //SG value of x is the minimum excluded value of v
            return SG[id(x)]; //we return the SG value of the state
        }

        virtual void addMove(state temp){ //adds a move to the list of available moves
            moves.pb(temp);
        }

        virtual void addTerm(state temp){ //adds a terminal state to the list of terminal states of the game
            term.pb(temp);
        }
};

class sumGame : public game{ //represents the sum of disjoint sub-games
    public:
        vector<game> games; //the list of sub-games of which the sum-game consists
        int size; //the amount of sub-games in the sum-game

        void setVARMAX(){ //sets maximum variable size for a sum of disjoint games of a certain size
            int val = MAXNUM;
            VARMAX = val; //the complexity for sums of disjoint games is linear wrt to the size of the sum-state
            for(auto game : games) game.VARMAX = val; //the sub-games have the same VARMAX
        }

        sumGame(int size) : size(size){ //constructor for creating a new sum-game with definite size
            games.resize(size);
            setVARMAX(); //sets the maximum variable size according to game size
        }

        sumGame(vector<game> games) : games(games){ //constructor for creating a new sum-game with a vector of sub-games
            size = games.size();
            setVARMAX(); //sets the maximum variable size according to game size
        }

        void initX(state x0){ //we don't actually store every possible state in a sum-game to minimise space and time complexity
            for(int i = 0; i < size; i++){
                this->games[i].initX({x0[i]}); //instead we store every possible state in each sub-game and later combine them when necessary
            }
        }

        vector< state > F(state x0){ //creates a vector with all states that we can go to in 1 move from our current state in the sum-game
            int size = x0.size();
            vector< state > f; //in f we store the followers of x0
            for(int i = 0; i < size; i++){ //since the sub-games are disjoint, we only change one heap at a time
                for(auto next : games[i].F({x0[i]})){ //we get the followers from each sub-game
                    state x = x0;
                    x[i] = next[0]; //next[] only has one element since each sub-game has a size of 1 and we only change the heap that corresponds to that sub-game
                    f.pb(x); //we add the state to the followers of x0
                }
            }
            return f; //we return the vector f
        }

        int g(state x0){ //calculates sprague-grundy value of a state
            int nimsum = 0, size = x0.size();
            for(int i = 0; i < size; i++){
                nimsum ^= games[i].g({x0[i]}); //SG value of a state in a sum-game is the xor-sum of the SG values of the sub-games
            }
            return nimsum; //we return the SG value of the state
        }

        int initg(state x0){ //initializes Sprague-Grundy array of each sub-game in the sum-game
            int nimsum = 0;
            for(int i = 0; i < size; i++){
                nimsum ^= this->games[i].initg({x0[i]}); //SG value of a state in a sum-game is the xor-sum of the SG values of the sub-games
            }
            return nimsum; //returns the SG value of the initial state in the sum-game
        }

        void addMove(state temp){ //adds a move to the list of available moves in the sum-game
            for(int i = 0; i < temp.size(); i++){
                if(temp[i] != 0) this->games[i].addMove({temp[i]}); //the move only affects one heap so we only add it to that one
            }
        }

        void addTerm(state temp){ //adds a terminal state to the list of terminal states of the sum-game
            for(int i = 0; i < temp.size(); i++){
                this->games[i].addTerm({temp[i]}); //the terminal state ends each sub-game
            }
        }
};

game* Game;

int mex(vector<int> s){ //returns the minimum excluded value in a vector of non-negative integers
    int n = 0;
    sort(all(s));
    for(auto el : s){
        if(n == el) n++;
    }
    return n;
}

void printx(state x){ //prints a game state
    for(auto el : x) printf("%d ", el);
    printf("\n");
}

void getDisjoint(int size){ //check if the game can be expressed as the sum of distinct sub-games
    printf("Enter \"1\" if there are no moves that affect more than 1 heap in a state, \"0\" otherwise: ");
    scanf("%d", &disjoint);
    if(!disjoint) Game = new game(size); //if not, then the entire game is a normal game of size = size
    else Game = new sumGame(size); //otherwise, the game is the sum-game of sub games of size = 1 and amount = size
}

void getMoves(int size){ //the user inputs the available moves in the game
    int varMoves;
    printf("Is the number of moves fixed (0) or variable (1)? ");
    scanf("%d", &varMoves);
    if(varMoves == 0){
        printf("Enter number of valid moves: ");
        int fsize;
        scanf("%d", &fsize);
        printf("Enter valid moves:\n");
        for(int i = 0; i < fsize; i++){
            state temp(size);
            for(int j = 0; j < size; j++) scanf("%d", &temp[j]);
            Game->addMove(temp);
        }
    }
    else if(varMoves == 1){
        printf("Name your variable v. Note that max variable value is %d.\n", Game->VARMAX);
        printf("Enter valid moves (to stop enter \"stop\"):\n");
        while(true){
            vector<string> tempv(size);
            cin>>tempv[0];
            if(tempv[0][0] == 's') break;
            for(int j = 1; j < size; j++) cin>>tempv[j];
            for(int v = 1; v <= Game->VARMAX; v++){
                state temp(size);
                for(int j = 0; j < size; j++){
                    if((tempv[j][0] == 'v') || (tempv[j][0] == '-' && tempv[j][1] == 'v')){
                        if(tempv[j][0] != '-'){
                            temp[j] = v;
                        }
                        else temp[j] = -v;
                    } else temp[j] = stoi(tempv[j]);
                }
                Game->addMove(temp);
            }
        }
    }
}

void getTerminalStates(int size){ //the user inputs the terminal states of the game
    printf("Enter number of terminal states: ");
    int nterm;
    scanf("%d", &nterm);
    printf("Enter terminal states:\n");
    for(int i = 0; i < nterm; i++){
        state temp(size);
        for(int j = 0; j < size; j++) scanf("%d", &temp[j]);
        Game->addTerm(temp);
    }
}

void solve(state x0){ //finds the winning player in the game
    int nimsum = 0, size = x0.size();
    Game->initX(x0);
    nimsum = Game->initg(x0);
    printf("SG value of initial state is %d\n", nimsum);
    if(nimsum != 0) printf("Player 1 wins if they play optimally!\n");
    else printf("Player 2 wins if they play optimally!\n");
}

void strategy(state x){ //prints the optimal strategy and how the game would be
    int size = x.size();
    printf("SG = %d | ", Game->g(x));
    printx(x);
    if(Game->F(x).empty()) return;
    if(Game->g(x) != 0){ //Current player wins if they play optimally!
        for(state y : Game->F(x)){
            if(Game->g(y) == 0){
                strategy(y);
                return;
            }
        }
        assert(false); //WE SHOULD NEVER GET HERE
    }
    else{ //Previous player wins if they play optimally, so just choose a move at random
        int i = rand()%Game->F(x).size();
        state y = Game->F(x)[i];
        strategy(y);
        return;
    }
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    int size;
    printf("Enter size of state: ");
    scanf("%d", &size);
    printf("Enter initial state: ");
    state x0(size);
    for(int i = 0; i < size; i++) scanf("%d", &x0[i]);
    getDisjoint(size); //optimization based on whether the game can be expressed as the sum of distinct sub-games
    getMoves(size);
    getTerminalStates(size);
    solve(x0);
    strategy(x0);
}
