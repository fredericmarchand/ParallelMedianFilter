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

void medianFilter(int x1, int y1, int x2, int y2)
{
    if ((x1 == x2-1) && (y1 == y2-1))
    {
        vector<int> values;
        int tempx, tempy;
        /// Create list of values based on k
        for (int i = 0; i < 8; ++i)
        {
            switch (i)
            {
                case 0:
                    for (int j = 1; j <= k; ++j)
                    {
                        x1-j < 0 ? tempx = 0 : tempx = x1-j;
                        values.push_back(inputArray[tempx][y1]);
                    }
                    break;
                case 1:
                    for (int j = 1; j <= k; ++j)
                    {
                        y1-j < 0 ? tempy = 0 : tempy = y1-j;
                        values.push_back(inputArray[x1][tempy]);
                    }
                    break;
                case 2:
                    for (int j = 1; j <= k; ++j)
                    {
                        x1-j < 0 ? tempx = 0 : tempx = x1-j;
                        y1-j < 0 ? tempy = 0 : tempy = y1-j;
                        values.push_back(inputArray[tempx][tempy]);
                    }
                    break;
                case 3:
                    for (int j = 1; j <= k; ++j)
                    {
                        x1+j > m-1 ? tempx = m-1 : tempx = x1+j;
                        values.push_back(inputArray[tempx][y1]);
                    }
                    break;
                case 4:
                    for (int j = 1; j <= k; ++j)
                    {
                        y1+j > n-1 ? tempy = n-1 : tempy = y1+j;
                        values.push_back(inputArray[x1][tempy]);
                    }
                    break;
                case 5:
                    for (int j = 1; j <= k; ++j)
                    {
                        y1+j > n-1 ? tempy = n-1 : tempy = y1+j;
                        x1+j > m-1 ? tempx = m-1 : tempx = x1+j;
                        values.push_back(inputArray[tempx][tempy]);
                    }
                    break;
                case 6:
                    for (int j = 1; j <= k; ++j)
                    {
                        x1+j > m-1 ? tempx = m-1 : tempx = x1+j;
                        y1-j < 0 ? tempy = 0 : tempy = y1-j;
                        values.push_back(inputArray[tempx][tempy]);
                    }
                    break;
                case 7:
                    for (int j = 1; j <= k; ++j)
                    {
                        y1+j > n-1 ? tempy = n-1 : tempy = y1+j;
                        x1-j < 0 ? tempx = 0 : tempx = x1-j;
                        values.push_back(inputArray[tempx][tempy]);
                    }
                    break;
            }
        }

        /// sort it
        sort(values.begin(), values.end(), integerSort);
        /// get median and write it in the output inputArray
        outputArray[y1][x1] = (values.at(k*8/2) + values.at(k*8/2-1)) / 2;
        for (vector<int>::const_iterator i = values.begin(); i != values.end(); ++i)
        {
            cout << *i << endl;
        }
    }
    else
    {
#if DEBUG == 0
//        cout << "(" << x1 << "," << y1 << ") (" << x2 << "," << y2 << ")\n";
#endif
        cilk_spawn medianFilter(x1, y1, x2-(x2-x1)/2, y2-(y2-y1)/2); 
//        cilk_spawn medianFilter(x1+(x2-x1)/2, y1, x2, y2-(y2-y1)/2);
//        cilk_spawn medianFilter(x1, y1+(y2-y1)/2, x2-(x2-x1)/2, y2);
//        cilk_spawn medianFilter(x1+(x2-x1)/2, y1+(y2-y1)/2, x2, y2);
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
    
#if DEBUG == 1
    printArray();
#endif

    /// Run the program
    time1 = __cilkview_getticks();

    medianFilter(0, 0, n, m);

    time2 = __cilkview_getticks();

    par_time = time2-time1;
    cout << "\nMedian Filter took " << par_time << " milliseconds." << endl;

    /// Write to output file

    ofstream outputFile ("temp");
    if (!outputFile.is_open())
    {
        cerr << "Failed to open output file" << endl;
        return 1;   /// Error
    }
   
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            outputFile << outputArray[i][j] << " ";
        }
        outputFile << endl;
    }

    outputFile.close();

    printArray();

    /// De-allocate the two dimensional arrays
    for (int i = 0; i < m; ++i)
    {
        delete [] inputArray[i];
        delete [] outputArray[i];
    }
    delete [] inputArray;
    delete [] outputArray;

    return 0;
}


