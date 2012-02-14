#include <iostream>
#include <boost/optional.hpp>

#include "cccallcc_fork.hpp"

boost::optional< cont<bool> > checkpoint;

bool guess() {
    return call_cc<bool>( [](cont<bool> k) {
        checkpoint = k;
        return true;
    } );
}

void fail() NORETURN;
void fail() {
    if (checkpoint) {
        (*checkpoint)(false);
    } else {
        std::cerr << "Nothing to be done." << std::endl;
        exit(1);
    }
}

void cut() {
    checkpoint = boost::none;
}

int integer(int m, int n) {
    for (int i=m; i<=n; i++) {
        if (guess())
            return i;
    }
    fail();
}

void factor(int n) {
    const int i = integer(2, 100);
    const int j = integer(2, 100);

    if (i*j != n)
        fail();

    std::cout << i << " * " << j << " = " << n << std::endl;
}

const int maze_size = 15;
char maze[] =
    "X-------------+\n"
    "        |     |\n"
    "|--+  | |   | |\n"
    "|  |  | | --+ |\n"
    "|     |     | |\n"
    "|-+---+--+- | |\n"
    "| |      |    |\n"
    "| | | ---+-+- |\n"
    "|   |      |  |\n"
    "| +-+-+--|    |\n"
    "| |   |  |--- |\n"
    "|     |       |\n"
    "|--- -+-------|\n"
    "|              \n"
    "+------------- \n";

void solve_maze() {
    int x=0, y=0;

    while ((x != maze_size-1)
        || (y != maze_size-1)) {

             if (guess()) x++;
        else if (guess()) x--;
        else if (guess()) y++;
        else              y--;

        if (  (x < 0) || (x >= maze_size) ||
              (y < 0) || (y >= maze_size)  )
            fail();

        const int i = y*(maze_size+1) + x;
        if (maze[i] != ' ')
            fail();
        maze[i] = 'X';
    }

    for (char c : maze) {
        if (c == 'X')
            std::cout << "\e[1;32mX\e[0m";
        else
            std::cout << c;
    }
}

int main() {
    factor(391);
    cut();
    solve_maze();
    return 0;
}
