#include <cassert>
#include <iostream>
#include <set>
#include <vector>
using namespace std;

// TODO : generatee colours as per Board Size.
enum Colors {
  RED = 2,
  BLUE,
  GREEN,
  YELLOW,
  WHITE,
  ORANGE,
  PURPLE,
}; // 1 is the queen.
enum { QUEEN = 1 };
// STATSICS
bool EnableStats = true;
unsigned long long int RecursiveFillingInvokeCount = 0;
#define LOGGING false

class Queens {
  unsigned int BSize;
  unsigned int NumColours;
  vector<vector<int>> Board;
  vector<vector<int>> InitalBoard;

  void verifyBoard() {
    // Queen count.
    int count = 0;
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++) {
        if (Board[i][j] == QUEEN)
          count++;
      }
    }
    assert(count == BSize and "Incorrect number of Queens.");

    // Verify 1 Queen per island.
    vector<int> QueenPerIsland(BSize + 2, 0);
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++) {
        if (Board[i][j] == QUEEN) {
          if (QueenPerIsland[InitalBoard[i][j]] == 1) {
            cout << "Island : " << InitalBoard[i][j]
                 << " has a queen placed on an Island with existing Queen.\n";
            assert(false);
          }
          QueenPerIsland[InitalBoard[i][j]] += 1;
        }
      }
    }
  };

public:
  Queens(int BSize)
      : BSize(BSize), NumColours(BSize), Board(BSize, vector<int>(BSize, 0)),
        InitalBoard(BSize, vector<int>(BSize, 0)) {}

  bool isValidTile(int x, int y) {
    return (x > -0 and x < BSize and y >= 0 and y < BSize);
  }

  bool isValidTile(pair<int, int> QueenTile) {
    return (QueenTile.first >= 0 and QueenTile.first < BSize and
            QueenTile.second >= 0 and QueenTile.second < BSize);
  }

  void printBoardVisual() {
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++)
        if (Board[i][j] == QUEEN)
          cout << "Q ";
        else
          cout << ". ";
      cout << "\n";
    }
  }

  void printBoardNumeric() {
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++)
        cout << Board[i][j] << " ";
      cout << "\n";
    }
  }

  vector<vector<int>> getCurrentStateBoardCopy() {
    vector<vector<int>> backup(BSize, vector<int>(BSize, 0));
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        backup[i][j] = Board[i][j];
    return backup;
  }

  // void setBoard0() {
  //   // ORANGE
  //   Board[0][1] = Board[0][2] = ORANGE;
  //   for (int i = 0; i < 6; i++)
  //     Board[i][0] = ORANGE;
  //   Board[4][1] = Board[4][2] = Board[4][3] = Board[3][2] = Board[5][2] =
  //       Board[5][3] = Board[6][2] = Board[6][3] = ORANGE;
  //   // Red
  //   Board[6][0] = Board[6][1] = Board[5][1] = Board[7][0] = Board[7][1] =
  //       Board[7][2] = Board[7][3] = Board[7][4] = RED;
  //   // Black
  //   Board[7][6] = BLACK;
  //   // Green
  //   Board[2][1] = Board[2][2] = Board[2][3] = Board[3][1] = GREEN;
  //   // Blue
  //   for (int i = 1; i < 5; i++)
  //     Board[1][i] = BLUE;
  //   for (int i = 1; i < 7; i++)
  //     Board[i][4] = BLUE;
  //   Board[3][3] = Board[3][5] = BLUE;
  //   // Purpule
  //   for (int i = 0; i < 3; i++)
  //     for (int j = 5; j < BSize; j++)
  //       Board[i][j] = PURPLE;
  //   Board[0][3] = Board[0][4] = Board[3][6] = Board[3][7] = PURPLE;

  //   // White
  //   Board[4][5] = Board[4][6] = Board[4][7] = Board[5][6] = WHITE;
  //   for (int i = 5; i < BSize; i++)
  //     for (int j = 5; j < BSize; j++)
  //       if (Board[i][j] == 0)
  //         Board[i][j] = YELLOW;

  //   for (int i = 0; i < BSize; i++)
  //     for (int j = 0; j < BSize; j++)
  //       InitalBoard[i][j] = Board[i][j];

  //   // printBoardNumeric(Board);
  // }

  void setBoard1() {
    // Purple
    for (int i = 0; i < BSize; i++) {
      Board[i][0] = PURPLE;
      if ((i >= 0 and i < 3) or (i >= 5 and i != 7))
        Board[i][1] = PURPLE;
    }
    // Red
    for (int j = 1; j < BSize - 1; j++) {
      Board[3][j] = RED;
      Board[4][j] = RED;
      if (j == 2 or j == 3 or j == 5 or j == 6)
        Board[2][j] = RED;
      if (j >= 2 and j < BSize - 2)
        Board[5][j] = RED;
    }
    Board[6][3] = Board[6][4] = Board[6][5] = Board[7][4] = RED;
    // Pink
    // for (int i = 0; i < 6; i++)
    //   Board[i][8] = PINK;
    // Board[0][5] = Board[0][6] = Board[0][7] = Board[0][8] = Board[5][7] =
    // PINK; Board[6][6] = Board[6][7] = Board[7][6] = Board[7][7] = PINK;
    // Orange
    Board[0][2] = Board[0][3] = Board[1][2] = ORANGE;
    // White
    Board[1][6] = Board[1][7] = Board[2][7] = WHITE;
    // Blue
    Board[0][4] = Board[1][4] = Board[2][4] = Board[1][3] = Board[1][5] = BLUE;
    // Black
    // Board[8][6] = Board[8][7] = Board[8][8] = Board[7][8] = Board[6][8] =
    // BLACK;
    // Yellow
    Board[7][1] = Board[7][2] = Board[7][3] = Board[8][3] = Board[6][2] =
        YELLOW;
    // Green
    Board[8][4] = Board[8][5] = Board[7][5] = GREEN;

    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        InitalBoard[i][j] = Board[i][j];
  }

  void setBoard3() {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 4; j++) {
        Board[i][j] = PURPLE;
      }
    }
    Board[1][1] = Board[1][2] = BLUE;
    Board[1][4] = Board[1][5] = PURPLE;
    for (int i = 4; i < BSize; i++) {
      for (int j = 4; j < BSize; j++)
        Board[i][j] = RED;
    }
    Board[3][BSize - 1] = RED;
    for (int i = 4; i < BSize; i++) {
      for (int j = 0; j < 4; j++) {
        Board[i][j] = GREEN;
      }
    }
    Board[5][4] = Board[3][0] = GREEN;
    Board[5][2] = YELLOW;
    for (int i = 1; i < 5; i++)
      Board[3][i] = WHITE;
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        if (Board[i][j] == 0)
          Board[i][j] = ORANGE;
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        InitalBoard[i][j] = Board[i][j];
    printBoardNumeric();
  }

  vector<pair<int, int>> getTilesOfIsland(int Island) {
    vector<pair<int, int>> Tiles;
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        if (Board[i][j] == Island)
          Tiles.push_back({i, j});
    return Tiles;
  }

  int checkNumberOfIslandsRuledOutAfterPlacing(pair<int, int> tile) {
    set<int> VisitedLands;
    int x = tile.first;
    int y = tile.second;

    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (i == 0 and j == 0)
          continue;
        if (!isValidTile(x + i, y + j))
          continue;
        int cx = i + x;
        int cy = i + y;
        if (Board[cx][cy] == -1)
          continue;
        if (VisitedLands.find(Board[cx][cy]) != VisitedLands.end())
          continue;
        VisitedLands.insert(Board[cy][cx]);
      }
    }

    for (int j = 0; j < BSize; j++) {
      if (Board[x][j] == -1)
        continue;
      if (VisitedLands.find(Board[x][j]) == VisitedLands.end())
        VisitedLands.insert(Board[x][j]);
    }

    for (int i = 0; i < BSize; i++) {
      if (Board[i][y] == -1)
        continue;
      if (VisitedLands.find(Board[i][y]) == VisitedLands.end())
        VisitedLands.insert(Board[i][y]);
    }

    return VisitedLands.size();
  }

  // If there are multiple Island that are candidates for the next
  // most constrained Island, then check if any of them are already filled
  // with a queen if yes then then such an Island cannot be a candidate for
  // the next most constrained Island. And if there still are multiple
  // candidates then choose the most constrained candiate based on the
  // constraints criteria. The constraint criteria used next would be to
  // check the number of Interferening Islands to the current Island, and
  // choose the Island with the most interfering Islands, The reason is when
  // we place Queen on this Island it will tigthen the constraints for more
  // number of Islands and thus reducing the number of trails required to
  // reach the solution.
  int chooseOptimalCandidateIsland(vector<int> &candiateIslands) {
    // TODO : Choose the most constrained Island based on the number of
    // interfering Islands.

    // For experimentation and performance evaluation, we can choose a random
    // candidate for Baseling.
    bool chooseRandom = false;
    if (chooseRandom)
      return candiateIslands[rand() % candiateIslands.size()];

    // There are two ways to do this, either construct the interference graph or
    // use a simple walk through the board, and check how many Islands will be
    // ruled out as a result of placing a Queen on this Island.
    // Well we can optimize this even more by considering even fine grained
    // analysis of each of the tiles on any Island and then make a global
    // decision based on tile level analysis.

    int island = candiateIslands[0];
    int maxRuledOutCount = 0;
    for (int &candiate : candiateIslands) {
      // Check if there is already a queen on this Island, if yes then this
      // Island cannot be a candidate for the next most constrained Island.
      bool isQueenPlaced = false;
      for (int i = 0; i < BSize; i++) {
        for (int j = 0; j < BSize; j++) {
          if (InitalBoard[i][j] == candiate and Board[i][j] == QUEEN)
            isQueenPlaced = true;
        }
      }
      if (isQueenPlaced)
        continue;

      // Now let's check how many Islands will be ruled out as a result of
      // placing a queen on for now any random tile of this Island. Later we can
      // refine the analysis to choosing the best tile on this Island based on
      // some strategy.
      vector<pair<int, int>> tilesOfCandiateIsland = getTilesOfIsland(candiate);
      for (auto tile : tilesOfCandiateIsland) {
        // Check how many Islands will be ruled out as a result of placing this
        // tile.
        int ruledOutCount = checkNumberOfIslandsRuledOutAfterPlacing(tile);
#if LOGGING
        cout << "Ruled out count seen : " << ruledOutCount << "\n";
#endif
        if (ruledOutCount > maxRuledOutCount) {
          maxRuledOutCount = ruledOutCount;
          island = candiate;
        }
      }
    }
#if LOGGING
    cout << "Island chosen : " << island
         << " Since ruled out count is :  " << maxRuledOutCount << "\n";
#endif
    return island;
  }

  vector<pair<int, int>> getNextMostConstrainedIsland(bool isStart) {
    enum STRATEGY { RANDOM, MOST_CONSTRAINED, LEAST_CONSTRAINED };
    bool tuneStrategy = false;
    if (tuneStrategy) {
      // Check random strategy to start with.
      STRATEGY strategy = RANDOM;
      if (isStart) {
        int randomIsland = rand() % BSize + 2;
        vector<pair<int, int>> Blob;
        for (int i = 0; i < BSize; i++)
          for (int j = 0; j < BSize; j++)
            if (Board[i][j] == randomIsland)
              Blob.push_back({i, j});
        return Blob;
      }
    }

    // -1 is an invalid cell, 1 is the Queen skip
    // both, 0 is not possible at this point.
    vector<int> count(BSize + 2, 0);
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++) {
        if (!isStart and Board[i][j] > 1)
          count[Board[i][j]] += 1;
        if (isStart)
          count[Board[i][j]] += 1;
      }
    }

    int island = -1;
    int MinSz = 1000000;
    for (int i = 2; i < BSize + 2; i++) {
      if (count[i] and count[i] < MinSz) {
        island = i;
        MinSz = count[i];
      }
    }

    // No canidate Island found, So this must not lead to a valid solution.
    if (island == -1)
      return {};

    vector<int> candiateIslands;
    for (int i = 2; i < BSize + 2; i++) {
      if (count[i] == MinSz)
        candiateIslands.push_back(i);
    }
    if (candiateIslands.size() == 1)
      island = candiateIslands[0];
    else if (candiateIslands.size() > 1) {
      island = chooseOptimalCandidateIsland(candiateIslands);
    }

    // Emsure no Queem is placed on this Island.
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        if (island == InitalBoard[i][j] and Board[i][j] == QUEEN)
          assert(false and "Queen placed on an Island that is a candidate for "
                           "next most constrained Island.");

    vector<pair<int, int>> Blob;
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        if (Board[i][j] == island)
          Blob.push_back({i, j});
    return Blob;
  }

  bool canPlaceQueen(pair<int, int> QueenTile) {
    assert(isValidTile(QueenTile) && "Invalid tile!");

    int x = QueenTile.first;
    int y = QueenTile.second;

    // Check if there is already a queen on this Island.
    int TileCol = InitalBoard[x][y];
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++) {
        if (InitalBoard[i][j] == TileCol) {
          if (Board[i][j] == QUEEN) {
            return false;
          }
        }
      }
    }

    // Check if there is no adjecent Queen.
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (!isValidTile(x + i, y + j))
          continue;
        if (Board[x + i][y + j] == QUEEN)
          return false;
      }
    }

    // Check if this row or col has no Queen.
    for (int i = 0; i < BSize; i++) {
      if (i != y and Board[x][i] == QUEEN)
        return false;
      if (i != x and Board[i][y] == QUEEN)
        return false;
    }

    return true;
  }

  bool tryToFillBoard(int &countOfQueensPlaced, pair<int, int> QueenTile) {
    assert(isValidTile(QueenTile) && "Invalid tile!");
    RecursiveFillingInvokeCount++;

    // Place the queen.
    int x = QueenTile.first;
    int y = QueenTile.second;

    // Get a copy of the current board in case no valid placement is found, we
    // need to revert to the current state.
    vector<vector<int>> BackupBoard = getCurrentStateBoardCopy();

    int TileCol = Board[x][y];
    Board[x][y] = QUEEN;
    countOfQueensPlaced += 1;

    if (countOfQueensPlaced == BSize) {
      printBoardVisual();
      return true;
    }

    // Mark the tiles that are rendered illegal by the placement of this queen.
    for (int i = 0; i < BSize; i++) {
      if (i != y)
        Board[x][i] = -1;
      if (i != x)
        Board[i][y] = -1;
    }
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (!isValidTile(x + i, y + j))
          continue;
        // Don't overwrite the queen
        if (i == 0 and j == 0)
          continue;
        // Mark tiles invalid.
        Board[x + i][y + j] = -1;
      }
    }
    // Also mark all the tiles of the Island as invalid, As only 1 Queen per
    // Island is allowed.
    for (int i = 0; i < BSize; i++) {
      for (int j = 0; j < BSize; j++) {
        if (Board[i][j] == -1)
          continue;
        if (Board[i][j] == TileCol)
          Board[i][j] = -1;
      }
    }
    assert(Board[x][y] == QUEEN and "Queen was accidentally removed.");

    // Now get the next most constrained island, if there are more than one
    // candidates get anyone island for now (Eventually we can choose an optimal
    // candidate by doing some kind of lookhead search)
    vector<pair<int, int>> nextIsland = getNextMostConstrainedIsland(false);

    // Now go over each of the next candidate tiles, and try to place them, if
    // sucessful in placing a queen at a tile, try to fill rest of the board, if
    // we cannot place any candidate tile, backtrack by removing the current
    // queentile placement.
    for (auto candTile : nextIsland) {
      int cx = candTile.first;
      int cy = candTile.second;
      if (canPlaceQueen(candTile)) {
        if (tryToFillBoard(countOfQueensPlaced, candTile))
          return true;
      }
    }

    // Looks like there was no valid config found, now revert the board state to
    // what we started with using the backup.
    countOfQueensPlaced -= 1;
    for (int i = 0; i < BSize; i++)
      for (int j = 0; j < BSize; j++)
        Board[i][j] = BackupBoard[i][j];

    return false;
  }

  void Fill(vector<pair<int, int>> &InitalIsland) {
    // Try every position in Initial Island whichever start post leads to a
    // valid fill, mark the fill and exit.
    for (auto cord : InitalIsland) {
      int x = cord.first;
      int y = cord.second;
      int countOfQueensPlaced = 0;
      if (tryToFillBoard(countOfQueensPlaced, cord)) {
        verifyBoard();
        return;
      }
      // Else clear the board and retry on a diff start tile.
    }
  }

  void start() {
    setBoard3();
    // Place queens in decending order of most to least constrained islands. As
    // we keep placing queens a lot of the tiles will be marked as not placeable
    // due to interference of the queens placed, and the constraints get tighter
    // on remaining queens, if no valid placement is found backtrack and try the
    // next placement of prior queens.
    vector<pair<int, int>> InitalIsland = getNextMostConstrainedIsland(true);
    Fill(InitalIsland);
    if (EnableStats) {
      cout << "\n\n STATISTICS : \n";
      cout << "\nRecursiveFillingInvokeCount : " << RecursiveFillingInvokeCount
           << "\n";
    }
  }
};

// TODO : Try to generate the board with random Islands and then try to solve.
int main() {
  Queens B(7);
  B.start();
  return 0;
}