#include "math.h"

int abs(int x) {

	if (x < 0)
		x = -x;

	return x;
}

/* Pre: b > 0 */
int mod(int a, int b) {
	return ((a%b) + b) % b;
}