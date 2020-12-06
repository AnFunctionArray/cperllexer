#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

static char* fileoutput, * currpos;

static HANDLE hEvent;

static char chCommand[0xFF];

poll()
{
	*chCommand = _getch();
}

gotopage(n)
{
	char* pnext = fileoutput + 1;
	int index;

	for (index = 0; pnext = strstr(pnext, "\n\n"); ++index)
	{
		pnext += 2;
		if (index == n)
		{
			currpos = pnext;
			return;
		}
	}

	return;
}

showpage()
{
	system("cls");
	printf("%.*s\n\n", (unsigned int)(strstr(currpos, "\n\n") - currpos), currpos);
}

processcommand()
{
	int n;
	char* p;
	switch (*chCommand)
	{
	case 'r':
		return 0;
	case  'g':
		scanf("%d", &n);
		gotopage(n);
		showpage();
		break;
	case KEY_LEFT:

		p = currpos - 2;

		while (*p && *p-- != '\n' || *p && *p != '\n');

		if (*p) currpos = p + 1;
		showpage();
		break;
	case KEY_RIGHT:
		currpos = strstr(currpos, "\n\n") + 2;
		showpage();
	}

	poll();


	return 1;
}

scroll()
{
	HANDLE hThread;
	char* pnext;
	while (pnext = strstr(currpos, "\n\n"))
	{
		//SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), where);
		system("cls");
		printf("%.*s\n\n", (unsigned int)(pnext - currpos), currpos);
		_sleep(1000);
		if (*chCommand)
		{
			while (processcommand());
			*chCommand = '\0';

			pnext = strstr(currpos, "\n\n");

			getc(stdin);

			_sleep(500);

			CreateThread(0, 0, poll, 0, 0, 0);
		}
		currpos = pnext + 2;
	}
}

main()
{
	FILE* foutput = fopen("output.txt", "r");

	size_t szfile;

	fseek(foutput, 0, SEEK_END);

	szfile = ftell(foutput);

	rewind(foutput);

	fileoutput = malloc(szfile + 2);

	fread(fileoutput + 1, szfile, 1, foutput);

	while (--szfile && (fileoutput[szfile + 1] == '\xCD' || fileoutput[szfile + 1] == '\x9'));

	++szfile;

	fileoutput[szfile + 1] = '\0';

	currpos = strstr(fileoutput + 1, "\n\n") + 3;

	*fileoutput = '\0';

	hEvent = CreateEvent(0, 0, 0, 0);

	CreateThread(0, 0, poll, 0, 0, 0);

	scroll();
}