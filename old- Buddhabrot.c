/* �����v���O���~���O Buddhabrot.c */
/* �u�b�_�u�� BA-DA-DHA */
/* 2013.01.15, 10RU004, �΋��ˑ� */

#include "Buddhabrot.h"
#include <limits.h>
#include <float.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795L	/* VC++6.0�ɒ�`����ĂȂ����� */
#endif

char Buddhabrot(const char *filename, unsigned char mode, unsigned long TMAX,
	double XMIN, double XMAX, double YMIN, double YMAX, unsigned long XSIZE, unsigned long YSIZE)
{
	unsigned char *maps, colmode = mode & 7, buf[500];
	unsigned long count, countZ, countT, mapx, mapy,
		*Buddhamaps, *Auddhamaps, *shadow, x_, y_, pro1, pro2;
	double x, y, *xt, *yt, *zt, *mx, *my, xt_, yt_, sT, fT, inf, *Mandelmaps,
		ZMIN = DBL_MAX, ZMAX = -DBL_MAX, INF = -log(0);

	/* �g�p���铮�I���������m�� */
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

	/* �}���f���u���[�W�������߂� */
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
						case 0:	/* �ŏI�ω��� */
							inf = (zt[countZ - 0] - zt[countZ - 1]) / 2;
							break;

						case 1:	/* �W�{���U */
						case 2:	/* �W���΍� */
							countZ++;
							sT = fT = 0.0;
							for(countT = 0; countT < countZ; sT += zt[countT++]);
							sT /= countZ;
							for(countT = 0; countT < countZ; fT += (zt[countT] - sT) * (zt[countT++] - sT));
							inf = fT / countZ;
							break;

						case 3:	/* ���U�ɗv�����񐔂̋t�� */
							inf = -1.0 / (countZ + 1);
							break;

						case 4:	/* ���U�ɗv�����񐔂̑ΐ� */
							inf = log(countZ + 1);
							break;
					}

					if (colmode == 2) inf = sqrt(inf);	/* �W���΍� */

					Mandelmaps[mapx + XSIZE * mapy] = inf;
					break;
				}
			}

			/* ���Z����}�b�v�̐U���� */
			if (Mandelmaps[mapx + XSIZE * mapy] == INF)
			     shadow = Auddhamaps;	/* ���������̂ŃA���`�u�b�_�u�� */
			else shadow = Buddhamaps;	/* ���U�����̂Ńu�b�_�u�� */

			/* ���W���}�b�v�ɉ��Z */
			for(countT = 0; countT < countZ; countT++)
			{
				/* �������W�ɍł��߂��}�b�v���W��T�� */
				for (x_ = 0; mx[x_] < xt[countT] && x_ < XSIZE; x_++);
				for (y_ = 0; my[y_] < yt[countT] && y_ < YSIZE; y_++);
				if (mx[x_] + mx[x_ - 1] > xt[countT] * 2) x_--;
				if (my[y_] + my[y_ - 1] > yt[countT] * 2) y_--;

				if (x_ < 0 || XSIZE <= x_ || x_ < 0 || YSIZE <= y_) continue;	/* �`��ʏ�ɂȂ��_ */

				if (shadow[x_ + XSIZE * y_] != ULONG_MAX) shadow[x_ + XSIZE * y_]++;	/* �Y�������_�𖾂邭���� */
			}
			if (25 < 100 * mapy / YSIZE && 100 * mapy / YSIZE < 75)
				if ((pro1 = (unsigned long)(1000000000.0 * (mapx + XSIZE * mapy) / XSIZE / YSIZE)) != pro2)
					printf("STEP: %10.7f %%\n", 0.0000001 * (pro2 = pro1));
		}

	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			/* �}���f���u��: Min-Max�̐ݒ� */
			if (Mandelmaps[mapx + XSIZE * mapy] != INF)
			{
				if (ZMIN > Mandelmaps[mapx + XSIZE * mapy]) ZMIN = Mandelmaps[mapx + XSIZE * mapy];
				if (ZMAX < Mandelmaps[mapx + XSIZE * mapy]) ZMAX = Mandelmaps[mapx + XSIZE * mapy];
			}
			/* �u�b�_�u��: �}�b�v��ōł����邢�_��{�� */
			if (Buddhamaps[XSIZE * YSIZE] < Buddhamaps[mapx + XSIZE * mapy])
				Buddhamaps[XSIZE * YSIZE] = Buddhamaps[mapx + XSIZE * mapy];
			/* �A���`�u�b�_�u��: �}�b�v��ōł����邢�_��{�� */
			if (Auddhamaps[XSIZE * YSIZE] < Auddhamaps[mapx + XSIZE * mapy])
				Auddhamaps[XSIZE * YSIZE] = Auddhamaps[mapx + XSIZE * mapy];
		}

	/* �}���f���u��: �f�[�^���s�N�Z���}�b�v�ɕϊ� */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
			if (Mandelmaps[mapx + XSIZE * mapy] == INF)
			{	/* ���U�����̂Ŕ��h�� */
				maps[(mapx + XSIZE * mapy) * 3 + 0] = UCHAR_MAX;
				maps[(mapx + XSIZE * mapy) * 3 + 1] = UCHAR_MAX;
				maps[(mapx + XSIZE * mapy) * 3 + 2] = UCHAR_MAX;
			}
			else
			{	/* �F�Â� */
				inf = M_PI * 330 / 180 * log((Mandelmaps[mapx + XSIZE * mapy] - ZMIN) / (ZMAX - ZMIN));
				maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
				maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
			}

	/* �}���f���u��: �摜�t�@�C���ɏo�� */
	sprintf(buf, "%s_m.bmp", filename);
	bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);

	/* �u�b�_�u��: �f�[�^���s�N�Z���}�b�v�ɕϊ� */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			inf = M_PI * 330 / 180 * log(Buddhamaps[mapx + XSIZE * mapy]) / log(Buddhamaps[XSIZE * YSIZE]);
			maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
		}

	/* �u�b�_�u��: �摜�t�@�C���ɏo�� */
	sprintf(buf, "%s_b.bmp", filename);
	bmpsave(buf, maps, XSIZE, YSIZE, 3, 1);

	/* �A���`�u�b�_�u��: �f�[�^���s�N�Z���}�b�v�ɕϊ� */
	for (mapy = 0; mapy < YSIZE; mapy++)
		for (mapx = 0; mapx < XSIZE; mapx++)
		{
			inf = M_PI * 330 / 180 * log(Auddhamaps[mapx + XSIZE * mapy]) / log(Auddhamaps[XSIZE * YSIZE]);
			maps[(mapx + XSIZE * mapy) * 3 + 0] = (unsigned char)(255 * (cos(inf               ) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 1] = (unsigned char)(255 * (cos(inf - M_PI * 2 / 3) + 1) / 2);
			maps[(mapx + XSIZE * mapy) * 3 + 2] = (unsigned char)(255 * (cos(inf - M_PI * 4 / 3) + 1) / 2);
		}

	/* �A���`�u�b�_�u��: �摜�t�@�C���ɏo�� */
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