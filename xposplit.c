// xposplit 1.0
//
// Splits Dynamics AX xpo objects definitions exported into filename.xpo into separate files.
//


#include <stdio.h>
#include <string.h>

#define MAX_PATH	256
#define MAX_LINE	4096

void displayusage()
{
  printf( "Usage: xposplit filename.xpo\n\n" );
  printf( "  Splits Dynamics AX xpo objects definitions exported into filename.xpo into separate files.\n" );

}

void rtrim( char *line )
{
  int l = strlen(line);
  if ( l > 0 )
    while ( line[l-1] == ' ' || line[l-1] == 10 )
      l--;
  line[l] = 0;
}

int skr_strcmp(char *line, const char* text)
{
	// compare to the length of text
	return strncmp(line,text,strlen(text));
}

void validatefilename( unsigned char *filename )
{
  int l = strlen(filename);
  int i = 0;
  int sp = 0;
  unsigned char chransi[] ="¤¥†¨©ˆãäà¢—˜«½¾^";
  unsigned char chrascii[]="¥¹ÆæÊê£³ÑñÓóŒœŸ¯¿";
  unsigned char chr7bit[]="AaCcEeLlNnOoSsZzZzo";

  if (l>0)
  {
    if (filename[l-1]=='\r') filename[l-1]=0;
    while (filename[i]!=0)
    {
	    unsigned int p;
	    if (filename[i]==' ') sp++;
	    if (sp==2) 
      {
        filename[i]=0; 
        return;
      }
      if (strchr("/\\?*<>\"",filename[i]) != NULL)
        filename[i]='_';
      for (p=0;p<strlen(chransi);p++)
        if (filename[i]==chransi[p]) filename[i]=chr7bit[p];

      if (filename[i]==94) filename[i]='o';

      i++;
    }
  }
}

int main(int argc, char* argv[])
{
  FILE *sf;
  FILE *df;
  char dfilename[MAX_PATH];
  char line[MAX_LINE];
  int hasHeader = 0;
  int inElement = 0;
  long elementCounter = 0;

  if ( argc != 2 )
  {
    displayusage();
    return 1;
  }

  sf = fopen( argv[1], "rt");
  if (sf==NULL)
  {
    printf("ERROR opening input file %s\n", argv[1] );
    return 2;
  }

  df = NULL;
    
  while ( !feof(sf) )
  {
    if ( fgets( line, MAX_LINE, sf ) != NULL )
    {
      line[MAX_LINE]=0;
      rtrim(line);
	  
	  // header
	  if (skr_strcmp(line,"Exportfile for AOT") == 0)
	  {
		  hasHeader = 1;
	  }
	  
	  
      if (strncmp(line,"OBJECT ",7) == 0)
      {
        if (df!=NULL) fclose(df);
        if (line[7]!='[')
          strncpy( dfilename,line+7, MAX_PATH);
        else
        {
          int p;
          strncpy( dfilename,line+8, MAX_PATH);
          p = strlen(dfilename)-1;
          if ( dfilename[p]==']')
            dfilename[p]=0;
        }
        validatefilename(dfilename);
        strncat( dfilename,".txt", 4);
        df = fopen( dfilename, "wt" );
        if (df==NULL)
        {
          printf("ERROR opening output file %s\n", dfilename );
          return 2;
        }
      }
      if (df!=NULL)
      {
        fputs(line,df);
        fputs("\n",df);
      }
    }
  }
  fclose(sf);
  if (df!=NULL) fclose(df);

  return 0;
}

