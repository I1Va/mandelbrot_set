#include <math.h>
#include "TXLib.h"

const size_t SCREEN_WIDTH = 1000;
const size_t SCREEN_HEIGHT = 600;
const size_t MAX_ITERATIONS_CNT = 500;
const double STABLE_RADIUS = 16;
const int    COLOR_MAX_VAL = 255;

class Complex
{
private:
    double r, i;
public:
    Complex(double r, double i)
        { this->r = r; this->i = i; }
    Complex operator+(const Complex &c)
        { return Complex(r + c.r, i + c.i); }
    Complex operator*(const Complex &c)
        { return Complex(r * c.r - i * c.i, 2 * r * c.i); }
    double abs(){return r * r + i * i;}
};

void display() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            Complex P(0, 0);
            int iters = 0;
            while (iters < MAX_ITERATIONS_CNT && P.abs() < STABLE_RADIUS) {
                P = P * P + Complex((x - 600) / 180.0, (y - 400) / 180.0);
                iters++;
            }

            double red_coef     = 0.1 + iters * 0.03 * 0.2;
            double green_coef   = 0.2 + iters * 0.03 * 0.3;
            double blue_coef    = 0.3 + iters * 0.03 * 0.1;

            COLORREF color = RGB(red_coef * COLOR_MAX_VAL, green_coef * COLOR_MAX_VAL, blue_coef * COLOR_MAX_VAL);
            txSetPixel(x, y, color);
        }
    }
}

int main () {
    txCreateWindow (SCREEN_WIDTH, SCREEN_HEIGHT);
    for (;; ) {
        display();
    }
    return 0;
}
