#include <stdlib.h>
#include <stdio.h>

int mainD(int argc, char *argv[])
{
	FILE *fp;
	if ((fp = fopen(*(++argv), "r")) == NULL)
	{
		printf("can't open %s\n", *argv);
		return 1;
	}

	char tempScan[1000];
	while (!feof(fp))	// while not end of file
	{
		// every time i do scanf, its like i just continue scan (no white chars)
		fscanf(fp, "%s", &(tempScan));
		printf("\n%s", tempScan);
	}
	scanf("%s");
}