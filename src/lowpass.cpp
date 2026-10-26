// Haydn Content
// MAT320
// Fall 2026
// Project 4 - lowpass.cpp

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// args:
// coeffcient       a1
// timest to run    n
// audio file       input.wav
int main(int argc, char* argv[])
{
    if (argc!=4) {cerr<<"incorrect number of args, expecting ./lowpass <a1> <n> <input.wav>"<<endl; return -1;}

    int filter_coef = atoi(argv[1]);
    int N = atoi(argv[2]);
    string in_string(argv[3]);

    ifstream in(in_string);
    if (!in.is_open()) {cerr<<"bad file, stream failed to open"<<" '"<<in_string<<"'."<<endl; return -1;}

    // read input wav file



    // apply lowpass filter n times



    // write output wav file

    return 0;
}