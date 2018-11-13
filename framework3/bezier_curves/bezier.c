/* Computer Graphics, Assignment, Bezier curves
 * Filename ........ bezier.c
 * Description ..... Bezier curves
 * Date ............ 22.07.2009
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

#include <math.h>
#include "bezier.h"
#include <stdio.h>

/* Given a Bezier curve defined by the 'num_points' control points
 * in 'p' compute the position of the point on the curve for parameter
 * value 'u'.
 *
 * Return the x and y values of the point by setting *x and *y,
 * respectively.
 */
 
// Calculates the factorial of some number k.
float fac(int k) {
    if (k) {
        int num = k;
        for (int i = k - 1; i > 0; i--) {
            num *= i;
        }
        return (float) num;
    }
    return 1.0;
}

// Calculates the binomial distribution.
float binomial_distr(int n, int k) {
    return fac(n) / (fac(k) * fac(n - k));
}

// Evaluates a bezier curve.
void
evaluate_bezier_curve(float *x, float *y, control_point p[], int num_points, float u)
{
    *x = 0.0;
    *y = 0.0;
    
    float bino_dist, B_n;
    int n = num_points - 1;
    // printf("evaluate, n: %d, u: %f\n", n, u);

    float u_m_pow = pow(1 - u, n);
    float u_pow = 1;
    for (int i = 0; i < num_points; i++) {
        bino_dist = binomial_distr(n, i);

        // printf("u_pow: %f, u_m_pow: %f\n", u_pow, u_m_pow);
        B_n = bino_dist * u_pow * pow(1 - u, n - i);
        
        u_pow *= u;
        u_m_pow /= (1 - u);
        *x += B_n * p[i].x;
        *y += B_n * p[i].y;
    }
}

/* Draw a Bezier curve defined by the control points in p[], which
 * will contain 'num_points' points.
 *
 *
 * The 'num_segments' parameter determines the "discretization" of the Bezier
 * curve and is the number of straight line segments that should be used
 * to approximate the curve.
 *
 * Call evaluate_bezier_curve() to compute the necessary points on
 * the curve.
 *
 * You will have to create a GLfloat array of the right size, filled with the
 * vertices in the appropriate format and bind this to the buffer.
 */

void
draw_bezier_curve(int num_segments, control_point p[], int num_points)
{
    GLuint buffer[1];

    /* Write your own code to create and fill the array here. */
    GLfloat verts[num_segments+1][2];
    
    float x, y;
    int i;
    float u;
    float u_incr = 1.0 / num_segments;
    for (i = 0, u = 0; i < num_segments; i++, u += u_incr) {
        evaluate_bezier_curve(&x, &y, p, num_points, u);
        verts[i][0] = x;
        verts[i][1] = y;
    }
    // Add last control point to verts. 
    verts[i][0] = p[num_points-1].x;
    verts[i][1] = p[num_points-1].y;

    // This creates the VBO and binds an array to it.
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts),
                 verts, GL_STATIC_DRAW);

    // This tells OpenGL to draw what is in the buffer as a Line Strip.
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, 0);
    glDrawArrays(GL_LINE_STRIP, 0, num_segments+1);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, buffer);
}

/* Find the intersection of a cubic Bezier curve with the line X=x.
   Return 1 if an intersection was found and place the corresponding y
   value in *y.
   Return 0 if no intersection exists.
*/

int
intersect_cubic_bezier_curve(float *y, control_point p[], float x)
{
    float u, y_new, x_new;
    float u_min = -1.0, u_max = 2.0;
    
    // Implements binary search to find the value of u.
    for (int i = 0; i < 10; i++) {
       // Guess best value for u.
       u = (u_min + u_max) / 2;
       
       // Evaluate with current estimate of u.
       evaluate_bezier_curve(&x_new, &y_new, p, 4, u);
       
       // If x_new is too small, then update min and max margins.
       if (x_new < x) {
          u_min = u;
       } else {
          u_max = u;
       }
    }    
    // If u makes sense, then return value, else fail.
    if (u >= 0.0 && u <= 1.0) {
        *y = y_new;
        return 1;
    }
    //printf("No intersection! %f\n", u);
    return 0;
}
