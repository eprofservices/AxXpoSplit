// axxposplit v 1.0
//
// Splits Dynamics AX (Axapta) objects definitions exported to xpo into separate files.
//

#include "stdafx.h"

#define _SCL_SECURE_NO_WARNINGS  
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>


#define MAX_PATH	256
#define MAX_LINE	4096

void displayusage()
{
	printf("Usage: axxposplit filename.xpo\n\n");
	printf("  Splits Dynamics AX (Axapta) objects definitions exported to xpo into separate files.\n");

}

void rtrim(char *line)
{
	int l = strlen(line);
	if (l > 0)
		while (line[l - 1] == ' ' || line[l - 1] == 10)
			l--;
	line[l] = 0;
}

void validatefilename(char *filename)
{
	int l = strlen(filename);
	int i = 0;
	int sp = 0;
	char chransi[] = "¤¥†¨©ˆãäà¢—˜«½¾^";
	char chrascii[] = "¥¹ÆæÊê£³ÑñÓóŒœŸ¯¿";
	char chr7bit[] = "AaCcEeLlNnOoSsZzZzo";

	if (l>0)
	{
		if (filename[l - 1] == '\r') filename[l - 1] = 0;
		while (filename[i] != 0)
		{
			unsigned int p;
			if (filename[i] == ' ') sp++;
			if (sp == 2)
			{
				filename[i] = 0;
				return;
			}
			if (strchr("/\\?*<>\"", filename[i]) != NULL)
				filename[i] = '_';
			for (p = 0; p<strlen(chransi); p++)
				if (filename[i] == chransi[p]) filename[i] = chr7bit[p];

			if (filename[i] == 94) filename[i] = 'o';

			i++;
		}
	}
}

void getFilename(char *dfilename, char *nextline)
{
 	//           1         2         3
	// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
	// ; Microsoft Dynamics AX Query: AccountantLogisticsAddress_BR — wy³adunek
	strncpy(dfilename, nextline+24, MAX_PATH);
	if (strcmp(dfilename+strlen(dfilename)-10, " — wy³adunek") == 0)
		dfilename[strlen(dfilename)-10] = 0;
	for( size_t i=0; i<strlen(dfilename); i++)
		if (dfilename[i] == ':' || dfilename[i] == ' ')
			dfilename[i] = '_';
}

int main(int argc, char* argv[])
{
	FILE *sf;
	FILE *df;
	char dfilename[MAX_PATH];
	char line[MAX_LINE];
	char nextline[MAX_LINE];


	if (argc != 2)
	{
		displayusage();
		return 1;
	}

	sf = fopen(argv[1], "rt");
	if (sf == NULL)
	{
		printf("ERROR opening input file %s\n", argv[1]);
		return 2;
	}

	df = NULL;

	while (!feof(sf))
	{
		if (fgets(line, MAX_LINE, sf) != NULL)
		{
			line[MAX_LINE] = 0;
			rtrim(line);
			if (strncmp(line, "***Element:", 11) == 0)
			{
				if (df != NULL) 
				{
					fputs("\n***Element: END\n", df);
					fclose(df);
					df = NULL;
				}
				if (strncmp(line+12, "END", 3) != 0 )
				{
					fgets(nextline, MAX_LINE, sf); // should be empty line
					if (fgets(nextline, MAX_LINE, sf) == NULL )
					{
						printf("ERROR in input file: $s n", line);
						return 3;
					}
				    if (strncmp(nextline, "; Microsoft Dynamics AX", 23) == 0)
					  getFilename(dfilename, nextline);
					else
					{
						printf("ERROR in input file: $s n", nextline);
						return 4;					
					}
					validatefilename(dfilename);
					strncat(dfilename, ".xpo", 4);
					df = fopen(dfilename, "wt");
					if (df == NULL)
					{
						printf("ERROR opening output file %s\n", dfilename);
						return 2;
					}
					fputs("Exportfile for AOT version 1.0 or later\n", df);
					fputs("Formatversion: 1\n\n", df);
					fputs(line, df);
					fputs("\n\n", df);
					fputs(nextline, df);
					fputs("\n", df);
				}
			}
			else if (df != NULL)
			{
				fputs(line, df);
				fputs("\n", df);
			}
		}
	}
	fclose(sf);
	if (df != NULL) fclose(df);

	return 0;
}

