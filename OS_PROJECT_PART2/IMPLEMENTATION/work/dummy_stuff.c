///////////////////////////////////
// Manolatos Spyridon, 1104802
// Maroudas Andreas-Erikos, 1100617
// Kirkinis Manousos, 1100590
// Chrysanthakopoulos Nikolaos, 1100760
////////////////////////////////////
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

double a = 1.1;

void core_delay()
{
	unsigned long j;

	for (j = 0; j < 100000; j++) {
		a += sqrt(1.1)*sqrt(1.2)*sqrt(1.3)*sqrt(1.4)*sqrt(1.5);
		a += sqrt(1.6)*sqrt(1.7)*sqrt(1.8)*sqrt(1.9)*sqrt(2.0);
		a += sqrt(1.1)*sqrt(1.2)*sqrt(1.3)*sqrt(1.4)*sqrt(1.5);
		a += sqrt(1.6)*sqrt(1.7)*sqrt(1.8)*sqrt(1.9);
	}
}



int main(int argc, char **argv)
{
	int pid = getpid();

	printf("process %d begins\n", pid);
	core_delay();
	printf("process %d ends\n", pid);

	return 0;
}
