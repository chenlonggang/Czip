/*============================================
# Filename: parmaters.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: the big parmater ball passing from CSA to Phi
=============================================*/
#ifndef _PARMATERS
#define _PARMATERS
#include"BasicClass.h"
typedef struct 
{
	i32 alphabetsize;
	i32 n;
	i32 L;
	i32 * start;
	uchar lastchar;
	i32 *& SA;
	uchar *& T;
	i32 * code;
}parmaters;
#endif
