////////////////////////////////////////
// XTypes.h

#ifndef _XTYPES_H
#define _XTYPES_H

typedef struct SXVersionSupport
{
	unsigned char ucMajor;
	unsigned char ucMinor;
	unsigned short usBuild;
	unsigned long ulSupport;
} SXVersionSupport;

typedef struct SXTime
{
	unsigned char ucHour;
	unsigned char ucMinute;
	unsigned char ucSecond;
	unsigned char ucHSecond;
} SXTime;

typedef struct SXDate
{
	unsigned short usYear;
	unsigned char ucMonth;
	unsigned char ucDay;
} SXDate;

#endif //_XTYPES_H
