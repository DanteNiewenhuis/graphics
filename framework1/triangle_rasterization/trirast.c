/* Computer Graphics assignment, Triangle Rasterization
 * Filename ........ trirast.c
 * Description ..... Implements triangle rasterization
 * Created by ...... Paul Melis
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

void
draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r_color, byte g_color, byte b_color)
{
    int lowest_x = x0;
    int highest_x = x0;
    int lowest_y = y0;
    int highest_y = y0;

    if (x1 < lowest_x) {
        lowest_x = x1;
    }
    if (x1 > highest_x) {
        highest_x = x1;
    }

    if (y1 < lowest_y) {
        lowest_y = y1;
    }
    if (y1 > highest_y) {
        highest_y = y1;
    }

    if (x2 < lowest_x) {
        lowest_x = x2;
    }
    if (x2 > highest_x) {
        highest_x = x2;
    }

    if (y2 < lowest_y) {
        lowest_y = y2;
    }
    if (y2 > highest_y) {
        highest_y = y2;
    }


    // float P[2];

    float a = x1 - x0;
    float b = x2 - x0;
    float c = y1 - y0;
    float d = y2 - y0;
    // float det = 1/(a*d - b*c);

    // float inv[4] = {d*det, -b*det, -c*det, a*det};
    

    // float alpha, beta, gamma;
    // int x, y;
    // for(x = lowest_x; x <= highest_x; x++) {
    //     for (y = lowest_y; y <= highest_y; y++) {
    //         P[0] = x - x0; P[1] = y - y0;

    //         beta = P[0] * inv[0] + P[1] * inv[1];
    //         gamma = P[0] * inv[2] + P[1] * inv[3]; 
    //         alpha = 1 - beta - gamma;
    //         if (beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1 && alpha >= 0 && alpha <= 1) {
    //             PutPixel(x, y, alpha*255, beta*255, gamma*255);
    //         }
    //     }
    // }

    float det = (a*d - b*c);
    float inv1 = d/det;
    float inv2 = -b/det;
    float inv3 = -c/det;
    float inv4 = a/det;
    float alpha, beta, gamma;

    int x, y;
    for(x = lowest_x - x0; x <= highest_x - x0; x++) {
        for (y = lowest_y - y0; y <= highest_y - y0; y++) {

            beta = x * inv1 + y * inv2;
            gamma = x * inv3 + y * inv4;
            if (beta >= 0 && gamma >= 0 && (beta + gamma) <= 1) {
                PutPixel(x + x0, y + y0, r_color, g_color, b_color);
            }
        }
    }
}

void
draw_triangle_optimized(float x0, float y0, float x1, float y1, float x2, float y2,
    byte r_color, byte g_color, byte b_color)
{
    float lowest_x = x0;
    float highest_x = x0;
    float lowest_y = y0;
    float highest_y = y0;

    if (x1 < lowest_x) {
        lowest_x = x1;
    }
    if (x1 > highest_x) {
        highest_x = x1;
    }

    if (y1 < lowest_y) {
        lowest_y = y1;
    }
    if (y1 > highest_y) {
        highest_y = y1;
    }

    if (x2 < lowest_x) {
        lowest_x = x2;
    }
    if (x2 > highest_x) {
        highest_x = x2;
    }

    if (y2 < lowest_y) {
        lowest_y = y2;
    }
    if (y2 > highest_y) {
        highest_y = y2;
    }



    float a = x1 - x0;
    float b = x2 - x0;
    float c = y1 - y0;
    float d = y2 - y0;
    float det = (a*d - b*c);

    float inv1 = d/det;
    float inv2 = -b/det;
    float inv3 = -c/det;
    float inv4 = a/det;

    float betaX, beta, gammaX, gamma;

    betaX = (lowest_x - x0) * inv1 + (lowest_y - y0) * inv2;
    gammaX = (lowest_x - x0) * inv3 + (lowest_y - y0) * inv4;
    beta = betaX;
    gamma = gammaX;
    highest_x++;
    highest_y++;

    for(int x = lowest_x; x < highest_x; x++) {
        
        for (int y = lowest_y; y < highest_y; y++) { 
            printf("beta: %f, gamma: %f\n", beta, gamma);
            if (beta > -0.0001 && gamma >= 0 && (beta + gamma) <= 1.01) {
                printf("executed!\n");
                PutPixel(x, y, r_color, g_color, b_color);
            }
            beta += inv2;
            gamma += inv4;
        }
        betaX += inv1;
        beta = betaX;
        
        gammaX += inv3;
        gamma = gammaX;
    }
}
