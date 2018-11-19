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
#include <stdlib.h>
#include "shaders.h"
#include "perlin.h"
#include "v3math.h"
#include "intersection.h"
#include "scene.h"
#include "quat.h"
#include "constants.h"

float min(float x, float y) {
    if (x < y) {
        return x;
    }
    return y;
}

float max(float x, float y) {
    if (x > y) {
        return x;
    }
    return y;
}

vec3 clip(vec3 v) {
    v.x = min(v.x, 1.0);
    v.y = min(v.y, 1.0);
    v.z = min(v.z, 1.0);
    return v;
}

// shade_constant()
//
// Always return the same color. This shader does no real computations
// based on normal, light position, etc. As such, it merely creates
// a "silhouette" of an object.

vec3
shade_constant(intersection_point ip)
{
    return v3_create(1, 0, 0);
}

vec3
shade_matte(intersection_point ip)
{
    vec3 noise;
    
    float I = 0, I_tmp = 0;
    for (int i = 0; i < scene_num_lights; i++) {
        // For each light, get its position and intensity.
        light li = scene_lights[i];
        
        // Determine unit vector pointing towards the lamp.
        vec3 l = v3_normalize(v3_subtract(li.position, ip.p)); 
        
        // Check whether this l vector is obstructed by an object.
        noise = v3_multiply(l, 0.001);
        if (shadow_check(v3_add(ip.p, noise), l)) {
            continue;
        }
        
        // Calculate intensity of surface for this one light.
        I_tmp = max(0, v3_dotprod(ip.n, l));
        
        // Accumulate light over all lights.
        I += I_tmp * li.intensity;    
    }
    
    // Add ambient component
    I += scene_ambient_light;
    
    // Combine with diffuse color coefficients
    vec3 c_f = v3_multiply(v3_create(1, 1, 1), I);
    
    // Clip final color if values exceed 1.0
    return clip(c_f);
}

vec3
shade_blinn_phong(intersection_point ip)
{
	vec3 noise;
    vec3 cd = v3_create(1, 0, 0);
    vec3 cs = v3_create(1, 1, 1);
    
    float alpha = 50, kd = 0.8, ks = 0.5;
    
    // Accumulate diffuse and specular components for each light.
    float I_diff = 0, I_spec = 0, I_tmp = 0;
    for (int i = 0; i < scene_num_lights; i++) {
        light li = scene_lights[i];
        
        // Determine vector l pointing towards the lamp.
        vec3 l = v3_subtract(li.position, ip.p); 
        l = v3_normalize(l);
        
        // Check whether this l vector is obstructed by an object.
        noise = v3_multiply(l, 0.001);
        if (shadow_check(v3_add(ip.p, noise), l)) {
            continue;
        }
        
        // Calculate diffuse intensity of surface for this one light
        // making sure not to have negative values.
        I_tmp = max(0, v3_dotprod(ip.n, l));
        
        // Accumulate light over all lights.
        I_diff += I_tmp * li.intensity;  
        
        // Calculate half vector
        vec3 h = v3_normalize(v3_add(ip.i, l));
        
        // determine Phong component.
        I_spec += pow(v3_dotprod(ip.n, h), alpha) * li.intensity;
    }
    
    // Add kd and ambient component
    I_diff = kd*I_diff + scene_ambient_light;
    I_spec = ks*I_spec; 
    
    // Combine specular with diffuse color coefficients
    vec3 cf = v3_add(v3_multiply(cd, I_diff), v3_multiply(cs, I_spec));
    return clip(cf);
}

vec3
shade_reflection(intersection_point ip)
{
    vec3 noise;
    
    float I = 0, I_tmp = 0;
    for (int i = 0; i < scene_num_lights; i++) {
        // For each light, get its position and intensity.
        light li = scene_lights[i];
        
        // Determine unit vector pointing towards the lamp.
        vec3 l = v3_normalize(v3_subtract(li.position, ip.p)); 
        
        // Check whether this l vector is obstructed by an object.
        noise = v3_multiply(l, 0.001);
        if (shadow_check(v3_add(ip.p, noise), l)) {
            continue;
        }
        
        // Calculate intensity of surface for this one light.
        I_tmp = max(0, v3_dotprod(ip.n, l));
        
        // Accumulate light over all lights.
        I += I_tmp * li.intensity;    
    }
    
    // Add ambient component
    I += scene_ambient_light;
    
    // Determine reflection ray
    float dot = v3_dotprod(ip.i, ip.n);
    vec3 r = v3_multiply(ip.n, 2*dot);
    r = v3_subtract(r, ip.i);
    
    vec3 z = v3_create(ip.p.x, ip.p.y, ip.p.z);
    
    vec3 c_ref = ray_color(0, v3_create(1, 1, 1), r);
    
    
    // Combine with diffuse color coefficients
    vec3 c_f = v3_multiply(v3_create(1, 1, 1), I);
    
    // Clip final color if values exceed 1.0
    return clip(c_f);
}

// Returns the shaded color for the given point to shade.
// Calls the relevant shading function based on the material index.
vec3
shade(intersection_point ip)
{
  switch (ip.material)
  {
    case 0:
      return shade_constant(ip);
    case 1:
      return shade_matte(ip);
    case 2:
      return shade_blinn_phong(ip);
    case 3:
      return shade_reflection(ip);
    default:
      return shade_constant(ip);

  }
}

// Determine the surface color for the first object intersected by
// the given ray, or return the scene background color when no
// intersection is found
vec3
ray_color(int level, vec3 ray_origin, vec3 ray_direction)
{
    intersection_point  ip;

    // If this ray has been reflected too many times, simply
    // return the background color.
    if (level >= 3)
        return scene_background_color;

    // Check if the ray intersects anything in the scene
    if (find_first_intersection(&ip, ray_origin, ray_direction))
    {
        // Shade the found intersection point
        ip.ray_level = level;
        return shade(ip);
    }

    // Nothing was hit, return background color
    return scene_background_color;
}
