/* Computer Graphics and Game Technology, Assignment Ray-tracing
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
#include <stdio.h>
#include "intersection.h"
#include "v3math.h"
#include "constants.h"
#include "scene.h"
#include "bvh.h"

// A few counters for gathering statistics on the number and types
// of ray shot

// The total number of rays
unsigned long long num_rays_shot = 0;

// Number of shadow rays
unsigned long long num_shadow_rays_shot = 0;

// Number of triangles tested for intersection
unsigned long long num_triangles_tested = 0;

// Number of bounding boxes tested for intersection
unsigned long long num_bboxes_tested = 0;

// Forward declarations

static int  find_first_intersected_bvh_triangle(intersection_point* ip,
                vec3 ray_origin, vec3 ray_direction);

// Checks if the given triangle is intersected by ray with given
// origin and direction.
//
// Returns 1 if there is an intersection, or 0 otherwise.
//
// When an intersection is found the fields of 'ip' will be filled in
// with the relevant values.
//
// Note: this routine does NOT return an intersection for triangles
// whose back side faces the ray (by definition a triangle normal
// points to the triangle's front side).
// I.e. we do back-face culling here ...
//
// Code based on Moller & Trumbore, 1997, "Fast, minimum storage
// ray/triangle intersection"

static int
ray_intersects_triangle(intersection_point* ip, triangle tri,
    vec3 ray_origin, vec3 ray_direction)
{
    vec3    edge1, edge2;
    vec3    tvec, pvec, qvec;
    double  det, inv_det;
    double  t, u, v;        // u, v are barycentric coordinates
    // t is ray parameter

    num_triangles_tested++;

    edge1 = v3_subtract(scene_vertices[tri.v[1]], scene_vertices[tri.v[0]]);
    edge2 = v3_subtract(scene_vertices[tri.v[2]], scene_vertices[tri.v[0]]);

    pvec = v3_crossprod(ray_direction, edge2);

    det = v3_dotprod(edge1, pvec);

    if (det < 1.0e-6)
        return 0;

    tvec = v3_subtract(ray_origin, scene_vertices[tri.v[0]]);

    u = v3_dotprod(tvec, pvec);
    if (u < 0.0 || u > det)
        return 0;

    qvec = v3_crossprod(tvec, edge1);

    v = v3_dotprod(ray_direction, qvec);
    if (v < 0.0 || u+v > det)
        return 0;

    t = v3_dotprod(edge2, qvec);

    if (t < 0.0)
        return 0;

    inv_det = 1.0 / det;
    t *= inv_det;
    u *= inv_det;
    v *= inv_det;

    // We have a triangle intersection!
    // Return the relevant intersection values.

    // Compute the actual intersection point
    ip->t = t;
    ip->p = v3_add(ray_origin, v3_multiply(ray_direction, t));

    // Compute an interpolated normal for this intersection point, i.e.
    // we use the barycentric coordinates as weights for the vertex normals
    ip->n = v3_normalize(v3_add(
        v3_add(
            v3_multiply(tri.vn[0], 1.0-u-v),
            v3_multiply(tri.vn[1], u)
        ),
        v3_multiply(tri.vn[2], v)));

    ip->i = v3_normalize(v3_negate(ray_direction));
    ip->material = tri.material;

    return 1;
}

// Check if the given sphere is intersected by the given ray.
// See Shirley et.al., section 10.3.1
// Returns 1 if there is an intersection (and sets the appropriate
// fields of ip), or 0 otherwise.
static int
ray_intersects_sphere(intersection_point* ip, sphere sph,
    vec3 ray_origin, vec3 ray_direction)
{
    float   A, B, C, D;
    vec3    diff;
    float   t_hit;

    A = v3_dotprod(ray_direction, ray_direction);

    diff = v3_subtract(ray_origin, sph.center);
    B = 2.0 * v3_dotprod(diff, ray_direction);
    C = v3_dotprod(diff, diff) - sph.radius * sph.radius;

    D = B*B - 4*A*C;

    if (D < 0.0)
        return 0;

    D = sqrt(D);

    // We're only interested in the first hit, i.e. the one with
    // the smallest t_hit, so we check -B-D first, followed by -B+D

    t_hit = (-B - D)/(2*A);

    if (t_hit < 0.0)
    {
        t_hit = (-B + D)/(2*A);
        if (t_hit < 0.0)
            return 0;
    }

    ip->t = t_hit;
    ip->p = v3_add(ray_origin, v3_multiply(ray_direction, t_hit));
    ip->n = v3_normalize(v3_subtract(ip->p, sph.center));
    ip->i = v3_normalize(v3_negate(ray_direction));
    ip->material = sph.material;

    return 1;
}











// Checks for an intersection of the given ray with the triangles
// stored in the BVH.
//
// Returns 1 if there is an intersection. The fields of 'ip' will be
// set to the relevant values. The intersection returned
// will be the one closest to the ray origin.
//
// Returns 0 if there are no intersections

int traverse_bvh(intersection_point* ip, bvh_node* node, 
				vec3 ray_origin, vec3 ray_direction, float* t0, float* t1) 
{
	// If node is a leaf, then update t and tri if t_new < t for all node tris.
	if (node->is_leaf) {
		// Get info about triangles in the leaf node.
		int n_tris = leaf_node_num_triangles(node);
		triangle* tris = leaf_node_triangles(node);
		intersection_point ip2;
		
		// Find intersections between ray and triangle for each triangle.
		for (int i = 0; i < n_tris; i++) {
        	if (ray_intersects_triangle(&ip2, tris[i], ray_origin, ray_direction)) {
        		// If we find a triangle closer than before, update global ip.
        		if (ip2.t < ip->t) {
        			*ip = ip2;
        			
        			// Update t1 such that other nodes with an intersection with 
        			// a higher t value are discarded later on.
        			*t1 = ip2.t;
        		}
        	}
		}
		return 0;
	}
	
	// If not a leaf, get its children.
	bvh_node* left_node = inner_node_left_child(node);
	bvh_node* right_node = inner_node_right_child(node);
	
	// TODO: pick most likely of the two nodes first!
	float t_min_left, t_max_left, t_min_right, t_max_right;
	int left_inter = bbox_intersect(&t_min_left, &t_max_left, left_node->bbox, ray_origin, ray_direction, *t0, *t1);
	int right_inter = bbox_intersect(&t_min_right, &t_max_right, right_node->bbox, ray_origin, ray_direction, *t0, *t1);
	
	// If there is an intersection with the child boxes in range t0 to t1, then 
	// call traverse_bvh on that node as the root.
	if (left_inter && right_inter && t_min_left < t_min_right) {
		traverse_bvh(ip, left_node, ray_origin, ray_direction, t0, t1);
		if (*t1 > t_min_right) {
		    traverse_bvh(ip, right_node, ray_origin, ray_direction, t0, t1);
		}
		
	} else if (left_inter && right_inter && t_min_left >= t_min_right) {
		traverse_bvh(ip, right_node, ray_origin, ray_direction, t0, t1);
		if (*t1 > t_min_left) {
		    traverse_bvh(ip, left_node, ray_origin, ray_direction, t0, t1);
		}
	} else if (left_inter) {
	    traverse_bvh(ip, left_node, ray_origin, ray_direction, t0, t1);
	    
	} else if (right_inter) {
	    traverse_bvh(ip, right_node, ray_origin, ray_direction, t0, t1);
	}
	
	return 0;
}

static int
find_first_intersected_bvh_triangle(intersection_point* ip, vec3 ray_origin, 
									vec3 ray_direction)
{
    float t0 = 0, t1 = C_INFINITY;
    float t_min, t_max;
    
    ip->t = C_INFINITY;
    
    // Check for intersection with root node.
    if (!bbox_intersect(&t_min, &t_max, bvh_root->bbox, ray_origin, ray_direction, t0, t1)) {
        return 0;
    }
    
    // Traverse the remainder of tree.
    traverse_bvh(ip, bvh_root, ray_origin, ray_direction, &t0, &t1);
    
    // If t is still infinity (not updated), then no triangle intersection.
    if (ip->t < C_INFINITY) {
    	return 1;
    } 
   	return 0;
}













// Returns the nearest hit of the given ray with objects in the scene
// (either a sphere or a triangle).
//
// Returns 1 and sets the intersection point values if there
// is an intersection, returns 0 otherwise.
int
find_first_intersection(intersection_point *ip, vec3 ray_origin, vec3 ray_direction)
{
    int     have_hit;
    float   t_nearest;
    intersection_point  ip2;

    num_rays_shot++;

    // We have found no hit yet
    t_nearest = C_INFINITY;
    have_hit = 0;

    // First check against spheres in the scene
    for (int s = 0; s < scene_num_spheres; s++)
    {
        // We need a second set of p and n variables, as there's the
        // possibility that we'll overwrite a closer intersection already
        // found
        if (ray_intersects_sphere(&ip2, scene_spheres[s], ray_origin, ray_direction))
        {
            if (ip2.t < t_nearest)
            {
                *ip = ip2;
                t_nearest = ip2.t;
                have_hit = 1;
            }
        }
    }

    // Then check against triangles in the scene

    if (use_bvh)
    {
        // Use the BVH to speed up intersection testing
        if (find_first_intersected_bvh_triangle(&ip2, ray_origin, ray_direction))
        {
            if (ip2.t < t_nearest)
            {
                *ip = ip2;
                t_nearest = ip2.t;
                have_hit = 1;
            }
        }
    }
    else
    {
        // Simply iterate over all the triangles in the scene and check for intersection
        for (int t = 0; t < scene_num_triangles; t++)
        {
            if (ray_intersects_triangle(&ip2, scene_triangles[t], ray_origin, ray_direction))
            {
                if (ip2.t < t_nearest)
                {
                    *ip = ip2;
                    t_nearest = ip2.t;
                    have_hit = 1;
                }
            }
        }
    }

    return have_hit;
}

// Optimized routine for tracing a shadow ray.
//
// This routine doesn't return the nearest intersection, but simply
// checks if there is any intersection.
int
shadow_check(vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    num_rays_shot++;
    num_shadow_rays_shot++;

    for (int s = 0; s < scene_num_spheres; s++)
    {
        if (ray_intersects_sphere(&ip, scene_spheres[s], ray_origin, ray_direction))
            return 1;
    }

    if (use_bvh)
    {
        // Use the BVH for speedy intersection testing
        if (find_first_intersected_bvh_triangle(&ip, ray_origin, ray_direction))
            return 1;
    }
    else
    {
        // Simply iterate over all the triangles in the scene and check for intersection
        for (int t = 0; t < scene_num_triangles; t++)
        {
            if (ray_intersects_triangle(&ip, scene_triangles[t], ray_origin, ray_direction))
                return 1;
        }
    }

    return 0;
}

