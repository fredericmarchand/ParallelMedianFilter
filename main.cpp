/*
 * MedianFilter.cilk
 *
 */


#include <cilk/cilk.h>
#include <cilktools/cilkview.h>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

#define DEBUG 1 

int m, n, k;
int **array;

void stringToCharArray(string s, char *a)
{
    a[s.size()] = 0;
    memcpy(a, s.c_str(), s.size());
}


void printArray()
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            cout << array[i][j] << " ";
        }
        cout << endl;
    }
}


int main(int argc, char** argv) 
{
    char * filepath;
    int time1;
    int time2;
    int par_time;
    string buffer;
    char *pch;
    const char * delim = " ";
    int x = 0;
    int y = 0;

    if (argc <= 1) 
    {
        cerr << "The program is missing an argument" << endl; 
        return 1;   /// Error
    }

    filepath = argv[1]; 
    
    ifstream input;
    input.open(filepath);

    if (!input.is_open())
    {
        cerr << "Could not open file " << filepath << endl;
        return 1;
    }

    getline(input, buffer);
    m = atoi(buffer.c_str());

    getline(input, buffer);
    n = atoi(buffer.c_str());

    getline(input, buffer);
    k = atoi(buffer.c_str());

    cout << "m: " << m << "\nn: " << n << "\nk: " << k << endl;

    array = new int*[m];
    for (int i = 0; i < m; ++i)
    {
        array[i] = new int[n];
    }

    while (getline(input, buffer))
    {
        y = 0;
        char line[buffer.size()+1];
        stringToCharArray(buffer, line); 
        pch = strtok (line, delim);
        while (pch != NULL)
        {
            array[x][y++] = atoi(pch);
            pch = strtok (NULL, delim);
        }
        x++;
    }

    input.close();
    
#if DEBUG == 1
    printArray();
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nMedian Filter took " << par_time << " milliseconds." << endl;

    /// De-allocate the two dimensional array
    for (int i = 0; i < m; ++i)
    {
        delete [] array[i];
    }
    delete [] array;

    return 0;
}


