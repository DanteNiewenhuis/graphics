/* Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
 * Description ..... Implements triangle rasterization
 * Created by ...... Paul Melis
 *
 * Student name .... Dante Niewenhuis, Thomas Bellucci
 * Student email ... th.bellucci@gmail.com & d.niewenhuis@hotmail.com
 * Collegekaart .... 11058595, 11257245
 * Date ............ 2 november 2018
 * Comments ........ 
 *  We optimized the triangle drawer by cutting off the drawer when the 
 *  end of the triangle row is reached and thus skipping alot of calculations
 *  We also change the starting point of the row by looking at the first 
 *  point that is drawn in a line. This makes it so the biggest part of the 
 *  empty space before and after the triangle is skipped.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "types.h"

/*
 * Rasterize a single triangle.
 * The triangle is specified by its corner coordinates
 * (x0,y0), (x1,y1) and (x2,y2).
 * The triangle is drawn in color (r,g,b).
 */

// Calculate the minumum value of three values. 
int min(float a, float b, float c) {
  int x_min;
  x_min = (a <= b) ? a : b;
  x_min = (x_min <= c) ? x_min : c;
  return x_min;
}

// Calculate the maximum value of three values.
int max(float a, float b, float c) {
  int x_max;
  x_max = (a > b) ? a : b;
  x_max = (x_max > c) ? x_max : c;
  return x_max;
}

// Implicit line function.
float f(float x0, float y0, float x1, float y1, float x, float y) {
  return (y0-y1)*x + (x1-x0)*y + x0*y1 - x1*y0;
}


void
draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
  // Determine bounding box.
  float x_min, x_max, y_min, y_max;
  x_min = floor(min(x0, x1, x2));
  y_min = floor(min(y0, y1, y2));
  x_max = ceil(max(x0, x1, x2));
  y_max = ceil(max(y0, y1, y2));
  
  // Pre-compute f values.
  float alpha, beta, gamma;
  float fa = f(x1, y1, x2, y2, x0, y0);
  float fb = f(x2, y2, x0, y0, x1, y1);
  float fg = f(x0, y0, x1, y1, x2, y2);
  
  // Calculate f value for off-screen point (-1, -1)^T.
  float fa_off = fa*f(x1, y1, x2, y2, -1, -1) > 0;
  float fb_off = fb*f(x2, y2, x0, y0, -1, -1) > 0;
  float fg_off = fg*f(x0, y0, x1, y1, -1, -1) > 0;
  
  // Loop though bounding box and determine barycentric coordinates.
  for (int y = y_min; y <= y_max; y++) {
    for (int x = x_min; x <= x_max; x++) {
      alpha = f(x1, y1, x2, y2, x, y) / fa;
      beta = f(x2, y2, x0, y0, x, y) / fb;
      gamma = 1.0 - (alpha + beta);

      // Check if the point is in the triangle and thus needs to be drawn.
      if (alpha>=0 && beta>=0 && gamma>=0) {
        if ((alpha > 0 || fa_off) && (beta > 0 || fb_off) && 
            (gamma > 0 || fg_off)) {
          PutPixel(x, y, r, g, b);
        }
      }
    }
  }
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r, byte g, byte b)
{
  float new_d12, new_d20, alpha, beta, gamma;
  int x, y, inTriangle, startingPoint;

  // Determine bounding box.
  int x_min = min(x0, x1, x2);
  int y_min = min(y0, y1, y2);
  int x_max = max(x0, x1, x2);
  int y_max = max(y0, y1, y2);
  
  // Pre-compute f values. 
  float fa = f(x1, y1, x2, y2, x0, y0);
  float fb = f(x2, y2, x0, y0, x1, y1);
  float fg = f(x0, y0, x1, y1, x2, y2);
  
  // Precompute whether fa and f for offscreen point is the same.
  float fa_off = fa*f(x1, y1, x2, y2, -1, -1) > 0;
  float fb_off = fb*f(x2, y2, x0, y0, -1, -1) > 0;
  float fg_off = fg*f(x0, y0, x1, y1, -1, -1) > 0;
  
  // Precompute updates in x- and y-direction
  float d12_x_update = y1 - y2; 
  float d20_x_update = y2 - y0;
  float d12_y_update = x2 - x1; 
  float d20_y_update = x0 - x2;
  
  // Precompute f12 and f20 as s12 and d20 respectively.
  float d12 = f(x1, y1, x2, y2, x_min, y_min);
  float d20 = f(x2, y2, x0, y0, x_min, y_min);
  
  startingPoint = x_min;

  // Loop though bounding box and fill in pixels if in triangle 
  // according to the barycentric coordinates apha, beta and gamma.
  for (y = y_min; y <= y_max; y++) {
  	
  	// Copy d12 and d20 for the inner loop.
  	new_d12 = d12;
  	new_d20 = d20;
  
    inTriangle = 0;
    
    // Loop through row of pixels, compute barycentric coordinates and fill in.
    for (x = startingPoint; x <= x_max; x++) {
      alpha = new_d12 / fa;
      beta = new_d20 / fb;
      gamma = 1.0 - (alpha + beta);

      // Check if the point is in the triangle and thus needs to be drawn.
      if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        if ((alpha > 0 || fa_off) && (beta > 0 || fb_off) && 
            (gamma > 0 || fg_off)) {
        
          // Go back on the x_axis to get to the first point.
          if (x == startingPoint && x > 0) {
              startingPoint--;
              d12 -= d12_x_update;
              d20 -= d20_x_update;
              x-=2;
              new_d12 -= d12_x_update;
              new_d20 -= d20_x_update;
              continue;
          }

          // Update the starting point on the first hit.
          if (!inTriangle) {
            startingPoint = x;
          }
          PutPixel(x, y, r, g, b);
          inTriangle = 1;
        }
        // Stop the loop if the end of the trianglerow is reached.
        else if (inTriangle) {
          break;
        }
      }
      // Stop the loop if the end of the trianglerow is reached.
      else if (inTriangle) {
        break;
      }
      // Update d12 and d20 for moving in the x-direction.
      new_d12 += d12_x_update;
      new_d20 += d20_x_update;

      // Update the initial d12 and d20 if the triangle has not been reached 
      // because the startingPoint will move one to the right.
      if (!inTriangle) {
          d12 += d12_x_update;
          d20 += d20_x_update;
      }
    }

    // Remove all the updates if the row was empty.
    if (!inTriangle) {
        d12 -= (x_max - startingPoint + 1)*d12_x_update;
        d20 -= (x_max - startingPoint + 1)*d20_x_update;
    }
    // Update d12 and d20 for moving in the y-direction.
  	d12 += d12_y_update;
  	d20 += d20_y_update;
  }
}
