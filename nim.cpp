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

const int MAXNUM = 500;

bool disjoint = false;

int mex(vector<int> s); //minimum excluded value in a vector of non-negative integers

void printx(state x); //prints a game state

class game{
    protected:
        vector<int> SG; //contains the Sprague-Grundy values for all game states

        vector< state > moves; //contains all possible moves (ways to go from one state to another)
        vector< state > term; //contains all terminal states (if a player is to play at a terminal state he/she loses)

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
        int VARMAX; //due to memory and time restrictions, the max size of variables for possible moves is restricted
        int size;

        void setVARMAX(){
            VARMAX = (int)pow(int(2*MAXNUM/size), (double)2/size);
        }

        game(){}

        game(int size) : size(size){
            setVARMAX();
        }

        void printX(){ //prints all possible game states
            for(auto x : X) printx(x);
        }

        virtual void initX(state x0){ //initializes the game states vector
            X.pb(x0);
            generateX(x0);
            sort(rall(X));
        }

        void printF(state x0){ //prints all states that we can go to in 1 move from our current state
            vector< state > f = F(x0);
            for(auto x : f) printx(x);
        }
        
        virtual int initg(state x0){ //initializes Sprague-Grundy array
            SG.resize(X.size(), -1);
            return g(x0);
        }

        virtual int g(state x){ //calculates sprague-grundy value of a state
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

        virtual vector< state > F(state x0){ //creates a vector with all states that we can go to in 1 move from our current state
            vector< state > f;
            for(auto move : moves){
                state x = x0;
                for(int i = 0; i < x.size(); i++) x[i] += move[i];
                if(find(all(X), x) != X.end()) f.pb(x);
            }
            return f;
        }

        virtual void addMove(state temp){
            moves.pb(temp);
        }

        virtual void addTerm(state temp){
            term.pb(temp);
        }
};

class sumGame : public game{
    public:
        vector<game> games;
        int size;

        void setVARMAX(){
            int val = MAXNUM;
            VARMAX = val;
            for(auto game : games) game.VARMAX = val;
        }

        sumGame(int size) : size(size){
            games.resize(size);
            setVARMAX();
        }

        sumGame(vector<game> games) : games(games){
            size = games.size();
            setVARMAX();
        }

        void initX(state x0){
            for(int i = 0; i < size; i++){
                this->games[i].initX({x0[i]});
            }
        }

        vector< state > F(state x0){
            int size = x0.size();
            vector< state > f;
            for(int i = 0; i < size; i++){
                for(auto next : games[i].F({x0[i]})){
                    state x = x0;
                    x[i] = next[0];
                    f.pb(x);
                }
            }
            return f;
        }

        int g(state x0){
            int nimsum = 0, size = x0.size();
            for(int i = 0; i < size; i++){
                nimsum ^= games[i].g({x0[i]});
            }
            return nimsum;
        }

        int initg(state x0){
            int nimsum = 0;
            for(int i = 0; i < size; i++){
                nimsum ^= this->games[i].initg({x0[i]});
            }
            return nimsum;
        }

        void addMove(state temp){
            for(int i = 0; i < temp.size(); i++){
                if(temp[i] != 0) this->games[i].addMove({temp[i]});
            }
        }

        void addTerm(state temp){
            for(int i = 0; i < temp.size(); i++){
                this->games[i].addTerm({temp[i]});
            }
        }
};

game* Game;

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
    if(!disjoint) Game = new game(size);
    else Game = new sumGame(size);
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

void solve(state x0){
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
    if(Game->g(x) != 0){ //Current player wins if he plays optimally!
        for(state y : Game->F(x)){
            if(Game->g(y) == 0){
                strategy(y);
                return;
            }
        }
        assert(false); //WE SHOULD NEVER GET HERE
    }
    else{ //Previous player wins if he plays optimally, so just choose something at random
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
    getDisjoint(size);
    getMoves(size);
    getTerminalStates(size);
    solve(x0);
    strategy(x0);
}
