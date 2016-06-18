////////////////////////////////////////
// XFmt.c

#include "XFmt.h"
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#define XFMT_MAX_ARGS 16


int XFmt_args(char* format, void* data, int* arg_size, void** arg_ptr, void** arg_val)
{
	int arg = 0;
	void* ptr = data;
	while (*format != 0)
	{
		void* val = 0;
		if ((*format == '%') || (*format == '@'))
		{
			format ++;
			if (*format == 0) return -1; //error - invalid format
			int swap = (*format == '@');
			int width = 0;
			int n = 0;
			if (sscanf(format, "%d%n", &width, &n) == 1) //width scanned
				format += n;
			if (*format == 0) return -1; //error - invalid format
			switch (*format)
			{
			case 0:
				return -1; //error - invalid format
			//prefixes
			case 'l':
				switch (format[1])
				{
				case 0:
					return -1; //error - invalid format
				case 'l': //prefix 'll'
					switch (format[2])
					{
					case 'd':
					case 'i':
					case 'o':
					case 'x':
					case 'X':
						val = ptr;
						arg_size[arg] = sizeof(long long);
						break;
					case 'u':
						val = ptr;
						arg_size[arg] = sizeof(unsigned long long);
						break;
					default:
						return -1; //error - invalid format
					}
					format++;
					break;
				case 'd':
				case 'i':
				case 'o':
				case 'x':
				case 'X':
					*((long int*)&val) = *((long int*)ptr);
					arg_size[arg] = sizeof(long int);
					break;
				case 'u':
					*((unsigned long int*)&val) = *((unsigned long int*)ptr);
					arg_size[arg] = sizeof(unsigned long int);
					break;
				case 'e':
				case 'E':
				case 'f':
				case 'g':
				case 'G':
					val = ptr;
					arg_size[arg] = sizeof(double);
					break;
				case 'c':
				case 'C':
					*((wchar_t*)&val) = *((wchar_t*)ptr);
					arg_size[arg] = sizeof(wchar_t);
					break;
				case 's':
				case 'S':
					val = ptr;
					arg_size[arg] = width * sizeof(wchar_t);
					break;
				default:
					return -1; //error - invalid format
				}
				format++;
				break;
			case 'L':
				switch (format[1])
				{
				case 0:
					return -1; //error - invalid format
				case 'e':
				case 'E':
				case 'f':
				case 'g':
				case 'G':
					val = ptr;
					arg_size[arg] = sizeof(long double);
					break;
				default:
					return -1; //error - invalid format
				}
				format++;
				break;
			case 'h':
				switch (format[1])
				{
				case 0:
					return -1; //error - invalid format
				case 'h': //prefix 'hh'
					switch (format[2])
					{
					case 'd':
					case 'i':
					case 'o':
					case 'x':
					case 'X':
						*((char*)&val) = *((char*)ptr);
						arg_size[arg] = sizeof(char);
						break;
					case 'u':
						*((unsigned char*)&val) = *((unsigned char*)ptr);
						arg_size[arg] = sizeof(unsigned char);
						break;
					default:
						return -1; //error - invalid format
					}
					format++;
					break;
				case 'd':
				case 'i':
				case 'o':
				case 'x':
				case 'X':
					*((short int*)&val) = *((short int*)ptr);
					arg_size[arg] = sizeof(short int);
					break;
				case 'u':
					*((unsigned short int*)&val) = *((unsigned short int*)ptr);
					arg_size[arg] = sizeof(unsigned short int);
					break;
				case 'c':
				case 'C':
					*((char*)&val) = *((char*)ptr);
					arg_size[arg] = sizeof(char);
					break;
				case 's':
				case 'S':
					val = ptr;
					arg_size[arg] = width * sizeof(char);
					break;
				default:
					return -1; //error - invalid format
				}
				format++;
				break;	
			//types
			case 'c':
				*((char*)&val) = *((char*)ptr);
				arg_size[arg] = sizeof(char);
				break;
			case 'C':
				*((wchar_t*)&val) = *((wchar_t*)ptr);
				arg_size[arg] = sizeof(wchar_t);
				break;
			case 'd':
			case 'i':
			case 'o':
			case 'x':
			case 'X':
				*((int*)&val) = *((int*)ptr);
				arg_size[arg] = sizeof(int);
				break;
			case 'u':
				*((unsigned int*)&val) = *((unsigned int*)ptr);
				arg_size[arg] = sizeof(unsigned int);
				break;
			case 'e':
			case 'E':
			case 'f':
			case 'g':
			case 'G':
				*((float*)&val) = *((float*)ptr);
				arg_size[arg] = sizeof(float);
				break;
			case 's':
				val = ptr;
				arg_size[arg] = width * sizeof(char);
				break;
			case 'S':
				val = ptr;
				arg_size[arg] = width * sizeof(wchar_t);
				break;
			default:
				return -1; //error - invalid format;
			}
			format++;
			if (arg_ptr != 0) arg_ptr[arg] = ptr;
			if (arg_val != 0) arg_val[arg] = val;
			ptr = (char*)ptr + arg_size[arg];
			arg++;
		}
		else
			format++;
	}
	return arg;
}

int XFmt_sscanf(char* str, char* format, void* data)
{
	int arg_size[XFMT_MAX_ARGS];
	void* arg_ptr[XFMT_MAX_ARGS];
	int arg_count = XFmt_args(format, data, arg_size, arg_ptr, 0);
	if (arg_count < 0) return arg_count;
	va_list va;// = 0;//(va_list)arg_ptr[0];
	void** pva = (void**)&va;
	*pva = &arg_ptr;
	return vsscanf(str, format, va);
}

int XFmt_sprintf(char* str, char* format, void* data)
{
	int arg_size[XFMT_MAX_ARGS];
	void* arg_val[XFMT_MAX_ARGS];
	int arg_count = XFmt_args(format, data, arg_size, 0, arg_val);
	if (arg_count < 0) return arg_count;
	va_list va;// = 0;//(va_list)arg_ptr[0];
	void** pva = (void**)&va;
	*pva = &arg_val;
	return vsprintf(str, format, va);
}
