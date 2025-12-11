#include <Board.h>
#include <Exception.h>
#include <iostream>
#include <fstream>
#include <format>
#include "Board.h"
#include "Types.h"

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cerr << "Usage: MoveStateTest NUM_OF_GAMES PATH_TO_FILE(S)" << std::endl;
        return 1;
    }

    const int numGames = std::stoi(argv[1]);
    Arktos::Board board;

    for (int i = 1; i <= numGames; ++i)
    {
        std::ifstream file(std::format("{}/game{}.txt", argv[2], i));
        std::string line;
        int lineNum = 1;

        if (file.is_open())
        {
            board.Init("startpos");
            while (std::getline(file, line))
            {
                try
                {
                    size_t split = line.find('|');
                    Arktos::Move move = board.ParseMove(line.substr(0, split));
                    board.MakeMove(move);
                    board.CheckFEN(line.substr(split + 1));
                }
                catch (Arktos::StateException &err)
                {
                    std::cerr << "Exception thrown at move (" << lineNum << "): " << err.what() << std::endl;
                    return 1;
                }
                ++lineNum;
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open game file at " << argv[2] << "/" << "game" << i << ".txt\n";
            return 1;
        }
    }

    return 0;
}