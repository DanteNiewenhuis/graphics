#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "types.h"
#include "bvh.h"

unsigned long long num_rays_shot;
unsigned long long num_shadow_rays_shot;
unsigned long long num_triangles_tested;
unsigned long long num_bboxes_tested;

int     find_first_intersection(intersection_point *ip,
            vec3 ray_origin, vec3 ray_direction);
int     bvh_triangle_helper(intersection_point *ip, vec3 ray_origin, 
                        vec3 ray_direction, bvh_node* node, float t0, float t1, float* closest_distance);

int     shadow_check(vec3 ray_origin, vec3 ray_direction);

#endif
