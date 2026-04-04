#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

enum SymbolsXO
{
    EMPTY,
    X,
    O
};

class Players
{
    public:
    string name;
    SymbolsXO currSymbol;
};

std::unordered_map<int , pair<int,int>> mp = {
    {1, {0,0}},
    {2, {0,1}},
    {3, {0,2}},
    {4, {1,0}},
    {5, {1,1}},
    {6, {1,2}},
    {7, {2,0}},
    {8, {2,1}},
    {9, {2,2}},

};

class TicTacToeGame
{
    public:
    Players pl1, pl2;
    vector<vector<SymbolsXO>> ticTacToeMap{3,vector<SymbolsXO>(3,EMPTY)};
    void printCurrentGameSituation(vector<vector<SymbolsXO>> &ticTacToeMap);
};
