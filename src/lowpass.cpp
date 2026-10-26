// Haydn Content
// MAT320
// Fall 2026
// Project 4 - lowpass.cpp

#include <iostream>
#include <fstream>
#include <string>

#include "complex.h"

using namespace std;
using namespace IMath;

void LowPassFilter(ComplexVector & signal)
{

}

// args:
// coeffcient       a1
// timest to run    n
// audio file       input.wav
int main(int argc, char* argv[])
{
    if (argc!=4) {cerr<<"incorrect number of args, expecting ./lowpass <a1> <n> <input.wav>"<<endl; return -1;}

    int filter_coef = atoi(argv[1]);
    int N = atoi(argv[2]);

    fstream in(argv[3],ios_base::binary|ios_base::in);
    if (!in) {cerr<<"bad file"<<endl; return -1;}

    char header[44];
    in.read(header,44);
    unsigned size = *reinterpret_cast<unsigned*>(header+40),
        rate = *reinterpret_cast<unsigned*>(header+24),
        count = size/2;
    char *data = new char[size];
    in.read(data,size);

    cout << "The sample rate is:  " << rate << endl;
    cout << "The data size is:  " << size << endl;

    // window/hop size
    const float PI = 4.0f*atan(1.0f),
              TWOPI = 2.0f*PI;
    const int N = 1 << 11,
            H = N >> 1;

    // read input wav file



    // apply lowpass filter n times



    // write output wav file

    
    return 0;
}