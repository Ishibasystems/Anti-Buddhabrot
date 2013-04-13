/* 数理プログラミング Buddhabrot.c */
/* ブッダブロ BA-DA-DHA */
/* 2013.01.15, 10RU004, 石橋祥太 */

#include "Buddhabrot.h"
#include <limits.h>
#include <float.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795L	/* VC++6.0に定義されてなかった */
#endif

char Buddhabrot(const char *filename, unsigned char mode, unsigned long TMAX,
	double XMIN, double XMAX, double YMIN, double YMAX, unsigned long XSIZE, unsigned long YSIZE)
{
	unsigned char *maps, colmode = mode & 7, buf[500];
	unsigned long count, countZ, countT, mapx, mapy,
		*Buddhamaps, *Auddhamaps, *shadow, x_, y_, pro1, pro2;
	double x, y, *xt, *yt, *zt, *mx, *my, xt_, yt_, sT, fT, inf, *Mandelmaps,
		ZMIN = DBL_MAX, ZMAX = -DBL_MAX, INF = -log(0);

	/* 使用する動的メモリを確保 */
	if ((xt = malloc(TMAX * sizeof(double))) == NULL) return 1;
	if ((yt = malloc(TMAX * sizeof(double))) == NULL) return 1;
	if ((zt = malloc(TMAX * sizeof(double))) == NULL) return 1;
	if ((mx = malloc(XSIZE * sizeof(double))) == NULL) return 1;
	if ((my = malloc(YSIZE * sizeof(double))) == NULL) return 1;
	if ((maps = malloc(XSIZE * YSIZE * 3 * sizeof(unsigned char))) == NULL) return 1;
	if ((Mandelmaps = malloc(XSIZE * YSIZE * sizeof(double))) == NULL) return 1;
	if ((Buddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return 1;
	if ((Auddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return 1;

	pro1 = pro2 = 0;
	for (count = 0; count < XSIZE; mx[count++] = (XMAX - XMIN) * count / XSIZE + XMIN);
	for (count = 0; count < YSIZE; my[count++] = (YMAX - YMIN) * count / YSIZE + YMIN);

	/* マンデルブロー集合を求める */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			x = (XMAX - XMIN) * mapx / XSIZE + XMIN;
			y = (YMAX - YMIN) * mapy / YSIZE + YMIN;

			xt_ = yt_ = 0.0;

			Mandelmaps[mapx + XSIZE * mapy] = INF;

			for(countZ = 0; countZ < TMAX; countZ++)
			{
				xt[countZ] = xt_ * xt_ - yt_ * yt_ + x;
				yt[countZ] = xt_ * yt_ * 2 + y;

				xt_ = xt[countZ];
				yt_ = yt[countZ];

				if ((zt[countZ] = sqrt(xt_ * xt_ + yt_ * yt_)) > 2)
				{
					switch(colmode)
					{
						case 0:	/* 最終変化量 */
							inf = (zt[countZ - 0] - zt[countZ - 1]) / 2;
							break;

						case 1:	/* 標本分散 */
						case 2:	/* 標準偏差 */
							countZ++;
							sT = fT = 0.0;
							for(countT = 0; countT < countZ; sT += zt[countT++]);
							sT /= countZ;
							for(countT = 0; countT < countZ; fT += (zt[countT] - sT) * (zt[countT++] - sT));
							inf = fT / countZ;
							break;

						case 3:	/* 発散に要した回数の逆数 */
							inf = -1.0 / (countZ + 1);
							break;

						case 4:	/* 発散に要した回数の対数 */
							inf = log(countZ + 1);
							break;
					}

					if (colmode == 2) inf = sqrt(inf);	/* 標準偏差 */

					Mandelmaps[mapx + XSIZE * mapy] = inf;
					break;
				}
			}

			/* 加算するマップの振分け */
			if (Mandelmaps[mapx + XSIZE * mapy] == INF)
			     shadow = Auddhamaps;	/* 収束したのでアンチブッダブロ */
			else shadow = Buddhamaps;	/* 発散したのでブッダブロ */

			/* 座標をマップに加算 */
			for(countT = 0; countT < countZ; countT++)
			{
				/* 実数座標に最も近いマップ座標を探す */
				for (x_ = 0; mx[x_] < xt[countT] && x_ < XSIZE; x_++);
				for (y_ = 0; my[y_] < yt[countT] && y_ < YSIZE; y_++);
				if (mx[x_] + mx[x_ - 1] > xt[countT] * 2) x_--;
				if (my[y_] + my[y_ - 1] > yt[countT] * 2) y_--;

				if (x_ < 0 || XSIZE <= x_ || x_ < 0 || YSIZE <= y_) continue;	/* 描画面上にない点 */

				if (shadow[x_ + XSIZE * y_] != ULONG_MAX) shadow[x_ + XSIZE * y_]++;	/* 該当した点を明るくする */
			}
			if (25 < 100 * mapy / YSIZE && 100 * mapy / YSIZE < 75)
				if ((pro1 = (unsigned long)(1000000000.0 * (mapx + XSIZE * mapy) / XSIZE / YSIZE)) != pro2)
					printf("STEP: %10.7f %%\n", 0.0000001 * (pro2 = pro1));
		}

	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			/* マンデルブロ: Min-Maxの設定 */
			if (Mandelmaps[mapx + XSIZE * mapy] != INF)
			{
				if (ZMIN > Mandelmaps[mapx + XSIZE * mapy]) ZMIN = Mandelmaps[mapx + XSIZE * mapy];
				if (ZMAX < Mandelmaps[mapx + XSIZE * mapy]) ZMAX = Mandelmaps[mapx + XSIZE * mapy];
			}
			/* ブッダブロ: マップ上で最も明るい点を捜索 */
			if (Buddhamaps[XSIZE * YSIZE] < Buddhamaps[mapx + XSIZE * mapy])
				Buddhamaps[XSIZE * YSIZE] = Buddhamaps[mapx + XSIZE * mapy];
			/* アンチブッダブロ: マップ上で最も明るい点を捜索 */
			if (Auddhamaps[XSIZE * YSIZE] < Auddhamaps[mapx + XSIZE * mapy])
				Auddhamaps[XSIZE * YSIZE] = Auddhamaps[mapx + XSIZE * mapy];
		}

	/* マンデルブロ: データをピクセルマップに変換 */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
			if (Mandelmaps[mapx + XSIZE * mapy] == INF)
			{	/* 発散したので白塗り */
				maps[(mapx + XSIZE * mapy) * 3 + 0] = UCHAR_MAX;
				maps[(mapx + XSIZE * mapy) * 3 + 1] = UCHAR_MAX;
				maps[(mapx + XSIZE * mapy) * 3 + 2] = UCHAR_MAX;
			}
			else
			{	/* 色づけ */
				inf = M_PI * 330 / 180 * log((Mandelmaps[mapx + XSIZE * mapy] - ZMIN) / (ZMAX - ZMIN));
				maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
			}

	/* マンデルブロ: 画像ファイルに出力 */
	sprintf(buf, "%s_m.bmp", filename);
	bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);

	/* ブッダブロ: データをピクセルマップに変換 */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			inf = M_PI * 330 / 180 * log(Buddhamaps[mapx + XSIZE * mapy]) / log(Buddhamaps[XSIZE * YSIZE]);
			maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
		}

	/* ブッダブロ: 画像ファイルに出力 */
	sprintf(buf, "%s_b.bmp", filename);
	bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);

	/* アンチブッダブロ: データをピクセルマップに変換 */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			inf = M_PI * 330 / 180 * log(Auddhamaps[mapx + XSIZE * mapy]) / log(Auddhamaps[XSIZE * YSIZE]);
			maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
		}

	/* アンチブッダブロ: 画像ファイルに出力 */
	sprintf(buf, "%s_a.bmp", filename);
	bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);

	free(xt);
	free(yt);
	free(zt);
	free(mx);
	free(my);
	free(maps);
	free(Mandelmaps);
	free(Buddhamaps);
	free(Auddhamaps);
	return 0;
}