/* 数理プログラミング Buddhabrot_bmp.c */
/* ブッダブロ BA-DA-DHA マルチコア用 描画部 */
/* 2013.03.02, 10RU004, 石橋祥太 */

#include "bmpsave.h"
#include <time.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795L	/* VC++6.0に定義されてなかった */
#endif

void main(void)
{	/*色量モード選択\n1:\t変化量\n2:\t標本分散\n3:\t標準偏差\n4:\t発散に要した回数の逆数*/
	char buf[256], filename[256];
	unsigned char *maps, core, count;
	unsigned long mapx, mapy, *Auddhamaps, *Buddhamaps, *Cuddhamaps, XSIZE, YSIZE;
	double hoge[4];
	double inf;
	FILE *data;

	time_t timer;

	buf[count = 0] = '\0';

	time(&timer);
	strcpy(buf, ctime(&timer));

	for(count = 0; count < strlen(buf); count++)
	{	/* ctime関数で返す文字列中のファイル名に指定できない文字を置換 */
		if (buf[count] == ':') buf[count] = '_';
		if (buf[count] == '\n') buf[count] = '\0';
	}
	strcpy(filename, buf);

	printf("\n出力ファイル名 : \"%s\"\n出力中... しばらくお待ちください\n", filename);

	/* 計算情報を読み込み */
	if ((data = fopen("init.bin", "rb")) == NULL) return;
	fread(&core, sizeof(unsigned char), 1, data);
	fread(&mapx, sizeof(unsigned long), 1, data);
	fread(hoge, sizeof(double), 4, data);
	fread(&XSIZE, sizeof(unsigned long), 1, data);
	fread(&YSIZE, sizeof(unsigned long), 1, data);
	fclose(data);

	/* 使用する動的メモリを確保 */
	if ((maps = malloc(XSIZE * YSIZE * 3 * sizeof(unsigned char))) == NULL) return;
	if ((Auddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return;
	if ((Buddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return;
	if ((Cuddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return;

	printf("Bitmap Tranc.\n");

	/* データをピクセルマップに変換保存 */
	for (count = 0; count < core + 1; count++)
	{
		sprintf(buf, "%x%x.bin", count, core);
		if (count == core) strcpy(buf, "00.bin");

		if ((data = fopen(buf, "rb")) == NULL) return;

		fread(Cuddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);

		for (mapy = 0; mapy < YSIZE; mapy++)
			for (mapx = 0; mapx < XSIZE; mapx++)
			{
				Auddhamaps[mapx + XSIZE * mapy] += Cuddhamaps[mapx + XSIZE * mapy];
				if (mapy > 0 && count != core) Auddhamaps[mapx + XSIZE * (YSIZE - mapy)] += Cuddhamaps[mapx + XSIZE * mapy];
			}

		fread(Cuddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);

		for (mapy = 0; mapy < YSIZE; mapy++)
			for (mapx = 0; mapx < XSIZE; mapx++)
			{
				Buddhamaps[mapx + XSIZE * mapy] += Cuddhamaps[mapx + XSIZE * mapy];
				if (mapy > 0 && count != core) Buddhamaps[mapx + XSIZE * (YSIZE - mapy)] += Cuddhamaps[mapx + XSIZE * mapy];
			}

		fclose(data);
	}

	free(Cuddhamaps);

	printf("Bitmap Save Start.\n");

	/* ピクセルマップをカラーにして変換保存 */
	for (count = 0; count < 2; count++)
	{
		if (count) Cuddhamaps = Buddhamaps;
		else  Cuddhamaps = Auddhamaps;

		for (mapy = 0; mapy < YSIZE; mapy++)
			for (mapx = 0; mapx < XSIZE; mapx++)
				if (Cuddhamaps[XSIZE * YSIZE] < Cuddhamaps[mapx + XSIZE * mapy])
					Cuddhamaps[XSIZE * YSIZE] = Cuddhamaps[mapx + XSIZE * mapy];

		for (mapy = 0; mapy < YSIZE; mapy++)
			for (mapx = 0; mapx < XSIZE; mapx++)
			{
				inf = M_PI * 330 / 180 * log(Cuddhamaps[mapx + XSIZE * mapy]) / log(Cuddhamaps[XSIZE * YSIZE]);
				maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
			}
		sprintf(buf, "%s_%c.bmp", filename, 'a' + count);
		printf("CREATE: %s\n", buf);
		bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);
	}
	free(maps);
	free(Auddhamaps);
	free(Buddhamaps);
	printf("Bitmap Save Finish.\n");
	return;
}