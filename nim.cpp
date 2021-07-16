#include <bits/stdc++.h>

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

bool disjoint = false;

int mex(vector<int> s); //minimum excluded value in a vector of non-negative integers

void printx(state x); //prints a game state

class game{
    protected:
        vector<int> SG; //contains the Sprague-Grundy values for all game states

        vector< state > X; //contains all possible game states
        
        void generateX(state x0){ //generates all possible game states
            //printx(x0);
            for(auto move : moves){
                state x = x0;
                bool valid = true;
                for(int i = 0; i < x.size(); i++){
                    x[i] += move[i];
                    if(x[i] < 0) valid = false;
                }
                if(!valid) continue;
                if(find(all(X), x) != X.end()) continue;
                X.pb(x);
                if(find(all(term), x) != term.end()) break;
                generateX(x);
            }
        }

        int id(state x){ //returns the state's "id" to use as an index
            auto it = find(all(X), x);
            assert(it != X.end());
            return distance(X.begin(), it);
        }

    public:
        int VARMAX = 50; //due to memory and time restrictions, the max size of variables for possible moves is restricted

        vector< state > moves; //contains all possible moves (ways to go from one state to another)
        vector< state > term; //contains all terminal states (if a player is to play at a terminal state he/she loses)

        void printX(){ //prints all possible game states
            for(auto x : X) printx(x);
        }

        void initX(state x0){ //initializes the game states vector
            X.pb(x0);
            generateX(x0);
            sort(rall(X));
        }

        void printF(state x0){ //prints all states that we can go to in 1 move from our current state
            vector< state > f = F(x0);
            for(auto x : f) printx(x);
        }
        
        int initg(state x0){ //initializes Sprague-Grundy array
            SG.resize(X.size(), -1);
            return g(x0);
        }

        int g(state x){ //calculates sprague-grundy value of a state
            if(SG[id(x)] != -1) return SG[id(x)];
            if(F(x).empty()){
                SG[id(x)] = 0;
                return SG[id(x)];
            }
            vector<int> v;
            for(state y : F(x)) v.pb(g(y));
            SG[id(x)] = mex(v);
            return SG[id(x)];
        }

        vector< state > F(state x0){ //creates a vector with all states that we can go to in 1 move from our current state
            vector< state > f;
            for(auto move : moves){
                state x = x0;
                for(int i = 0; i < x.size(); i++) x[i] += move[i];
                if(find(all(X), x) != X.end()) f.pb(x);
            }
            return f;
        }

        void addGameMove(state temp){
            moves.pb(temp);
        }

        void addGameTerm(state temp){
            term.pb(temp);
        }
};

vector<game> Games; //Games[0] is the main game. Ff the games are disjoint, Games[1...size] are the individual games

int mex(vector<int> s){ //minimum excluded value in a vector of non-negative integers
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

void getDisjoint(int size){
    printf("Enter \"1\" if there are no moves that affect more than 1 heap in a state, \"0\" otherwise: ");
    scanf("%d", &disjoint);
    if(!disjoint){
        Games.resize(1);
        Games[0].VARMAX = 50;
    }
    else{
        Games.resize(size+1);
        for(int i = 0; i < size+1; i++) Games[i].VARMAX = 1000;
    }
}

void addMove(state temp){
    if(!disjoint) Games[0].addGameMove(temp);
    else{
        for(int i = 0; i < temp.size(); i++){
            if(temp[i] != 0) Games[i+1].addGameMove({temp[i]});
        }   
    }
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
            addMove(temp);
        }
    }
    else if(varMoves == 1){
        printf("Name your variable v. Note that max variable value is %d.\n", Games[0].VARMAX);
        printf("Enter valid moves (to stop enter \"stop\"):\n");
        while(true){
            vector<string> tempv(size);
            cin>>tempv[0];
            if(tempv[0][0] == 's') break;
            for(int j = 1; j < size; j++) cin>>tempv[j];
            for(int v = 1; v <= Games[0].VARMAX; v++){
                state temp(size);
                for(int j = 0; j < size; j++){
                    if((tempv[j][0] == 'v') || (tempv[j][0] == '-' && tempv[j][1] == 'v')){
                        if(tempv[j][0] != '-'){
                            temp[j] = v;
                        }
                        else temp[j] = -v;
                    } else temp[j] = stoi(tempv[j]);
                }
                addMove(temp);
            }
        }
    }
}

void addTerm(state temp){
    if(!disjoint) Games[0].addGameTerm(temp);
    else{
        for(int i = 0; i < temp.size(); i++){
            if(temp[i] != 0) Games[i+1].addGameTerm({temp[i]});
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
        addTerm(temp);
    }
}

int solve(state x0){
    int nimsum = 0, size = x0.size();
    if(!disjoint){
        Games[0].initX(x0);
        nimsum = Games[0].initg(x0);
    }
    else{
        for(int i = 1; i <= size; i++){
            Games[i].initX({x0[i-1]});
            nimsum ^= Games[i].initg({x0[i-1]});
        }
    }
    printf("SG value of initial state is %d\n", nimsum);
    if(nimsum != 0) printf("Player 1 wins if they play optimally!\n");
    else printf("Player 2 wins if they play optimally!\n");
    return nimsum;
}

vector< state > sumF(state x0){
    int size = x0.size();
    vector< state > f;
    for(int i = 1; i < size+1; i++){
        for(auto next : Games[i].F({x0[i-1]})){
            state x = x0;
            x[i-1] = next[0];
            f.pb(x);
        }
    }
    return f;
}

int sumg(state x0){
    int nimsum = 0, size = x0.size();
    for(int i = 1; i <= size; i++){
        nimsum ^= Games[i].g({x0[i-1]});
    }
    return nimsum;
}

void strategy(state x){ //prints the optimal strategy and how the game would be
    int size = x.size();
    if(!disjoint){
        printf("SG = %d | ", Games[0].g(x));
        printx(x);
        if(Games[0].F(x).empty()) return;
        if(Games[0].g(x) != 0){ //Current player wins if he plays optimally!
            for(state y : Games[0].F(x)){
                if(Games[0].g(y) == 0){
                    strategy(y);
                    return;
                }
            }
            assert(false); //WE SHOULD NEVER GET HERE
        }
        else{ //Previous player wins if he plays optimally, so just choose something at random
            int i = rand()%Games[0].F(x).size();
            state y = Games[0].F(x)[i];
            strategy(y);
            return;
        }
    }
    else{
        printf("SG = %d | ", sumg(x));
        printx(x);
        if(sumF(x).empty()) return;
        if(sumg(x) != 0){ //Current player wins if he plays optimally!
            for(state y : sumF(x)){
                if(sumg(y) == 0){
                    strategy(y);
                    return;
                }
            }
            assert(false); //WE SHOULD NEVER GET HERE
        }
        else{ //Previous player wins if he plays optimally, so just choose something at random
            int i = rand()%sumF(x).size();
            state y = sumF(x)[i];
            strategy(y);
            return;
        }
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
    getDisjoint(size);
    getMoves(size);
    getTerminalStates(size);
    int nimsum = solve(x0);
    strategy(x0);
}
