/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file lens_mist.cpp
 *     Mist lense effect functions.
 * @par Purpose:
 *     Functions to apply Mist lense effect to the image.
 * @par Comment:
 *     None.
 * @author   Tomasz Lis
 * @date     05 Jan 2009 - 12 Aug 2009
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#include "lens_mist.h"
#include "globals.h"
#include "bflib_basics.h"

/******************************************************************************/

/******************************************************************************/
CMistFade *Mist = new CMistFade();
/******************************************************************************/

void CMistFade::setup(unsigned char *lens_mem, unsigned char *fade, unsigned char *ghost)
{
  this->lens_data = lens_mem;
  this->fade_data = fade;
  this->ghost_data = ghost;
  this->field_C = 0;
  this->field_D = 0;
  this->field_E = 50;
  this->field_F = 128;
  this->field_14 = 1024;
  this->field_10 = 0;
  this->field_18 = 2;
  this->field_19 = 1;
  this->field_1A = 253;
  this->field_1B = 3;
}

void CMistFade::animset(long a1, long a2)
{
  this->field_10 = a1;
  this->field_14 = a2;
}

void CMistFade::animate(void)
{
  this->field_C += this->field_18;
  this->field_D += this->field_19;
  this->field_E -= this->field_1A;
  this->field_10 += this->field_14;
  this->field_F += this->field_1B;
}

void CMistFade::mist(unsigned char *dstbuf, long dstwidth, unsigned char *srcbuf, long srcwidth, long width, long height)
{
  unsigned char *src;
  unsigned char *dst;
  unsigned long p2,c2,p1,c1;
  long i,k,n;
  long w,h;

  if ((lens_data == NULL) || (fade_data == NULL))
  {
    LbErrorLog("Can't draw Mist as it's not initialized!\n");
    return;
  }
  src = srcbuf;
  dst = dstbuf;
  p2 = this->field_C;
  c2 = this->field_D;
  p1 = this->field_E;
  c1 = this->field_F;
  for (h=height; h > 0; h--)
  {
    for (w=width; w > 0; w--)
    {
      i = this->lens_data[(c1 << 8) + p1];
      k = this->lens_data[(c2 << 8) + p2];
      n = (k + i) >> 3;
      if (n > 32)
        n = 32;
      if (n < 0)
        n = 0;
      *dst = this->fade_data[(n << 8) + *src];
      src++;
      dst++;
      c1--;
      p2++;
    }
    // Move buffers to end of this line
    dst += (dstwidth-width);
    src += (srcwidth-width);
    // Update other counters
    p2 -= width;
    c1 += width;
    c2++;
    p1--;
  }
}

CMistFade::CMistFade(void)
{
  setup(NULL, NULL, NULL);
}

CMistFade::~CMistFade(void)
{
}

/******************************************************************************/
