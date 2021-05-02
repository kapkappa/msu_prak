#include <stdlib.h>
#include <stdio.h>

int main(int argc, char**argv)
{
	if(argc<2){
		printf("No input file\n");
		exit(1);
	}
	FILE*f = fopen(argv[1], "r");
	if(f==NULL){
		printf("Error in opening file\n");
		exit(1);
	}
	int ch, s=0, w=0, c=0, word=0;
	while((ch=fgetc(f))!= EOF)
	{
		if(ch==10) s+=1;
		if(word && (ch==' ' || ch=='\n')) word=0;
		else if(!word && ch!=' ' && ch!='\n'){
			word=1;
			w+=1;
		}
		c+=1;
	}
	printf("%d%c%d%c%d%c%s\n",s,' ',w,' ',c,' ',argv[1]);
	fclose(f);
	return 0;
}

