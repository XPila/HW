////////////////////////////////////////
// XPort.c

#include "XPort.h"
#include <malloc.h>

short hex(char c)
{
	static char* tab = "0123456789abcdef";
	return tab[c>>4] | (tab[c&15] << 8);
}

char* hexstr(char* p, int n)
{
	short* str = (short*)malloc(2*n+1);
	int i; for (i = 0; i < n; i++)
		str[i] = hex(p[i]);
	*(char*)(str+n) = 0;
	return (char*)str;
}
