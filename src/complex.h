#ifndef COMPLEX_H
#define COMPLEX_H

// haydn.content
// MAT320
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <vector>

using std::cos;
using std::sin;
using std::string;
using std::ostream;
using std::vector;

namespace IMath
{
using ComplexVector = vector<Complex>;

const double TWOPI = 8.f * std::atan(1.f);

struct Complex
{
    double real, imag;
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}
};

Complex operator-(Complex a) { return Complex(-a.real,-a.imag); }

Complex operator+(Complex a, Complex b) { return Complex(a.real + b.real, a.imag + b.imag); }
Complex operator-(Complex a, Complex b) { return Complex(a.real - b.real, a.imag - b.imag); }
Complex operator*(Complex a, Complex b) { return Complex(a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real); }
Complex operator*(double m,Complex a) { return Complex(m*a.real,m*a.imag); }
Complex operator*(Complex a,double m) { return Complex(m*a.real,m*a.imag); }

Complex conjugate(Complex a) { return Complex(a.real, -a.imag); }
Complex polar(double theta) { return Complex(cos(theta), sin(theta)); }
Complex rotate_twopi_x(const Complex &a, double x) { return a * polar(x * TWOPI); }
Complex parse_complex(const string &line)
{
    size_t op_in = line.find_first_of("+-", 1);
    double r,i=0.0;

    if (op_in == string::npos)
    {
        string term = line;
        if (term.back() == 'i')
            i = std::stod(term.substr(0, term.size()-1));
        else
            r = std::stod(term);
    }
    else
    {
        string first = line.substr(0, op_in);
        char op = line[op_in];
        string second = line.substr(op_in+1);

        double op_sign = op == '-' ? -1.0 : 1.0;

        if (first.back() == 'i')
            i = std::stod(first.substr(0,first.size()-1));
        else 
            r = std::stod(first);

        if (second.back() == 'i')
            i = op_sign * std::stod(second.substr(0,second.size()-1));
        else
            r = op_sign * std::stod(second);
    }
        
    return Complex(r, i);
}
void print_complex(const Complex &a)
{
    char op = a.imag < 0.0 ? '-' : '+';
    if (a.real != 0.0) std::cout << a.real;
    if (a.imag != 0.0) std::cout << op << std::abs(a.imag) << 'i';
    std::cout<<std::endl;
}
void out_complex(const Complex &a, ostream & o)
{
    char op = a.imag < 0.0 ? '-' : '+';
    if (a.real != 0.0) o << a.real;
    if (a.imag != 0.0) o << op << std::abs(a.imag) << 'i';
    o<<std::endl;
}
Complex powers(int N, int k)
{
    if (k == 0) { return Complex(0, 0); }

    double theta = TWOPI / N;
    Complex sum(1, 0);
    for (int i = 1; i < k; ++i) { sum = sum + polar(theta * i); }

    return sum;
}
vector<Complex> powers_vec(int N, int k)
{
    if (k == 0) { return std::vector<Complex>(); }

    double theta = TWOPI / N;
    std::vector<Complex> result; 
    result.push_back(Complex(1,0));
    for (int i = 1; i < k; ++i) { result.push_back( polar(theta * i) ); }

    return result;
}
Complex inner_product(const std::vector<Complex> &a, const std::vector<Complex> &b, int N)
{
    Complex total;
    for (int i = 0; i < N; ++i)
        total = total + (a[i] * conjugate(b[i]));
    return total;
}

} // namespace

#endif