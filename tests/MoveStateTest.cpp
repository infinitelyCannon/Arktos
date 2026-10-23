#include <Board.h>
#include <Exception.h>
#include <iostream>
#include <fstream>
#include <format>
#include <utility>
#include <algorithm>

#include "Arktos.h"
#include "Board.h"
#include "Types.h"

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: MoveStateTest NUM_OF_GAMES PATH_TO_FILE(S)" << std::endl;
        return 1;
    }

    Arktos::PrintVersion();

    const int numGames = std::stoi(argv[1]);
    Arktos::Board board;
    std::vector<std::pair<Arktos::Move, std::string>> moves; // wish I knew how to stream a file backwards, but until then...
    board.Init();

    for (int i = 1; i <= numGames; ++i)
    {
        std::ifstream file(std::format("{}/game{}.txt", argv[2], i));
        std::string line;
        int lineNum = 1;

        if (file.is_open())
        {
            std::cout << "Reading " << argv[2] << "/game" << i << ".txt...\n";
            board.SetFEN("startpos");
            while (std::getline(file, line))
            {
                try
                {
                    size_t split = line.find('|');
                    Arktos::Move move = board.ParseMove(line.substr(0, split));
                    moves.emplace_back(move, line.substr(split + 1));
                    board.MakeMove(move);
                    board.CheckFEN(line.substr(split + 1));
                }
                catch (const Arktos::StateException &err)
                {
                    std::cerr << "Exception thrown at move (" << lineNum << "): " << err.what() << std::endl;
                    file.close();
                    return 1;
                }
                ++lineNum;
            }
            file.close();

            try
            {
                --lineNum;
                std::for_each(moves.rbegin(), moves.rend(), [&board, &lineNum](const std::pair<Arktos::Move, std::string> pair){
                    board.CheckFEN(pair.second);
                    board.UnMakeMove(pair.first);
                    --lineNum;
                });
                board.CheckFEN("startpos");
                moves.clear();
            }
            catch(const Arktos::StateException &err)
            {
                std::cerr << "Exception while undoing at line (" << lineNum << "): " << err.what() << std::endl;
                return 1;
            }
        }
        else
        {
            std::cerr << "Unable to open game file at " << argv[2] << "/" << "game" << i << ".txt\n";
            return 1;
        }
    }

    return 0;
}