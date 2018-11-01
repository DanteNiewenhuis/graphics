/* Computer Graphics, Assignment 1, Bresenham's Midpoint Line-Algorithm
 *
 * Filename ........ mla.c
 * Description ..... Midpoint Line Algorithm
 * Created by ...... Jurgen Sturm
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 *
 * (always fill in these fields before submitting!!)
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

// Swaps two two-dimensional vectors (i.e. points).
int swap(int* x0, int* x1, int* y0, int* y1) {
  int s = *x0;
  *x0 = *x1;
  *x1 = s;
  int k = *y0;
  *y0 = *y1;
  *y1 = k;

  return 1;
}
 
// Optimized version of mla().
void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  int d, x, y, x0s, y0s, x_incr, y_incr, x_update, xy_update, comp, *pointerX, 
      *pointerY, is_swapped = 0;
  
  // put end pixels
  PutPixel(t, x0, y0, colour);
  PutPixel(t, x1, y1, colour);
  
  // Ensure x1 >= x0. This ensures that we always draw from left to right.
  if (x0 > x1) {
    swap(&x0, &x1, &y0, &y1);
  }

  if ((y1 >= y0) && (x1-x0) > (y1-y0)) {
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) + 1;
    x_incr = 1;
    y_incr = 1;
    comp = -1;
  // Tackle nearly horizontal top actant.
  } else if ((y0 > y1) && (x1-x0) >= (y0-y1)) {
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) - 1;
    x_incr = 1;
    y_incr = -1;
    comp = 1;
  // Tackle nearly vertical bottom actant.
  } else if ((y0 <= y1) && (y1-y0) >= (x1-x0)) {
    is_swapped = swap(&x0, &y0, &x1, &y1);
    x0s = (x0 + 1) << 1;
    y0s = (y0 << 1) + 1;
    x_incr = 1;
    y_incr = 1;
    comp = -1;
  // Tackle nearly vertical top actant.
  } else {
    is_swapped = swap(&x0, &y0, &x1, &y1);
    x0s = (x0 - 1) << 1;
    y0s = (y0 << 1) + 1;
    x_incr = -1;
    y_incr = 1;
    comp = 1;
  }
          
  // Execute mla algorithm with modifications for the 
  // different cases and uses integers only.
  d = (y0 - y1)*x0s + (x1 - x0)*y0s + ((x0*y1) << 1) - ((x1*y0) << 1);
  x_update = (x_incr << 1)*(y0 - y1);
  xy_update = (y_incr << 1)*(x1 - x0) + x_update;
  
  pointerX = is_swapped ? &y : &x;
  pointerY = is_swapped ? &x : &y;
  for (x = x0, y = y0; x != x1; x += x_incr) {
    PutPixel(t, *pointerX, *pointerY, colour);
    if (comp*d > 0) {
      y += y_incr;
      d += xy_update;
    } else {
      d += x_update;
    }
  }

  return;
}


/* REMOVE THIS FROM COMMENTS TO SEE THE OLDER VERSION
void mla(SDL_Texture *t, int x0, int y0, int x1, int y1, Uint32 colour) {
  int d, x, y;
  
  // put end pixels
  PutPixel(t, x0, y0, colour);
  PutPixel(t, x1, y1, colour);
  
  // Ensure x1 >= x0 by swapping (if needed).
  // This takes care of the left side of the circle.
  if (x0 > x1) {
    x = x0;
    y = y0; 
    x0 = x1;
    y0 = y1;
    x1 = x;
    y1 = y;
  }
    
  // tackle nearly horizontal bottom octant
  if ((y1 >= y0) && (x1-x0) >= (y1-y0)) {
    
    d = (y0-y1)*(x0+1) + (x1-x0)*(y0+0.5) + x0*y1 - x1*y0;
    for (x=x0, y=y0; x!=x1; x++) {
      PutPixel(t, x, y, colour);
      if (d < 0) {
        y = y + 1;
        d = d + (y0 - y1) + (x1 - x0);
      } else {
        d = d + (y0 - y1);
      }
    }
  
  // tackle nearly horizontal top actant
  } else if ((y0 > y1) && (x1-x0) >= (y0-y1)) {

    d = (y0-y1)*(x0+1) + (x1-x0)*(y0-0.5) + x0*y1 - x1*y0;
    for (x=x0, y=y0; x!=x1; x++) {
      PutPixel(t, x, y, colour);
      if (d > 0) {
        y = y - 1;
        d = d + (y0 - y1) - (x1 - x0);
      } else {
        d = d + (y0 - y1);
      }
    }
  
  // tackle nearly vertical bottom octant
  } else if ((y0 < y1) && (y1-y0) > (x1-x0)) {
    
    d = (x0-x1)*(y0+1) + (y1-y0)*(x0+0.5) + y0*x1 - y1*x0;
    for (x=x0, y=y0; y!=y1; y++) {
      PutPixel(t, x, y, colour);
      if (d < 0) {
        x = x + 1;
        d = d + (x0 - x1) + (y1 - y0);
      } else {
        d = d + (x0 - x1);
      }
    }

  // tackle nearly vertical top octant
  } else if ((y0 > y1) && (y0-y1) > (x1-x0)) {

    d = (x0-x1)*(y0-1) + (y1-y0)*(x0+0.5) + y0*x1 - y1*x0;
    for (x=x0, y=y0; y!=y1; y--) {
      PutPixel(t, x, y, colour);
      if (d > 0) {
        x = x + 1;
        d = d - (x0 - x1) + (y1 - y0);
      } else {
        d = d - (x0 - x1);
      }
    }
  }
}
*/

