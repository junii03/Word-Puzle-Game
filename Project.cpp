#include <fstream>
#include <iostream>
using namespace std;

// Function Prototypes
void MENU();
bool loadPuzzle(ifstream &fin, char board[], const int ROW, const int COL, char **puzzle);
bool dictionaryMatch(ifstream &fin, char dictionary[], char word[]);
bool easyLevel(char **puzzle, char word[], int row, int col);
bool interLevel(char **puzzle, char word[], int row, int col);
bool hardLevel(char **puzzle, char word[], int row, int col);
int charLen(char[]);
bool charCmp(char[], char[]);
void toUpper(char[]);
void toLower(char[]);
bool displayHighscore(ifstream &fin, char highscore[]);
void Game(int &lives, int &score, const int ROW, const int COL, char **, char word[], ofstream &fout, char pause[], ifstream &fin, char dictionary[], char level);
void scoreLifeUpdate(char level, char **puzzle, char word[], const int ROW, const int COL, int &score, int &lives);
bool resumeGame(ifstream &fin, char pause[], int &lives, int &score, const int ROW, const int COL, char **puzzle);
void updateHighScore(ifstream &fin, char highscore[], int score, ofstream &fout, char name[]);

int main()
{
    // Initialization
    ifstream fin;
    ofstream fout;
    int ROW = 15, COL = 15;

    // Dynamically allocate memory for puzzle
    char **puzzle = new char *[ROW];
    for (int i = 0; i < ROW; i++)
    {
        puzzle[i] = new char[COL];
    }

    // Dynamically allocate memory for word and name
    char *word = new char[ROW]{0};
    char *name = new char[ROW]{0};

    // File names
    char board[]{"board.txt"};
    char dictionary[]{"dictionary.txt"};
    char pause[]{"PauseGame.txt"};
    char highscore[]{"Highscore.txt"};

    char choice;
    char level = 'e';
    int lives = 3, score = 0;
    int dspScore = 0;

    // Display game title
    cout << "\t\t\tWORD SEARCH PUZZLE\n";

    // Main game loop
    do
    {
        // Display menu
        MENU();

        cout << "Your choice: ";
        cin >> choice;
        cin.ignore();
        cout << endl;

        // Load puzzle from file
        if (!loadPuzzle(fin, board, ROW, COL, puzzle))
        {
            cout << "The game is unable to load puzzle file\nSorry for the inconvenience\nExiting\n";
            return 1;
        }

        // Switch based on user choice
        switch (choice)
        {
        case 'n':
        case 'N':
        {
            // Start a new game
            cout << "You have selected to start a new game\n\n";

            // Player Inputs
            cout << "Enter player name: ";
            cin.getline(name, ROW);
            cout << "Hello player: \"" << name << "\" You have " << lives << " lives and your score is " << score << endl;
            cout << endl;

            // Start the game
            Game(lives, score, ROW, COL, puzzle, word, fout, pause, fin, dictionary, level);
            break;
        }
        case 'r':
        case 'R':
        {
            // Resume a saved game
            if (!resumeGame(fin, pause, lives, score, ROW, COL, puzzle))
            {
                cout << "Unable to load saved game\nSorry for inconvenience\n";
                break;
            }
            // Continue the game
            Game(lives, score, ROW, COL, puzzle, word, fout, pause, fin, dictionary, level);
            break;
        }
        case 'l':
        case 'L':
        {
            // Change difficulty level
            cout << "Change Difficulty level\nEnter \'e\' for easy / \'i\' for intermediate / \'h\' for hard: ";
            cin >> level;
            cin.ignore();

            // If unable to resume, load a new puzzle
            if (!resumeGame(fin, pause, lives, score, ROW, COL, puzzle))
            {
                loadPuzzle(fin, board, ROW, COL, puzzle);
            }
            break;
        }
        case 'h':
        case 'H':
        {
            // Display highscores
            if (!displayHighscore(fin, highscore))
            {
                cout << "Unable to check for highscores\nSorry for inconvenience\n";
            }
            break;
        }
        case 'e':
        case 'E':
        {
            // Exit the game
            cout << "Are you sure you want to exit\nPress e to exit or any other key to continue\n";
            cin >> choice;
            break;
        }
        default:
        {
            // Invalid choice
            cout << "!!Invalid choice!!\nNo operation exists against your choice\n";
            break;
        }
        }

        if (lives == 0 && score != 0)
        {
            dspScore = score;

            // Update highscore
            updateHighScore(fin, highscore, score, fout, name);
            // reset score
            score = 0;
        }
    } while (choice != 'e' && choice != 'E');

    // Delete dynamically allocated memory
    for (int i = 0; i < ROW; i++)
    {
        delete[] puzzle[i];
    }
    delete[] puzzle;

    delete[] name;
    delete[] word;

    // Remove pause file
    if (remove(pause) == 0)
    {
        cout << "!!Record for paused game deleted successfuly!!\n";
    }

    // Display game over message
    cout << "Game Over!!! You have scored " << dspScore << " points\n";
    cout << "Thank you for playing\n\n";

    return 0;
}

/*Display Menu of the game*/
void MENU()
{
    cout << "\n\t\tMENU\n";
    cout << "N/n - Start new game\n";
    cout << "R/r - Resume game (If any)\n";
    cout << "L/l - Change difficulty level\n";
    cout << "H/h - Highscores\n";
    cout << "E/e - Exit\n";
    cout << "------------------------\n";
}

// Checks if the HighScore is higher than previous 5 then updates it
void updateHighScore(ifstream &fin, char highscore[], int score, ofstream &fout, char name[])
{
    // Open highscore file
    fin.open(highscore);
    if (!fin.is_open())
    {
        cout << "Unable to check highscores\n";
        return;
    }

    bool high = false;
    char Name[15];
    int highScore;
    int rank = 0;

    // Check if the current score is higher than the existing high scores
    while (fin >> Name >> highScore)
    {
        if (score >= highScore)
        {
            high = true;
            break;
        }
        rank++;
    }

    // Close the file
    fin.close();

    // If high score, update the high scores
    if (high)
    {
        fin.open(highscore);
        fout.open("temp.txt");
        if (!(fin.is_open() || fout.is_open()))
        {
            cout << "Error updating highscores\n";
            return;
        }

        // Copy the top 4 high scores and insert the new score
        for (int i = 0; i < 5; i++)
        {
            if (i == rank)
            {
                fout << name << "\t" << score << endl;
                if (i == 4)
                    break; // If the new score is the 5th, stop writing
            }
            fin >> Name >> highScore;
            fout << Name << "\t" << highScore << endl;
        }

        // Close the files
        fout.close();
        fin.close();

        // Remove the old highscore file and rename the temp file
        if (remove(highscore) != 0)
        {
            cout << "!!Error deleting \'" << highscore << "\'!!\n";
        }
        if (rename("temp.txt", highscore) != 0)
        {
            cout << "!!Error updating \'" << highscore << "\'!!\n";
        }
    }
}

// Function to check if a word matches any word in the dictionary
bool dictionaryMatch(ifstream &fin, char dictionary[], char word[])
{
    // Open the dictionary file
    fin.open(dictionary);

    // If the file cannot be opened, print an error message and exit the program
    if (!fin.is_open())
    {
        cout << "Unable to open \"" << dictionary << "\" file to match your word\nSorry for inconvinience\nExitting\n";
        exit(3);
    }

    // Variable to hold each word read from the file
    char fileWord[50];

    // Convert word into lower case as all data in dictionary file is in lower case
    toLower(word);

    // Loop through each word in the file
    while (fin >> fileWord)
    {
        // If the word matches the word in the file, close the file and return true
        if (charCmp(word, fileWord))
        {
            fin.close();
            return true;
        }
    }

    // Close the file and return false if no match is found
    fin.close();
    return false;
}

// Loads the puzzle from a file into a 2D character array.
bool loadPuzzle(ifstream &fin, char board[], const int ROW, const int COL, char **puzzle)
{
    // Open the file
    fin.open(board);

    // Check if the file is successfully opened
    if (!fin.is_open())
    {
        // Return false if file opening fails
        return false;
    }

    // Temporary variable to store characters read from the file
    char a;

    // Loop through each row and column to populate the puzzle array
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            // Read a character from the file
            fin >> a;

            // Check if the character is not a newline character
            if (a != '\n')
            {
                // Populate the puzzle array with the read character
                puzzle[i][j] = a;
            }
        }
    }

    // Close the file after reading
    fin.close();

    // Return true to indicate successful loading of the puzzle
    return true;
}

// Function to compare two character arrays
bool charCmp(char arr1[], char arr2[])
{
    // Get the length of the first array
    int size1 = charLen(arr1);

    // Get the length of the second array
    int size2 = charLen(arr2);

    // If the lengths are not equal, return false
    if (size1 != size2)
    {
        return false;
    }

    // Loop through each character in the arrays
    for (int i = 0; i < size1; i++)
    {
        // If the characters at the same position in both arrays are not equal, return false
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }

    // If all characters are equal, return true
    return true;
}

// Function to convert a character array to upper case
void toUpper(char arr[])
{
    // Loop through each character in the array until the null character is found
    for (int i = 0; arr[i] != '\0'; i++)
    {
        // If the character is a lower case letter
        if (arr[i] >= 'a' && arr[i] <= 'z')
        {
            // Convert it to upper case by subtracting 32
            arr[i] -= 32;
        }
    }
}

// Function to convert a character array to Lower case
void toLower(char arr[])
{
    // Loop through each character in the array until the null character is found
    for (int i = 0; arr[i] != '\0'; i++)
    {
        // If the character is a Upper case letter
        if (arr[i] >= 'A' && arr[i] <= 'Z')
        {
            // Convert it to Lower case by subtracting 32
            arr[i] += 32;
        }
    }
}

// Function to calculate the length of a character array
int charLen(char arr[])
{
    // Initialize a counter variable
    int i = 0;

    // Loop through the array until the null character is found
    while (arr[i] != '\0')
    {
        // Increment the counter
        i++;
    }

    // Return the length of the character array
    return i;
}

// Function to resume a saved game
bool resumeGame(ifstream &fin, char pause[], int &lives, int &score, const int ROW, const int COL, char **puzzle)
{
    // Open the pause file
    fin.open(pause);
    if (!fin.is_open())
    {
        return false;
    }

    // Read the game state from the file
    fin >> lives >> score;
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            fin >> puzzle[i][j];
        }
    }

    // Close the file
    fin.close();

    return true;
}

// Function to display highscores
bool displayHighscore(ifstream &fin, char highscore[])
{
    // Open the highscore file
    fin.open(highscore);
    if (!fin.is_open())
    {
        return false;
    }

    cout << "Top 5 Highscores:\n";
    char Name[15];
    int highScore = 0;

    // Display the top 5 high scores
    while (fin >> Name >> highScore)
    {
        cout << Name << "\t\t" << highScore << endl;
    }

    // Close the file
    fin.close();

    return true;
}

// Function to run the game
void Game(int &lives, int &score, const int ROW, const int COL, char **puzzle, char word[], ofstream &fout, char pause[], ifstream &fin, char dictionary[], char level)
{
    // Display the difficulty level
    cout << "Your difficulty level: ";
    if (level == 'e' || level == 'E')
    {
        cout << "Easy\n";
    }
    else if (level == 'i' || level == 'I')
    {
        cout << "Intermediate\n";
    }
    else
    {
        cout << "Hard\n";
    }

    // Start the game
    cout << "Starting game...\n\n";

    // If the player has no lives, reset the lives and score
    if (lives == 0)
    {
        lives = 3;
        score = 0;
    }

    // Run the game while the player has lives
    while (lives != 0)
    {
        // Display the puzzle
        for (int i = 0; i < ROW; i++)
        {
            cout << "\t";
            for (int j = 0; j < COL; j++)
            {
                cout << puzzle[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;

        // Prompt the player to enter a word or pause the game
        cout << "\nEnter a word to search in the puzzle or \'p\' to pause game: ";
        cin.getline(word, ROW);

        // If the player wants to pause the game
        if (strcmp(word, "p") == 0)
        {
            // Save the current state of the game
            cout << "\nCurrent state of game is being stored\n";
            fout.open(pause);
            if (!fout.is_open())
            {
                cout << "There is an error saving the game\n Sorry for inconvenience\nExitting\n";
                exit(2);
            }

            // Write the lives and score to the file
            fout << lives << " " << score << endl;

            // Write the puzzle to the file
            for (int i = 0; i < ROW; i++)
            {
                for (int j = 0; j < COL; j++)
                {
                    fout << puzzle[i][j];
                }
                fout << endl;
            }

            // Close the file and pause the game
            fout.close();
            cout << "\t\t\nThe game has been PAUSED successfuly\n";
            break;
        }
        else if (charLen(word) >= 3)
        {
            // If the word does not match any word in the dictionary
            if (dictionaryMatch(fin, dictionary, word) == false)
            {
                // The player loses a life
                cout << "The word you entered is does not match\n!!You lost a life!!\n";
                lives--;
                cout << "Remaining lives: " << lives << "\nYour Score: " << score << endl;
            }
            else
            {
                // Convert the word to upper case
                toUpper(word);

                // Update the score and lives based on the level and the word
                scoreLifeUpdate(level, puzzle, word, ROW, COL, score, lives);
            }
        }
        else
        {
            cout << "!!THE WORD SHOULD HAVE AT LEAST 3 CHARACTERS!\n";
        }
    }
}

// Function to update score and lives based on the entered word
void scoreLifeUpdate(char level, char **puzzle, char word[], const int ROW, const int COL, int &score, int &lives)
{
    // Check if the entered word matches in the puzzle based on the level
    switch (level)
    {
    case 'e':
    case 'E':
    {
        if (easyLevel(puzzle, word, ROW, COL))
        {
            cout << "Word is found\n";
            score += 10;
            cout << "Your Score: " << score << endl;
        }
        else
        {
            cout << "The word you entered is does not match\n!!You lost a life!!\n";
            lives--;
            cout << "Remaining lives: " << lives << "\nYour Score: " << score << endl;
        }
        break;
    }
    case 'i':
    case 'I':
    {
        if (interLevel(puzzle, word, ROW, COL))
        {
            cout << "Word is found\n";
            score += 10;
            cout << "Your Score: " << score << endl;
        }
        else
        {
            cout << "The word you entered is does not match\n!!You lost a life!!\n";
            lives--;
            cout << "Remaining lives: " << lives << "\nYour Score: " << score << endl;
        }
        break;
    }
    case 'h':
    case 'H':
    {
        if (hardLevel(puzzle, word, ROW, COL))
        {
            cout << "Word is found\n";
            score += 10;
            cout << "Your Score: " << score << endl;
        }
        else
        {
            cout << "The word you entered is does not match\n!!You lost a life!!\n";
            lives--;
            cout << "Remaining lives: " << lives << "\nYour Score: " << score << endl;
        }
        break;
    }
    default:
    {
        cout << "Invalid Level!!\n";
        break;
    }
    }
}

// Checks for matches only in forward row and column
bool easyLevel(char **puzzle, char word[], int row, int col)
{
    // Size of the word
    int size = charLen(word);

    // Starting and ending positions of the matched word
    int startRow = 0;
    int endRow = 0;
    int startCol = 0;
    int endCol = 0;

    // Flag to indicate a match
    bool match = false;

    // Loop through each cell in the puzzle
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            // Check if the first character of the word matches the current cell
            if (word[0] == puzzle[i][j])
            {
                // Check right
                if (j + size <= 15)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i][j + k] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i;
                        startCol = j;
                        endCol = j + size - 1;
                        break;
                    }
                }
                // Check down
                if (i + size <= 15)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i + k][j] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i + size - 1;
                        startCol = j;
                        endCol = j;
                        break;
                    }
                }
            }
        }

        // Process the match
        if (match)
        {
            // Update puzzle based on matched word
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < col; j++)
                {
                    if (i >= startRow && i <= endRow)
                    {
                        if (j >= startCol && j <= endCol)
                        {
                            // Mark the matched characters with '-'
                            puzzle[i][j] = '-';
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

// check for matches in in forward and backward direction as well as single row and single column
bool interLevel(char **puzzle, char word[], int row, int col)
{
    // Get the length of the word
    int size = charLen(word);

    // Initialize the start and end positions for the row and column
    int startRow = 0;
    int endRow = 0;
    int startCol = 0;
    int endCol = 0;

    // Initialize match to false
    bool match = false;

    // Loop through each cell in the puzzle
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            // If the first character of the word matches the current cell
            if (word[0] == puzzle[i][j])
            {
                // Check right
                if (j + size <= col)
                {
                    int k = 0;
                    // Loop through the word size
                    for (k = 0; k < size; k++)
                    {
                        // If the characters do not match, break the loop
                        if (puzzle[i][j + k] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, set match to true and update the start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i;
                        startCol = j;
                        endCol = j + size - 1;
                        break;
                    }
                }
                // Similar checks for down, left, and up directions
                // Check down
                if (i + size <= row)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        if (puzzle[i + k][j] != word[k])
                        {
                            break;
                        }
                    }
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i + size - 1;
                        startCol = j;
                        endCol = j;
                        break;
                    }
                }
                // Check left
                if (j - size >= -1)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        if (puzzle[i][j - k] != word[k])
                        {
                            break;
                        }
                    }
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i;
                        startCol = j;
                        endCol = j - size + 2;
                        break;
                    }
                }
                // Check up
                if (i - size >= -1)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        if (puzzle[i - k][j] != word[k])
                        {
                            break;
                        }
                    }
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i - size + 2;
                        startCol = j;
                        endCol = j;
                        break;
                    }
                }
            }
        }
        // If a match is found, replace the matched word in the puzzle with '-'
        if (match)
        {
            for (int i = 0; i < row; i++)
            {
                for (int j = 0; j < col; j++)
                {
                    // If the current cell is within the start and end positions
                    if (i >= startRow && i <= endRow)
                    {
                        if (j >= startCol && j <= endCol)
                        {
                            // Replace the cell with '-'
                            puzzle[i][j] = '-';
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            // Return true if a match is found
            return true;
        }
    }
    // Return false if no match is found
    return false;
}

// checks for matches in forward, backward directions and in diagonals.
bool hardLevel(char **puzzle, char word[], int row, int col)
{
    // Size of the word
    int size = charLen(word);

    // Starting and ending positions of the matched word
    int startRow = 0;
    int endRow = 0;
    int startCol = 0;
    int endCol = 0;

    // Flag to indicate a match
    bool match = false;

    // Flags to indicate diagonal directions
    bool diagonalDown = true, diagonalUp = true;

    // Loop through each cell in the puzzle
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            // Check if the first character of the word matches the current cell
            if (word[0] == puzzle[i][j])
            {
                // Check left
                if (j - size >= -1)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i][j - k] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i;
                        startCol = j;
                        endCol = j - size + 2;
                        break;
                    }
                }
                // Check up
                if (i - size >= -1)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i - k][j] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i - size + 2;
                        startCol = j;
                        endCol = j;
                        break;
                    }
                }
                // Check diagonal down-right
                diagonalDown = false;
                if (i + size <= 15 && j + size <= 15)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i + k][j + k] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i + size;
                        startCol = j;
                        endCol = j + size;
                        diagonalDown = true;
                        break;
                    }
                }
                // Check diagonal up-left
                diagonalUp = false;
                if (i - size >= -1 && j - size >= -1)
                {
                    int k = 0;
                    for (k = 0; k < size; k++)
                    {
                        // Check if the characters match
                        if (puzzle[i - k][j - k] != word[k])
                        {
                            break;
                        }
                    }
                    // If all characters match, update start and end positions
                    if (k == size)
                    {
                        match = true;
                        startRow = i;
                        endRow = i - size + 2;
                        startCol = j;
                        endCol = j - size + 2;
                        diagonalUp = true;
                        break;
                    }
                }
            }
        }

        // Process the match
        if (match)
        {
            // Update puzzle based on matched word

            // If the match is diagonal down
            if (diagonalDown)
            {
                for (int i = 0; i < size; i++)
                {
                    puzzle[startRow + i][startCol + i] = '-';
                }
            }
            // If the match is diagonal up
            else if (diagonalUp)
            {
                for (int i = 0; i < size; i++)
                {
                    puzzle[startRow - i][startCol - i] = '-';
                }
            }
            // If the match is in left, up, down, or right direction
            else
            {
                for (int i = 0; i < row; i++)
                {
                    for (int j = 0; j < col; j++)
                    {
                        if (i >= startRow && i <= endRow)
                        {
                            if (j >= startCol && j <= endCol)
                            {
                                // Mark the matched characters with '-'
                                puzzle[i][j] = '-';
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else
                        {
                            continue;
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}
