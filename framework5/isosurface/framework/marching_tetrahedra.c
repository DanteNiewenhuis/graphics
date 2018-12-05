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
	triangle tri1, tri2;
	int n = 0;

	// Determine whether data point values are greater than the isovalue.
	int b3 = c.value[v3] > isovalue; 
	int b2 = c.value[v2] > isovalue; 
	int b1 = c.value[v1] > isovalue;
	int b0 = c.value[v0] > isovalue;
	int B = b0 + 2 * b1 + 4 * b2 + 8 * b3;
	
	// If all ones or all zeros, then early termination.
	if (B == 0 || B == 15) {
		return 0;
	}
	
	else if (B == 1 || B == 14) { // 0001 or 1110 (1 triangle)
		tri1.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v2], 1, 1);
		triangles[0] = tri1;
		n = 1;
	}
	
	else if (B == 2 || B == 13) { // 0010 or 1101 (1 triangle)
		tri1.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], 1, 1);
		triangles[0] = tri1;
		n = 1;
	}
	
	else if (B == 3 || B == 12) { // 0011 or 1100 (2 triangles)
		tri1.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v1], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v2], 1, 1);
		triangles[0] = tri1;
		
		tri2.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], 1, 1);
		tri2.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], 1, 1);
		tri2.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], 1, 1);
		triangles[1] = tri2;
		n = 2;
	}
	
	else if (B == 4 || B == 11) { // 0100 or 1011 (1 triangle)
		tri1.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v1], 1, 1);
		triangles[0] = tri1;
		n = 1;
	}
	
	else if (B == 5 || B == 10) { // 0101 or 1010 (2 triangles)
		tri1.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v2], 1, 1);
		triangles[0] = tri1;
		
		tri2.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], 1, 1);
		tri2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], 1, 1);
		tri2.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v1], 1, 1);
		triangles[1] = tri2;
		n = 2;
	}
	
	else if (B == 6 || B == 9) { // 0110 or 1001 (2 triangles)
		tri1.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v0], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], 1, 1);
		triangles[0] = tri1;
		
		tri2.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], 1, 1);
		tri2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], 1, 1);
		tri2.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v1], 1, 1);
		triangles[1] = tri2;
		n = 2;
	}

	else if (B == 7 || B == 8) { // 1000 or 0111 (1 triangle)
		tri1.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], 1, 1);
		tri1.p[1] = interpolate_points(isovalue, c.p[v3], c.p[v2], 1, 1);
		tri1.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v1], 1, 1);
		triangles[0] = tri1;
		n = 1;
	}

	// If the opposite case (0001 vs 1110), then swap points in triangles.
	if (B > 7) {
		vec3 tmp = triangles[0].p[1];
		triangles[0].p[1] = triangles[0].p[2];
		triangles[0].p[2] = tmp;
		if (n == 2) {
			tmp = triangles[1].p[1];
			triangles[1].p[1] = triangles[1].p[2];
			triangles[1].p[2] = tmp;
		} 
	}
	
	return n;
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
	int tetras[6][4] = {{0, 3, 1, 7}, 
						{0, 1, 5, 7}, 
						{0, 5, 4, 7}, 
						{0, 6, 2, 7}, 
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






