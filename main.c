/* �����v���O���~���O main.c */
/* �u�b�_�u�� �o�̓��j���[ */
/* 2013.01.09, 10RU004, �΋��ˑ� */

#include <time.h>
#include "Buddhabrot.h"

void main(void)
{	/*�F�ʃ��[�h�I��\n1:\t�ω���\n2:\t�W�{���U\n3:\t�W���΍�\n4:\t���U�ɗv�����񐔂̋t��*/
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
			{	/* ctime�֐��ŕԂ������񒆂̃t�@�C�����Ɏw��ł��Ȃ�������u�� */
				if (buf[count] == ':') buf[count] = '_';
				if (buf[count] == '\n') buf[count] = '\0';
			}
		sprintf(file, "%s", buf);
		}
	}

	printf("\n�o�̓t�@�C���� : \"%s\"\n�o�͒�... ���΂炭���҂���������\n", file);

	if (Buddhabrot(file, colmode, 1000000, -2.5, 1.5, -2, 2, 5000, 5000) != 0)
		 printf("�G���[���������܂���\n");
	else printf("�o�͂��I�����܂���\n");

	return;
}