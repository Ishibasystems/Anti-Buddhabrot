/* �����v���O���~���O Buddhabrot_init.c */
/* �u�b�_�u�� BA-DA-DHA �}���`�R�A�p �f�[�^�t�@�C�������� */
/* 2013.01.30, 10RU004, �΋��ˑ� */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

char Buddhabrot(const char *filename, unsigned char mode, unsigned long TMAX,
	double XMIN, double XMAX, double YMIN, double YMAX, unsigned long XSIZE, unsigned long YSIZE)
{
	unsigned char buf[] = "init.bin", core = 8;
	unsigned long count, *Buddhamaps;
	double *mx, *my;
	FILE *data;

	printf("Initialization Start.\n");

	/* �g�p���铮�I���������m�� */
	if ((mx = malloc(XSIZE * sizeof(double))) == NULL) return 1;
	if ((my = malloc(YSIZE * sizeof(double))) == NULL) return 1;

	for (count = 0; count < XSIZE; mx[count++] = (XMAX - XMIN) * count / XSIZE + XMIN);
	for (count = 0; count < YSIZE; my[count++] = (YMAX - YMIN) * count / YSIZE + YMIN);

	core |= core << 4;

	printf("CREATE: %s\n", buf);
	if ((data = fopen(buf, "wb")) == NULL) return 1;
	fwrite(&core, sizeof(unsigned char), 1, data);
	fwrite(&TMAX, sizeof(unsigned long), 1, data);
	fwrite(&XMIN, sizeof(double), 1, data);
	fwrite(&XMAX, sizeof(double), 1, data);
	fwrite(&YMIN, sizeof(double), 1, data);
	fwrite(&YMAX, sizeof(double), 1, data);
	fwrite(&XSIZE, sizeof(unsigned long), 1, data);
	fwrite(&YSIZE, sizeof(unsigned long), 1, data);
	fwrite(mx, sizeof(double), XSIZE, data);
	fwrite(my, sizeof(double), YSIZE, data);
	fclose(data);

	free(mx);
	free(my);

	printf("Initialization Finish.\n");
	return 0;
}