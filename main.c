/* 数理プログラミング main.c */
/* ブッダブロ 出力メニュー */
/* 2013.01.09, 10RU004, 石橋祥太 */

#include <time.h>
#include "Buddhabrot.h"

void main(void)
{	/*色量モード選択\n1:\t変化量\n2:\t標本分散\n3:\t標準偏差\n4:\t発散に要した回数の逆数*/
	char colmode = 3, buf[256], file[256] = "\0", type[2][4] = {"ppm", "bmp"};
	unsigned char count;
	time_t timer;

	buf[count = 0] = '\0';

	if (!(count > 4 && buf[count - 4] == '.' && strcmp(&buf[count - 3], type[0]) == 0))
	{
		if (count == 0)
		{
			time(&timer);
			sprintf(buf, "%s", ctime(&timer));
			for(count = 0; count < strlen(buf); count++)
			{	/* ctime関数で返す文字列中のファイル名に指定できない文字を置換 */
				if (buf[count] == ':') buf[count] = '_';
				if (buf[count] == '\n') buf[count] = '\0';
			}
		sprintf(file, "%s", buf);
		}
	}

	printf("\n出力ファイル名 : \"%s\"\n出力中... しばらくお待ちください\n", file);

	if (Buddhabrot(file, colmode, 1000000, -2.5, 1.5, -2, 2, 5000, 5000) != 0)
		 printf("エラーが発生しました\n");
	else printf("出力が終了しました\n");

	return;
}