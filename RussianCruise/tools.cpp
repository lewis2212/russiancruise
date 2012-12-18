#include "tools.h"
#include <stdio.h>
namespace tools {
void log(const char *text){

	FILE *file;

	file = fopen("C:\\cruise.log","a");
	fputs(text, file);
	fputs("\n", file);
	fclose(file);
}
}
