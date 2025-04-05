#include <stdio.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_ITER 1000

int mandelbrot(double real, double imag) {
    int n;
    double r = real, i = imag;
    for (n = 0; n < MAX_ITER; n++) {
        double r2 = r * r - i * i + real;
        double i2 = 2 * r * i + imag;
        r = r2;
        i = i2;
        if (r * r + i * i > 4) return n;
    }
    return MAX_ITER;
}

void draw_mandelbrot() {
    double min_real = -2.0, max_real = 1.0;
    double min_imag = -1.0, max_imag = 1.0;
    double real, imag;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            real = min_real + (x / (double)WIDTH) * (max_real - min_real);
            imag = min_imag + (y / (double)HEIGHT) * (max_imag - min_imag);
            int m = mandelbrot(real, imag);
            char c = m == MAX_ITER ? ' ' : ".,-~:;=!*#$@"[m % 12];
            putchar(c);
        }
        putchar('\n');
    }
}

int main() {
    draw_mandelbrot();
    return 0;
}
