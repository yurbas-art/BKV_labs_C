#include <stdio.h>
#include <ctype.h>
int isvowel(int c);
int main(int argc, char **argv)
{
   FILE *F; int c, letter, cons, vowel, total=0;
  while ( --argc )
 {
   F=fopen(argv[argc],"r");
   	total=letter=cons=vowel=0;
   	while ( (c=fgetc(F))!=EOF)
   	 {
		  total++;
   	      if (isalpha(c))
   	      { 
   	       letter++;
   	       if (isvowel(c)) vowel++;
   	       else cons++;
	      }
   	 }
   	  printf("FILE %s: total=%d letters=%d vowels=%d, cons=%d\n",argv[argc],total,letter,vowel,cons);
  }
 	return 0;
}

int isvowel(int c)
{
	switch(tolower(c))
	{
		case 'a': 
		case 'e':
		case 'i': 
		case 'o': 
		case 'u':
		case 'y': return 1;
	}
	return 0;
}
