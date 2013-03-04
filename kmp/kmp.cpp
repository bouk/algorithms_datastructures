#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<int> failure_function;
string pattern;

int match(string &line)
{
    size_t line_position = 0;
    size_t pattern_position = 0;
    while (line_position < line.size())
    {
        if (pattern[pattern_position] == line[line_position])
        {
            if (pattern_position == pattern.size() - 1)
            {
                // end of pattern reached, pattern found! Return start of pattern
                return line_position - pattern.size() + 1;
            }
            line_position++;
            pattern_position++;
        }
        else if (pattern_position > 0)
        {
            // No match, but maybe we can backtrack
            while(pattern_position > 0 and pattern[pattern_position] != line[line_position])
            {
                pattern_position = failure_function[pattern_position - 1];
            }
        }
        else
        {
            line_position++;
        }
    }

    return -1;
}

void create_failure_function()
{
    size_t i = 1;
    size_t j = 0;

    failure_function.assign(pattern.size(), -1);
    failure_function[0] = 0;

    while (i < pattern.size())
    {
        if (pattern[i] == pattern[j])
        {
            // we have matched j + 1 characters
            failure_function[i] = j + 1;
            i++;
            j++;
        }
        else if (j > 0)
        {
            // j indexes just after a prefix of pattern that must match
            j = failure_function[j - 1];
        }
        else
        {
            // we have no match here
            failure_function[i] = 0;
            i++;
        }
    }
}

int main(int argc, char const *argv[])
{
    string line;
    if (argc < 2)
    {
        cout << "Provide a string to search for as first argument" << endl;
        return 1;
    }
    pattern = string(argv[1]);
    create_failure_function();

    while (!cin.eof())
    {
        getline(cin, line);
        int pos = match(line);
        if(pos != -1)
        {
            cout << line << endl;
        }
    }

    return 0;
}