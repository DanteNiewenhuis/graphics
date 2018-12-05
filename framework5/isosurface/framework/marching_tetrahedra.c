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

	//MAKE OUTLIER DETECTION
    
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
	int b_sum = 8 * b[0] + 4 * b[1] + 2* b[2] + b[3];
	triangle tri0, tri1;

	switch (b_sum) {
		// 0000, 1111
		case 0: 
		case 15: 	return 0;

		// 0001, 1110
		case 1:
		case 14:
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			
			triangles[0] = tri0;
			return 1;

		// 0010, 1101
		case 2:
		case 13: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
			
			triangles[0] = tri0;
			return 0;

		// 0100, 1011
		case 4:
		case 11: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
			
			triangles[0] = tri0;
			return 0;

		// 1000, 0111
		case 8:
		case 7: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], c.value[v3], c.value[v0]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v3], c.p[v2], c.value[v3], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v1], c.value[v3], c.value[v1]);
			
			triangles[0] = tri0;
			return 0;

		// 0011, 1100
		case 3:
		case 12: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[0] = tri0;
			
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[1] = tri1;
			return 0;

		// 0110, 1001
		case 6:
		case 9: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[0] = tri0;
			
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[1] = tri1;
			return 0;

		// 0101, 1010
		case 5:
		case 10: 	
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[0] = tri0;
			
			tri0.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri0.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			tri0.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			triangles[1] = tri1;
			return 0;
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
						{0, 2, 6, 7}, 
						{0, 1, 5, 7}, 
						{0, 2, 3, 7}, 
						{0, 4, 5, 7}, 
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






