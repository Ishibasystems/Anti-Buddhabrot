/* 数理プログラミング Buddhabrot_multi.c */
/* ブッダブロ BA-DA-DHA マルチコア用 計算部 */
/* 2013.02.27, 10RU004, 石橋祥太 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
	unsigned char count, flag, core, buf[20] = "init.bin", mode = 0;
	unsigned long countZ, countT, mapx, mapy, TMAX, XSIZE, YSIZE,
		*Buddhamaps, *Auddhamaps, *shadow, x_, y_;
	double x, y, *xt, *yt, *mx, *my, xt_, yt_, XMIN, XMAX, YMIN, YMAX;
	FILE *data;

	/* 計算情報を読み込み */
	printf("OPEN: %s\n", buf);
	if ((data = fopen(buf, "r+b")) == NULL) return 1;
	fread(&core, sizeof(unsigned char), 1, data);
	fread(&TMAX, sizeof(unsigned long), 1, data);
	fread(&XMIN, sizeof(double), 1, data);
	fread(&XMAX, sizeof(double), 1, data);
	fread(&YMIN, sizeof(double), 1, data);
	fread(&YMAX, sizeof(double), 1, data);
	fread(&XSIZE, sizeof(unsigned long), 1, data);
	fread(&YSIZE, sizeof(unsigned long), 1, data);

	mode = core >> 4 != 0;
	core = (core >> 4) - 1 << 4 | core & 0x0f;

	/* 使用する動的メモリを確保 */
	if ((xt = malloc(TMAX * sizeof(double))) == NULL) return 1;
	if ((yt = malloc(TMAX * sizeof(double))) == NULL) return 1;
	if ((mx = malloc(XSIZE * sizeof(double))) == NULL) return 1;
	if ((my = malloc(YSIZE * sizeof(double))) == NULL) return 1;
	if ((Buddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return 1;
	if ((Auddhamaps = calloc(XSIZE * YSIZE + 1, sizeof(unsigned long))) == NULL) return 1;

	fread(mx, sizeof(double), XSIZE, data);
	fread(my, sizeof(double), YSIZE, data);

	if (mode)
	{
		fseek(data, 0, SEEK_SET);
		fwrite(&core, sizeof(unsigned char), 1, data);
	}
	fclose(data);

	mapy = mode ? (core & 0x0f) - (core >> 4) - 1 : YSIZE / 2;

	if (mode)
	{
		sprintf(buf, "%02x.bin", core);

		/* 途中結果(途中中断・再開用)を存在すれば入力 */
		printf("OPEN: %s\n", buf);
		if ((data = fopen(buf, "rb")) != NULL)
		{
			printf("Reading\n");
			fread(Buddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
			fread(Auddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
			if (Buddhamaps[XSIZE * YSIZE] != Auddhamaps[XSIZE * YSIZE]) printf("ERROR: Checksum\n");
			mapy = Buddhamaps[XSIZE * YSIZE];
			fclose(data);
		}
		printf("CORE: %d\n", core & 0x0f);
	}

	printf("START: %lu from %lu\n", mode ? mapy % (core & 0x0f) : mapy, mapy);

	for (; mapy < YSIZE / 2 || mode == 0; mapy += core & 0x0f)
	{	/* マンデルブロー集合を求める */
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			x = (XMAX - XMIN) * mapx / XSIZE + XMIN;
			y = (YMAX - YMIN) * mapy / YSIZE + YMIN;

			xt_ = yt_ = 0.0;

			flag = 1;

			for(countZ = 0; countZ < TMAX && flag; countZ++)
			{
				xt[countZ] = xt_ * xt_ - yt_ * yt_ + x;
				yt[countZ] = xt_ * yt_ * 2 + y;

				xt_ = xt[countZ];
				yt_ = yt[countZ];

				if (xt_ * xt_ + yt_ * yt_ > 4) flag = 0;
			}

			/* 加算するマップの振分け */
			if (flag)
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
			
			if (mode == 0) printf("LOOP: %lu / %lu\n", mapx, XSIZE);
		}

		if (mode == 0) break;

		/* 途中結果(途中中断・再開用)保存は計算の1/4以上の範囲が対象 */
		if (YSIZE * 1 / 4 < mapy)
		{
			printf("Writing: %s\n", buf);

			if ((data = fopen(buf, "wb")) != NULL)
			{	/* 途中結果を出力 */
				Buddhamaps[XSIZE * YSIZE] = mapy + (core & 0x0f);
				Auddhamaps[XSIZE * YSIZE] = Buddhamaps[XSIZE * YSIZE];
				fwrite(Buddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
				fwrite(Auddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
				fclose(data);
			}
		}
		printf("LOOP: %lu / %lu\n", mapy / (core & 0x0f) + 1, YSIZE / 2 / (core & 0x0f));
	}

	printf("Writing: %s\n", buf);

	/* 結果を出力 */
	while ((data = fopen(mode ? buf : "00.bin", "wb")) == NULL);

	Buddhamaps[XSIZE * YSIZE] = mapy + (core & 0x0f);
	Auddhamaps[XSIZE * YSIZE] = 0;	/* 予約領域 */
	fwrite(Buddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
	fwrite(Auddhamaps, sizeof(unsigned long), XSIZE * YSIZE + 1, data);
	fclose(data);

	free(xt);
	free(yt);
	free(mx);
	free(my);
	free(Buddhamaps);
	free(Auddhamaps);
	return 0;
}