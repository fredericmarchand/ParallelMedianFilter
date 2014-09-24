/*
 * MedianFilter.cilk
 *
 */

#include <cilk/cilk.h>
#include <cilktools/cilkview.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string.h>
using namespace std;

#define DEBUG 0 

int m, n, k;
int **inputArray;
int **outputArray;

bool integerSort(int i, int j) 
{
    return i < j;
}

void findMedianFor(int x, int y, int k)
{
    vector<int> values;

    int x1;
    int y1;

    for (int i = x-k; i <= x+k ; ++i)
    {
        for (int j = y-k; j <= y+k; ++j)
        {
            x1 = i;
            y1 = j;
            if (i < 0) x1 = 0;
            if (i >= m) x1 = m - 1;
            if (j < 0) y1 = 0;
            if (j >= n) y1 = n - 1;
            values.push_back(inputArray[x1][y1]);
        }
    }

    /// sort it
    sort(values.begin(), values.end(), integerSort);
    outputArray[x][y] = values.at(values.size() / 2);

#if DEBUG == 1
    for (vector<int>::const_iterator i = values.begin(); i != values.end(); ++i)
    {
        cout << *i << endl;
    }
#endif

}

void medianFilter(int x1, int y1, int x2, int y2)
{
    if ((x2 - x1 == 1) && (y2 - y1 == 1))
    {
        if (outputArray[x1][y1] != 0)
            return;
        findMedianFor(x1, y1, k);
    }
    else
    {
        if (x2 - x1 == 1)
        {
            cilk_spawn medianFilter(x1, y1, x2, y2-(y2-y1)/2);
            cilk_spawn medianFilter(x1, y1+(y2-y1)/2, x2, y2); 
        }
        else if (y2 - y1 == 1)
        {
            cilk_spawn medianFilter(x1, y1, x2-(x2-x1)/2, y2);
            cilk_spawn medianFilter(x1+(x2-x1)/2, y1, x2, y2); 
        }
        else
        {
            cilk_spawn medianFilter(x1, y1, x2-(x2-x1)/2, y2-(y2-y1)/2); 
            cilk_spawn medianFilter(x1+(x2-x1)/2, y1, x2, y2-(y2-y1)/2);
            cilk_spawn medianFilter(x1, y1+(y2-y1)/2, x2-(x2-x1)/2, y2);
            cilk_spawn medianFilter(x1+(x2-x1)/2, y1+(y2-y1)/2, x2, y2);
        }
        cilk_sync;
    }
}

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
            cout << outputArray[i][j] << " ";
        }
        cout << endl;
    }
}

void freeMem()
{
    /// De-allocate the two dimensional arrays
    for (int i = 0; i < m; ++i)
    {
        delete [] inputArray[i];
        delete [] outputArray[i];
    }
    delete [] inputArray;
    delete [] outputArray;
}

int main(int argc, char** argv) 
{
    char * filepath;
    char * outputFilepath;
    int time1;
    int time2;
    int par_time;
    string buffer;
    char *pch;
    const char * delim = " ";
    int x = 0;
    int y = 0;

    if (argc <= 2) 
    {
        cerr << "The program is missing an argument" << endl; 
        return 1;   /// Error
    }

    filepath = argv[1]; 
    outputFilepath = argv[2];

    ifstream input;
    input.open(filepath);

    if (!input.is_open())
    {
        cerr << "Could not open file " << filepath << endl;
        return 1;   /// Error
    }

    getline(input, buffer);
    m = atoi(buffer.c_str());

    getline(input, buffer);
    n = atoi(buffer.c_str());

    getline(input, buffer);
    k = atoi(buffer.c_str());

    cout << "m: " << m << "\nn: " << n << "\nk: " << k << endl;

    inputArray = new int*[m];
    outputArray = new int*[m];
    for (int i = 0; i < m; ++i)
    {
        inputArray[i] = new int[n];
        outputArray[i] = new int[n];
    }

    while (getline(input, buffer))
    {
        y = 0;
        char line[buffer.size()+1];
        stringToCharArray(buffer, line); 
        pch = strtok (line, delim);
        while (pch != NULL)
        {
            inputArray[x][y++] = atoi(pch);
            pch = strtok (NULL, delim);
        }
        x++;
    }

    input.close();

    if (m < k || n < k)
    {
        cerr << "Value of k is too low, must be greater or equal to m and n" << endl;
        freeMem();
        return 1;
    }

#if DEBUG == 1
    printArray();
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    medianFilter(0, 0, m, n);

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nMedian Filter took " << par_time << " milliseconds." << endl;

    /// Write to output file

    ofstream outputFile (outputFilepath);
    if (!outputFile.is_open())
    {
        cerr << "Failed to open output file" << endl;
        return 1;   /// Error
    }
   
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outputFile << outputArray[i][j];
            if ((j + 1) != n)
            {
                outputFile << " ";
            }
        }
        outputFile << endl;
    }

    outputFile.close();

#if DEBUG == 1
    printArray();
#endif
    freeMem();

    return 0;
}


