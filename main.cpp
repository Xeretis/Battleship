#include <iostream>
#include <random>

#define BOARD_SIZE 10
#define MAX_ATTEMPTS 30

enum class Axis {
    X, Y
};

struct Field {
    char value = '-';
    bool revealed = false;
};

void clear();

char getDisplayValue(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], const Field& field);

void displayBoard(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]);

void displayRawBoard(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]);

void clearBoard(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]);

bool placeShip(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], int x, int y, int length, Axis axis, char shipChar);

void populateField(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], const std::vector<int>& ships);

bool checkWin(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]);

int main() {

    Field playingBoard[BOARD_SIZE][BOARD_SIZE];

    std::vector<int> ships;
    int currentLength;

    std::cin >> currentLength;

    while (currentLength != -1) {
        ships.emplace_back(currentLength);

        std::cin >> currentLength;
    }

    populateField(playingBoard, ships);

    while (true) {
        clear();
        displayBoard(playingBoard);

        unsigned int row, column;

        std::cin >> row >> column;

        if (row - 1 >= BOARD_SIZE || column - 1 >= BOARD_SIZE  || row == 0 || column == 0) {
            std::cout << "Ervenytelen sor/oszlop!";

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        } else if (playingBoard[row - 1][column - 1].revealed) {
            std::cout << "Erre a mezore mar tippeltel!";

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        } else {
            playingBoard[row - 1][column - 1].revealed = true;

            if (checkWin(playingBoard))
                break;
        }
    }

    clear();

    displayRawBoard(playingBoard);

    std::cout << "Nyertel!";

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}

void clear() {
    std::cout << "\033[2J\033[1;1H";
}

char getDisplayValue(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], const Field& field) {
    if (field.value == '-')
        return 'O';

    bool isSank = true;

    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (playingBoard[i][j].value == field.value && playingBoard[i][j].revealed == false) {
                isSank = false;
                break;
            }

    return isSank ? 'S' : 'T';
}

void clearBoard(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            playingBoard[i][j].value = '-';
}

void displayBoard(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            std::cout << (playingBoard[i][j].revealed ? getDisplayValue(playingBoard, playingBoard[i][j]) : 'X') << ' ';
        }
        std::cout << std::endl;
    }
}

void displayRawBoard(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            std::cout << playingBoard[i][j].value << ' ';
        }
        std::cout << std::endl;
    }
}

bool placeShip(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], int x, int y, int length, Axis axis, char shipChar) {
    if (axis == Axis::X) {
        if (x + length > BOARD_SIZE - 1)
            return false;
        for (int i = 0; i < length; ++i)
            if(playingBoard[length > 0 ? x + i : x - i][y].value != '-')
                return false;
        for (int i = 0; i < length; ++i)
            playingBoard[length > 0 ? x + i : x - i][y].value = shipChar;
    } else {
        if (y + length > BOARD_SIZE - 1)
            return false;
        for (int i = 0; i < length; ++i)
            if(playingBoard[x][length > 0 ? y + i : y - i].value != '-')
                return false;
        for (int i = 0; i < length; ++i)
            playingBoard[x][length > 0 ? y + i : y - i].value = shipChar;
    }
    return true;
}

void populateField(Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE], const std::vector<int>& ships) {
    std::random_device rd;
    std::default_random_engine engine(rd());

    std::uniform_int_distribution<int> posDist (0, 8);
    std::uniform_int_distribution<int> dirDist (0, 1);

    for (int i = 0; i < ships.size(); ++i) {

        char shipChar = 65 + i;

        bool valid = false;
        int attempts = 0;

        while (!valid) {

            int x = posDist(engine);
            int y = posDist(engine);

            switch (dirDist(engine)) {
                case 0:
                    if (placeShip(playingBoard, x, y, ships[i], Axis::X, shipChar)) {
                        valid = true;
                        attempts = 0;
                    } else
                        goto switchEnd;
                    break;
                case 1:
                    if (placeShip(playingBoard, x, y, ships[i], Axis::Y, shipChar)) {
                        valid = true;
                        attempts = 0;
                    } else
                        goto switchEnd;
                    break;
            }
            switchEnd: attempts++;

            if (attempts >= MAX_ATTEMPTS) {
                clearBoard(playingBoard);
                populateField(playingBoard, ships);
                return;
            }
        }
    }
}

bool checkWin(const Field (&playingBoard)[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            if (!(playingBoard[i][j].value == '-' || playingBoard[i][j].revealed))
                return false;

    return true;
}