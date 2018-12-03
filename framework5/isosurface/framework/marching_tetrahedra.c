/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 * (always fill in these fields before submitting!!)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "marching_tetrahedra.h"

/* Compute a linearly interpolated position where an isosurface cuts
   an edge between two vertices (p1 and p2), each with their own
   scalar value (v1 and v2) */

static vec3
interpolate_points(unsigned char isovalue, vec3 p1, vec3 p2, unsigned char v1, unsigned char v2)
{
    /* Initially, simply return the midpoint between p1 and p2.
       So no real interpolation is done yet */
       
    // Calculate distance between points.
    float d1 = fabs(v1 - isovalue);
    float d2 = fabs(v2 - isovalue);
    
    // Normalize distances such that d1 + d2 = 1.
    float norm = d1 + d2;
    d1 = d1 / norm;
    d2 = d2 / norm;
    
    return v3_add(v3_multiply(p1, d2), v3_multiply(p2, d1));
}

/* Using the given iso-value generate triangles for the tetrahedron
   defined by corner vertices v0, v1, v2, v3 of cell c.

   Store the resulting triangles in the "triangles" array.

   Return the number of triangles created (either 0, 1, or 2).

   Note: the output array "triangles" should have space for at least
         2 triangles.
*/

static int
generate_tetrahedron_triangles(triangle *triangles, unsigned char isovalue, cell c, int v0, int v1, int v2, int v3)
{
	// Determine whether data point values are greater than the isovalue.
	int b[4] = {c.value[v3] > isovalue, 
				c.value[v2] > isovalue, 
				c.value[v1] > isovalue, 
				c.value[v0] > isovalue};
	int b_sum = b[0] + b[1] + b[2] + b[3];
	
	// If all zeros or all ones, then there are zero triangles.
	if (b_sum == 0 || b_sum == 4) {
		return 0;
	}
	
	// Set up mapping from {0, 1, 2, 3} to cell indices.
	int map[4] = {v3, v2, v1, v0};
	
	// Additionally, maintain interpolated edge points.
	vec3 pts[4];
	int n_pts = 0;
	
	// Loop though edges.
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			// If BW-edge, then interpolate the point on the edge.
			if (i > j && ((b[i] && !b[j]) || (!b[i] && b[j]))) {
				vec3 p_i = c.p[map[i]];
				vec3 p_j = c.p[map[j]];
				
				unsigned char s_i = c.value[map[i]];
				unsigned char s_j = c.value[map[j]];
				
				pts[n_pts] = interpolate_points(isovalue, p_i, p_j, s_i, s_j);
				n_pts++;
			}
		}
	}
	
	// If three points have been found, create single triangle.
	if (n_pts == 3) {
		triangle tri;
		tri.p[0] = pts[0];
		tri.p[1] = pts[1];
		tri.p[2] = pts[2];
		
		triangles[0] = tri;
		return 1;
	} 
	
	// If four points are found, create two triangles forming a quad.
	if (n_pts == 4) {
		triangle tri0, tri1;
		tri0.p[0] = pts[0];
		tri0.p[1] = pts[1];
		tri0.p[2] = pts[2];
		triangles[0] = tri0;
		
		tri1.p[0] = pts[1];
		tri1.p[1] = pts[2];
		tri1.p[2] = pts[3];
		triangles[1] = tri1;
		return 2;
	}
		
	return 0;
}

/* Generate triangles for a single cell for the given iso-value. This function
   should produce at most 6 * 2 triangles (for which the "triangles" array should
   have enough space).

   Use calls to generate_tetrahedron_triangles().

   Return the number of triangles produced
*/

int
generate_cell_triangles(triangle *triangles, cell c, unsigned char isovalue)
{
	int tetras[6][4] = {{0, 1, 3, 7}, 
						{0, 1, 5, 7}, 
						{0, 4, 5, 7}, 
						{0, 2, 6, 7}, 
						{0, 2, 3, 7}, 
						{0, 4, 6, 7}};
						
	int v0, v1, v2, v3;
	int dn;	
	int n = 0;	
	for (int i = 0; i < 6; i++) {
		triangle tris[2];
		v0 = tetras[i][3];
		v1 = tetras[i][2];
		v2 = tetras[i][1];
		v3 = tetras[i][0];
		dn = generate_tetrahedron_triangles(tris, isovalue, c, v0, v1, v2, v3);
		
		for (int j = 0; j < dn; j++) {
			triangles[n++] = tris[j];
		}
	}
	
	return n;
}






