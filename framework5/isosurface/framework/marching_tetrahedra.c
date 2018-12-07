/* Computer Graphics, Assignment, Volume rendering with cubes/points/isosurface
 *
 * Student name .... Dante Niewenhuis Thomas Bellucci
 * Student email ... d.niewenhuis@hotmail.com	th.bellucci@gmail.com
 * Collegekaart .... 11058595	11257245
 * Date ............ 7 - 12 - 2018
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
	
	// determine the configuration of the tetrahedron and make triangles based on it.
	switch (B) {

		// If all ones or all zeros, then early termination. 
		case 0:
		case 15: return 0;
		
		// 0001 or 1110 (1 triangle).
		case 1:
		case 14:
			tri1.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			triangles[0] = tri1;
			n = 1;
			break;
		
		// 0010 or 1101 (1 triangle).
		case 2:
		case 13:
			tri1.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
			triangles[0] = tri1;
			n = 1;
			break;

		// 1000 or 0111 (1 triangle).
		case 7:
		case 8:
			tri1.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], c.value[v3], c.value[v0]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v3], c.p[v2], c.value[v3], c.value[v2]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v1], c.value[v3], c.value[v1]);
			triangles[0] = tri1;
			n = 1;
			break;
		
		// 0011 or 1100 (2 triangles). 
		case 3: 
		case 12:
			tri1.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v0], c.p[v2], c.value[v0], c.value[v2]);
			triangles[0] = tri1;
			
			tri2.p[0] = interpolate_points(isovalue, c.p[v3], c.p[v0], c.value[v3], c.value[v0]);
			tri2.p[1] = interpolate_points(isovalue, c.p[v1], c.p[v2], c.value[v1], c.value[v2]);
			tri2.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
			triangles[1] = tri2;
			n = 2;
			break;

		// 0100 or 1011 (1 triangle).
		case 4:
		case 11:
			tri1.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
			triangles[0] = tri1;
			n = 1;
			break;
		
		// 0101 or 1010 (2 triangles).
		case 5:
		case 10:
			tri1.p[0] = interpolate_points(isovalue, c.p[v0], c.p[v1], c.value[v0], c.value[v1]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v0], c.p[v3], c.value[v0], c.value[v3]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v2], c.value[v3], c.value[v2]);
			triangles[0] = tri1;
			
			tri2.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
			tri2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
			tri2.p[2] = interpolate_points(isovalue, c.p[v2], c.p[v1], c.value[v2], c.value[v1]);
			triangles[1] = tri2;
			n = 2;
			break;
		
		// 0110 or 1001 (2 triangles).
		case 6:
		case 9: 
			tri1.p[0] = interpolate_points(isovalue, c.p[v1], c.p[v0], c.value[v1], c.value[v0]);
			tri1.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
			tri1.p[2] = interpolate_points(isovalue, c.p[v1], c.p[v3], c.value[v1], c.value[v3]);
			triangles[0] = tri1;
			
			tri2.p[0] = interpolate_points(isovalue, c.p[v2], c.p[v0], c.value[v2], c.value[v0]);
			tri2.p[1] = interpolate_points(isovalue, c.p[v2], c.p[v3], c.value[v2], c.value[v3]);
			tri2.p[2] = interpolate_points(isovalue, c.p[v3], c.p[v1], c.value[v3], c.value[v1]);
			triangles[1] = tri2;
			n = 2;
			break;
	}

	// If the opposite case (0001 vs 1110), swap order of points in triangle.
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
	// Define tetrahedra for the cell.
	int tetras[6][4] = {{0, 3, 1, 7}, 
						{0, 1, 5, 7}, 
						{0, 5, 4, 7}, 
						{0, 6, 2, 7}, 
						{0, 2, 3, 7}, 
						{0, 4, 6, 7}};
						
	int v0, v1, v2, v3;
	int dn;	
	int n = 0;	
	// Loop through tetrahedra.
	for (int i = 0; i < 6; i++) {
		triangle tris[2];
		v0 = tetras[i][3];
		v1 = tetras[i][2];
		v2 = tetras[i][1];
		v3 = tetras[i][0];
		// Get triangles if they exist for the cell.
		dn = generate_tetrahedron_triangles(tris, isovalue, c, v0, v1, v2, v3);
		
		// Fill in triangle array with triangles and maintain a count.
		for (int j = 0; j < dn; j++) {
			triangles[n++] = tris[j];
		}
	}
	
	return n;
}






