// Haydn Content
// MAT320
// Fall 2026
// Project 4 - lowpass.cpp

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

// y_t = x_t + a*x_{t-1}
class lowpass
{ 
public:
    lowpass(float a_) : a(a_), x1(0.f) {}
    float operator()(float x) 
    { 
        double y=x+(a*x1);
        x1=x;
        return float(y); 
    }
private:
    float a;
    float x1;
};

void LowPassFilter(vector<float> & signal, float coef)
{
    lowpass lpf(coef);
    for(unsigned i=0; i<signal.size(); ++i)
        signal[i]=lpf(signal[i]);
}

void Normalize(vector<float> & signal)
{
    float max=0.f;
    for(float s : signal)
        if (fabs(s)>max) max=fabs(s);
    if (max==0.f) return;
    float factor=1.f/max;
    for(float& s : signal) s*=factor;
}

// args:
// coeffcient       a1
// times to run     n
// audio file       input.wav
int main(int argc, char* argv[])
{
    if (argc!=4) {cerr<<"incorrect number of args, expecting ./lowpass <a1> <n> <input.wav>"<<endl; return -1;}

    float filter_coef = atof(argv[1]);
    int N = atoi(argv[2]);
    string in_filename(argv[3]);

    fstream in(in_filename,ios_base::binary|ios_base::in);
    if (!in) {cerr<<"bad file"<<endl; return -1;}

    char header[44];
    in.read(header,44);

    unsigned size,rate;
    memcpy(&size,header+40,4);
    memcpy(&rate,header+24,4);
    unsigned long int sampleCount = size / sizeof(short);
    vector<short> rawData(sampleCount);
    in.read(reinterpret_cast<char*>(rawData.data()),size);

    cout << setw(25) << "The sample rate is: " << setw(10) << rate << endl;
    cout << setw(25) << "The data size is: "   << setw(10) << size << endl;

    // convert to floating point data
    vector<float> fdata(sampleCount);
    for(unsigned long int i=0l; i<sampleCount; ++i)
	    fdata[i] = static_cast<float>(rawData[i]) / float(1<<15);

    // apply lowpass filter n times
    for(int i=0; i<N; ++i)
        LowPassFilter(fdata,filter_coef);

    Normalize(fdata);

    // write output wav file
    vector<short> samples(sampleCount);
    for (unsigned long int i=0l; i<sampleCount; ++i) 
        samples[i]=static_cast<short>(fdata[i]*float((1<<15)-1));

    fstream out("output.wav",ios_base::binary|ios_base::out);
    out.write(header,44);
    out.write(reinterpret_cast<char*>(samples.data()),size);

    return 0;
}