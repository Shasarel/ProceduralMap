#include "SimplexNoise.h"


int SimplexNoise::fastFloor(double x) {
	return x>0 ? (int)x : (int)x - 1;
}
double SimplexNoise::dot(const int g[], double x, double y) {
	return g[0] * x + g[1] * y;
}

double SimplexNoise::noise(double xin, double yin) {
	double n0, n1, n2; 
					 
	double F2 = 0.5 * (sqrt3 - 1.0);
	double s = (xin + yin) * F2;
	int i = fastFloor(xin + s);
	int j = fastFloor(yin + s);
	double G2 = (3.0 - sqrt3) / 6.0;
	double t = (i + j) * G2;
	double X0 = i - t; 
	double Y0 = j - t;
	double x0 = xin - X0;
	double y0 = yin - Y0;

	int i1, j1; 
	if (x0 > y0) 
	{ 
		i1 = 1;
		j1 = 0; 
	} 
	else 
	{ 
		i1 = 0; 
		j1 = 1;
	} 

	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2;
	double y2 = y0 - 1.0 + 2.0 * G2;

	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	double t0 = 0.5 - x0*x0 - y0*y0;
	if (t0<0) 
		n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); 
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1<0) 
		n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	double t2 = 0.5 - x2*x2 - y2*y2;
	if (t2<0)
		n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	return 70.0 * (n0 + n1 + n2);
}

SimplexNoise::SimplexNoise(int quality, int octaves, double persistence)
{
	for (int i = 0; i<512; i++)
		perm[i] = p[i & 255];
	this->quality = quality;
	this->octaves = octaves;
	this->persistence = persistence;
}

double SimplexNoise::simplexNoise(double x, double y)
{
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double maxValue = 0;
	double step = 1.0 / quality;
	for (int i = 0; i<octaves; i++) {
		total += noise(x  * step * frequency, y * step * frequency) * amplitude;

		maxValue += amplitude;

		amplitude *= persistence;
		frequency *= 2;
	}

	return ((total / (maxValue)) + 1) / 2;
}
