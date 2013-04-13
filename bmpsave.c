/* 数理プログラミング bmpsave.c */
/* bmpとしてデータ列を保存 */
/* 数理情報学演習II Mandelbrot.c (2012.01.16) からの抜き出し */
/* 2013.01.31, 10RU004, 石橋祥太 */

#include "bmpsave.h"

#define ERROR_FORMAT	1
#define ERROR_FILEOPEN	2

char bmpsave(const char *filename, const void *pixmaps, unsigned long XSIZE, unsigned long YSIZE, unsigned char ZSIZE, unsigned char mode)
{
	const unsigned char *maps = pixmaps;
	unsigned long BMPHEADER[10], count, countX, countY;
	FILE *fp;

	if (mode & 1)
	{	/* BITMAP 出力処理 */
		/* ファイル書き込み開始 */
		if ((fp = fopen(filename, "wb")) == NULL) return ERROR_FILEOPEN;

		/* BMPFILEHEADER */
		fprintf(fp, "BM");	/* bfType */
		BMPHEADER[0] = 54 + XSIZE * YSIZE * ZSIZE;	/* bfSize */
		BMPHEADER[1] = 0;	/* bfReserved1 + bfReserved2 */
		BMPHEADER[2] = 54;	/* bfOffBits */
		if (ZSIZE == 1) BMPHEADER[2] += 1024;
		fwrite(BMPHEADER, sizeof(unsigned long), 3, fp);

		/* BMPINFOHEADER */
		BMPHEADER[0] = 40;	/* biSize */
		BMPHEADER[1] = XSIZE;	/* biWidth */
		BMPHEADER[2] = YSIZE;	/* biHeight */
		BMPHEADER[3] = 1 | ZSIZE * 8 << 16;	/* biPlanes + biBitCount */
		BMPHEADER[4] = 0;	/* biCompression */
		BMPHEADER[5] = XSIZE * YSIZE * ZSIZE;	/* biSizeImage */
		BMPHEADER[6] = 0;	/* biXPelsPerMeter */
		BMPHEADER[7] = 0;	/* biYPelsPerMeter */
		BMPHEADER[8] = 0;	/* biClrUsed */
		BMPHEADER[9] = 0;	/* biClrImportant */
		if (ZSIZE == 1) BMPHEADER[8] = 256;
		if (ZSIZE == 2) BMPHEADER[8] = 6;
		fwrite(BMPHEADER, sizeof(unsigned long), 10, fp);

		/* TABLE */
		if (ZSIZE == 1)
			for (count = 0; count < 256; count++)
				fprintf(fp, "%c%c%c%c", count, count, count, 0);

		/* DATA */
		switch (ZSIZE)
		{
		case 1:
			for (countY = YSIZE; countY > 0; countY--)	/* 左下から右上へ */
			{
				fwrite(&maps[XSIZE * (countY - 1) * ZSIZE], ZSIZE, XSIZE, fp);

				/* 1ラインは必ず4バイトの倍数にする */
				for (count = 0; count < (4 - XSIZE % 4) % 4; count++)
					fprintf(fp, "%c", 0);
			}
			break;

		case 2:
			break;

		case 3:
		case 4:
			for (countY = YSIZE; countY > 0; countY--)	/* 左下から右上へ */
			{
				for (countX = XSIZE * (countY - 1); countX < XSIZE * countY; countX++)
					for (count = ZSIZE; count > 0; count--)
						fprintf(fp, "%c", maps[countX * ZSIZE + count - 1]);

				if (ZSIZE == 3)	/* 1ラインは必ず4バイトの倍数にする */
					for (count = 0; count < (4 - XSIZE * 3 % 4) % 4; count++)
						fprintf(fp, "%c", 0);
			}
			break;
		}
	}
	else
	{	/* P5-P6 出力処理 */
		/* ファイル書き込み開始 */
		if ((fp = fopen(filename, "wb")) == NULL) return ERROR_FILEOPEN;
		fprintf(fp, "P%d\n# 10RU004 石橋祥太\n%lu %lu\n255\n", 5 + (ZSIZE > 1), XSIZE, YSIZE);

		switch (ZSIZE)
		{
		case 1:
		case 3:
			fwrite(maps, sizeof(unsigned char), XSIZE * YSIZE * ZSIZE, fp);
			break;

		case 2:
			break;

		case 4:
			for (count = 0; count < XSIZE * YSIZE; count++)
				fwrite(&maps[count * 4 + 1], sizeof(unsigned char), 3, fp);
			break;
		}
	}
	fclose(fp);	/* ファイル書き込み終了 */
	return 0;
}