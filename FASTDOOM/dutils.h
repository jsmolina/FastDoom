//
// Copyright (C) 1993-1996 Id Software, Inc.
// Copyright (C) 2016-2017 Alexey Khokholov (Nuke.YKT)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Cheat code checking.
//

#ifndef __DUTILS__
#define __DUTILS__

typedef struct
{
    const unsigned char *sequence;
    unsigned char *p;

} cheatseq_t;

typedef unsigned char byte;

byte cht_CheckCheat(cheatseq_t *cht);
void cht_GetParam(cheatseq_t *cht, char *buffer);

#endif
