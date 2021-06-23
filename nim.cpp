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

const int VARMAX = 50;

vector< state > moves; //contains all possible moves (ways to go from one state to another)
vector< state > term; //contains all terminal states (if a player is to play at a terminal state he/she loses)
vector<int> SG; //contains the Sprague-Grundy values for all game states

vector< state > X; //contains all possible game states
vector< state > F(state x0); //creates a vector with all states that we can go to in 1 move from our current state

int g(state x); //calculates sprague-grundy value of a state

void strategy(state x); //prints the optimal strategy and how the game would be

void printx(state x){ //prints a game state
    for(auto el : x) printf("%d ", el);
    printf("\n");
}

void printX(){ //prints all possible game states
    for(auto x : X) printx(x);
}

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

void initX(state x0){ //initializes the game states vector
    X.pb(x0);
    generateX(x0);
    sort(rall(X));
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

void printF(state x0){ //prints all states that we can go to in 1 move from our current state
    vector< state > f = F(x0);
    for(auto x : f) printx(x);
}

int mex(vector<int> s){ //minimum excluded value in a vector of non-negative integers
    int n = 0;
    sort(all(s));
    for(auto el : s){
        if(n == el) n++;
    }
    return n;
}

int id(state x){ //returns the state's "id" to use as an index
    auto it = find(all(X), x);
    assert(it != X.end());
    return distance(X.begin(), it);
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

void initg(state x0){ //initializes Sprague-Grundy array
    SG.resize(X.size(), -1);
    int val = g(x0);
}

void strategy(state x){ //prints the optimal strategy and how the game would be
    printf("SG = %d | ", g(x));
    printx(x);
    if(F(x).empty()) return;
    if(g(x) != 0){ //Current player wins if he plays optimally!
        for(state y : F(x)){
            if(g(y) == 0){
                strategy(y);
                return;
            }
        }
        assert(false); //WE SHOULD NEVER GET HERE
    }
    else{ //Previous player wins if he plays optimally, so just choose something at random
        int i = rand()%F(x).size();
        state y = F(x)[i];
        strategy(y);
        return;
    }
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);
    int size;
    printf("Enter size of state: ");
    scanf("%d", &size);
    printf("Enter initial state: ");
    state x0(size);
    for(int i = 0; i < size; i++) scanf("%d", &x0[i]);
    int varMoves;
    printf("Is the number of moves fixed (0) or variable (1)? ");
    scanf("%d", &varMoves);
    switch(varMoves){
        case 0:{
            printf("Enter number of valid moves: ");
            int fsize;
            scanf("%d", &fsize);
            printf("Enter valid moves:\n");
            for(int i = 0; i < fsize; i++){
                state temp(size);
                for(int j = 0; j < size; j++) scanf("%d", &temp[j]);
                moves.pb(temp);
            }
            break;
        }
        case 1: {
            printf("Name your variable v. Note that max variable value is %d.\n", VARMAX);
            printf("Enter valid moves (to stop enter \"stop\"):\n");
            while(true){
                vector<string> tempv(size);
                cin>>tempv[0];
                if(tempv[0][0] == 's') break;
                for(int j = 1; j < size; j++) cin>>tempv[j];
                for(int v = 1; v <= VARMAX; v++){
                    state temp(size);
                    for(int j = 0; j < size; j++){
                        if((tempv[j][0] == 'v') || (tempv[j][0] == '-' && tempv[j][1] == 'v')){
                            if(tempv[j][0] != '-'){
                                temp[j] = v;
                            }
                            else temp[j] = -v;
                        } else temp[j] = stoi(tempv[j]);
                    }
                    moves.pb(temp);
                }
            }
            break;
        }
        default:
            printf("Invalid answer\n");
            return 0;
    }
    printf("Enter number of terminal states: ");
    int nterm;
    scanf("%d", &nterm);
    printf("Enter terminal states:\n");
    for(int i = 0; i < nterm; i++){
        state temp(size);
        for(int j = 0; j < size; j++) scanf("%d", &temp[j]);
        term.pb(temp);
    }
    initX(x0);
    initg(x0);
    int nimsum = g(x0);
    printf("SG value of initial state is %d\n", nimsum);
    if(nimsum != 0) printf("Player 1 wins if he plays optimally!\n");
    else printf("Player 2 wins if he plays optimally!\n");
    strategy(x0);
}
