#include "TicTacToe.hpp"
string deducePrint(SymbolsXO s)
{
    if(s==SymbolsXO::X)
    {
        return "X";
    }
    else if(s==SymbolsXO::O)
    {
        return "O";
    }
    else
    {
        return " ";
    }
}

void TicTacToeGame::printCurrentGameSituation(vector<vector<SymbolsXO>> &ticTacToeMap)
{
    for(int i=0;i<ticTacToeMap.size();i++)
    {
        if(i!=0)
        {
            std::cout<<"-------------"<<std::endl;
        }
        for(int j=0;j<ticTacToeMap[0].size();j++)
        {
            std::cout<<deducePrint(ticTacToeMap[i][j])<<" | ";
        }
        std::cout<<std::endl;
    }
}

SymbolsXO deduceChoosedSymbol(char choosedChar)
{
    return choosedChar == 'X' ? SymbolsXO::X : SymbolsXO::O;
}

void initializePlayers(TicTacToeGame &game)
{
    for(int i=0;i<2;i++)
    {
        string takeName;
        std::cout<<"Enter player "<<i+1<<" name : "<<std::endl;
        cin>>takeName;
        if(i==0)
        {
            char chooseSymbol;
            std::cout<<"Choose a symbol for "<<takeName<<" : (X or O)"<<std::endl;
            cin>>chooseSymbol;
            SymbolsXO deducedSymbol = deduceChoosedSymbol(chooseSymbol);
            game.pl1.name=takeName;
            game.pl1.currSymbol = deducedSymbol;
            game.pl2.currSymbol = deducedSymbol == SymbolsXO::X ? SymbolsXO::O : SymbolsXO::X;
        }
        else
        {
            game.pl2.name=takeName;
            cout<<takeName<<" symbol is : "<<deducePrint(game.pl2.currSymbol)<<endl;
        }
    }
}

bool checkIfPlayerHasWon(TicTacToeGame &game, Players &p)
{
    for(int i=0; i<3;i++)
    {
        if((game.ticTacToeMap[i][0]==p.currSymbol && game.ticTacToeMap[i][1]==p.currSymbol && game.ticTacToeMap[i][2]==p.currSymbol))
        {
            return true;
        }
    }
    for(int i=0; i<3;i++)
    {
        if((game.ticTacToeMap[0][i]==p.currSymbol && game.ticTacToeMap[1][i]==p.currSymbol && game.ticTacToeMap[2][i]==p.currSymbol))
        {
            return true;
        }
    }
    //diagonal
    if((game.ticTacToeMap[0][0]==p.currSymbol && game.ticTacToeMap[1][1]==p.currSymbol && game.ticTacToeMap[2][2]==p.currSymbol)
    || (game.ticTacToeMap[2][0]==p.currSymbol && game.ticTacToeMap[1][1]==p.currSymbol && game.ticTacToeMap[0][2]==p.currSymbol))
    {
        return true;
    }
    return false;
}

int main()
{
    TicTacToeGame game;
    game.printCurrentGameSituation(game.ticTacToeMap);
    initializePlayers(game);
    int spaceExhaust = 9;
    bool isNotDraw = false;
    while(spaceExhaust)
    {
        bool player1Entry = false;
        while(!player1Entry)
        {
            std::cout<<"Enter where you want put the symbol "<<game.pl1.currSymbol<<" (1-9) : ";
            int choiceofPl1;
            std::cin>>choiceofPl1;
            if(choiceofPl1>=1 && choiceofPl1<=9)
            {
                auto index = mp[choiceofPl1];
                if(game.ticTacToeMap[index.first][index.second] == SymbolsXO::EMPTY)
                {
                    game.ticTacToeMap[index.first][index.second] = game.pl1.currSymbol;
                    spaceExhaust--; 
                    player1Entry = true;
                }
                else
                {
                    std::cout<<"Spot already taken choose another one "<<std::endl;
                }
            }
            else
            {
                std::cout<<"Wrong choice re enter"<<std::endl;
            }
        }
        game.printCurrentGameSituation(game.ticTacToeMap);
        if(spaceExhaust==0){break;}
        if(!checkIfPlayerHasWon(game, game.pl1))
        {
            bool player1Entry = false;
            while(!player1Entry)
            {
                std::cout<<"Enter where you want put the symbol "<<game.pl2.currSymbol<<" (1-9) : ";
                int choiceofPl2;
                std::cin>>choiceofPl2;
                if(choiceofPl2>=1 && choiceofPl2<=9)
                {
                    auto index = mp[choiceofPl2];
                    if(game.ticTacToeMap[index.first][index.second] == SymbolsXO::EMPTY)
                    {
                        game.ticTacToeMap[index.first][index.second] = game.pl2.currSymbol;
                        spaceExhaust--; 
                        player1Entry = true;
                    }
                    else
                    {
                        std::cout<<"Spot already taken choose another one "<<std::endl;
                    }
                }
                else
                {
                    std::cout<<"Wrong choice re enter"<<std::endl;
                }
            }
            game.printCurrentGameSituation(game.ticTacToeMap);
        }
        else
        {
            cout<<game.pl1.name<<" has won the game ! "<<std::endl;
            game.printCurrentGameSituation(game.ticTacToeMap);
            isNotDraw = true;
            break;
        }
        if(checkIfPlayerHasWon(game, game.pl2))
        {
            cout<<game.pl2.name<<" has won the game ! "<<std::endl;
            game.printCurrentGameSituation(game.ticTacToeMap);
            isNotDraw = true;
            break;
        }
    }

    if(!isNotDraw)
    {
        cout<<"Game ended with a draw "<<std::endl;
        game.printCurrentGameSituation(game.ticTacToeMap);
    }

    return 0;

}