/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
 * Description ..... Midpoint Line Algorithm
 * Created by ...... Jurgen Sturm
 *
 * Student name .... Thomas Bellucci & Dante Niewenhuis
 * Student email ... th.bellucci@gmail.com & d.niewenhuis@hotmail.com
 * Collegekaart .... 11257245 & 11058595
 * Date ............ 2 November 2018
 * Comments ........ Note the bit-shifting, this is faster than multiplying
 *                   by 2. This "times 2" is used to get rid of floating point 
 *                   operations and make the function integer-only. Additionally
 *                   note the swap function, this is used make sure the program
 *                   always draws from one side to the other (e.g. left to 
 *                   right).
 */

#include "SDL.h"
#include "init.h"

/*
 * Midpoint Line Algorithm
 *
 * As you probably will have figured out, this is the part where you prove
 * your programming skills. The code in the mla function should draw a direct
 * line between (x0,y0) and (x1,y1) in the specified color.
 *
 * Until now, the example code below draws only a horizontal line between
 * (x0,y0) and (x1,y0) and a vertical line between (x1,y1).
 *
 * And here the challenge begins..
 *
 * Good luck!
 *
 *
 */

// Swaps two points each defined by x and y coordinates.
int swap(int* x0, int* x1, int* y0, int* y1) {
  int s = *x0;
  *x0 = *x1;
  *x1 = s;
  s = *y0;
  *y0 = *y1;
  *y1 = s;
  
  return 1;
}
 
// Optimized version of mla().
void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  int d, x, y, x0s, y0s, x_update, xy_update, *pointerX, *pointerY;
  int x_incr = 1, y_incr = 1, comp = 1, is_swapped = 0;
  
  // Place end pixels.
  PutPixel(t, x0, y0, colour);
  PutPixel(t, x1, y1, colour);
  
  // Ensure x1 >= x0. TEnsures that we always draw from from one side to the other.
  if (x0 > x1) {
    swap(&x0, &x1, &y0, &y1);
  }

  // Fill in parameters needed to draw a line in a particular octant.
  if ((y1 >= y0) && (x1-x0) > (y1-y0)) {
    // Tackle nearly horizontal bottom actant.
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) + 1;

  } else if ((y0 > y1) && (x1-x0) >= (y0-y1)) {
    // Tackle nearly horizontal top actant.
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) - 1;
    y_incr = -1;
    comp = -1;
  } else if ((y0 <= y1) && (y1-y0) >= (x1-x0)) {
    // Tackle nearly vertical bottom actant.
    is_swapped = swap(&x0, &y0, &x1, &y1);
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) + 1;
  } else {
    // Tackle nearly vertical top actant.
    is_swapped = swap(&x0, &y0, &x1, &y1);
    x0s = (x0 - 1) << 1;
    y0s = (y0 << 1) + 1;
    x_incr = -1;
    comp = -1;
  }
          
  // Pre-compute starting value d and its update values.
  d = (y0 - y1)*x0s + (x1 - x0)*y0s + ((x0*y1) << 1) - ((x1*y0) << 1);
  x_update = (x_incr << 1)*(y0 - y1);
  xy_update = (y_incr << 1)*(x1 - x0) + x_update;
  
  // Create pointers to location of x and y variables to get correct 
  // pixel coordinate for drawing the pixel when swapped.
  pointerX = is_swapped ? &y : &x;
  pointerY = is_swapped ? &x : &y;
  
  // Loop through x (or y when swapped) and place pixel at (x,y).
  for (x = x0, y = y0; x != x1; x += x_incr) {
    PutPixel(t, *pointerX, *pointerY, colour);
    if (comp*d < 0) {
      y += y_incr;
      d += xy_update;
    } else {
      d += x_update;
    }
  }

  return;
}