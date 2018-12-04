/* Computer Graphics, Assignment, Texture Mapping
 *
 * Filename ........ geometry.c
 * Description .....
 * Date ............ 22.11.2007
 * Created by ...... Paul Melis
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 *
 * (always fill in these fields before submitting!!)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/glew.h>
#else
#include <GL/glew.h>
#endif

#include "geometry.h"
#include "polys.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

static void
setSpherePoint(GLfloat *p, GLfloat* n, GLfloat* t, int latitude, int longitude,
    double sx, double sy, double sz, double ox, double oy, double oz)
{
    double  dToR = M_PI / 180.0;
    double  len;

    // Set vertex position
    p[0] = ox + sin(longitude * dToR) * cos(latitude * dToR) * sx;
    p[1] = oy + sin(latitude * dToR) * sy;
    p[2] = oz + cos(longitude * dToR) * cos(latitude * dToR) * sz;

    // Set texture coordinate
    t[0] = 0.0;
    t[1] = 0.0;

    // calculate normal, this actually doesn't take the sphere size
    // per axis into account, but should still be usable
    n[0] = p[0] - ox;
    n[1] = p[1] - oy;
    n[2] = p[2] - oz;

    len = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
    n[0] /= len;
    n[1] /= len;
    n[2] /= len;
}

void
createSphere(polys * list, double sx, double sy, double sz, double ox,
           double oy, double oz, double r, double g, double b)
{
    int     latitude, longitude;
    poly    p;

    // prepare poly variable, as the color values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    GLuint buffers[3];

    GLfloat vertices[12];
    GLfloat normals[12];
    GLfloat tcoords[8];

    // sweep over sphere's surface and generate polys
    for (latitude = -90; latitude < 90; latitude += 10)
    {
        for (longitude = 0; longitude < 360; longitude += 10)
        {
            // Insert the values into the buffers at the right offsets.
            setSpherePoint(&vertices[0], &normals[0], &tcoords[0],
                latitude, longitude, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&vertices[3], &normals[3], &tcoords[2],
                latitude+10, longitude, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&vertices[6], &normals[6], &tcoords[4],
                latitude+10, longitude+10, sx, sy, sz, ox, oy, oz);
            setSpherePoint(&vertices[9], &normals[9], &tcoords[6],
                latitude, longitude+10, sx, sy, sz, ox, oy, oz);

            // Generate the buffers and copy the data to the GPU.
            glGenBuffers(3, buffers);
            p.vert_id = buffers[0];
            p.normal_id = buffers[1];
            p.texbuffer_id = buffers[2];

            glBindBuffer(GL_ARRAY_BUFFER, p.vert_id);
            glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.normal_id);
            glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.texbuffer_id);
            glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tcoords, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            AddPolyToPolylist(list, p);
        }
    }
}

// Calculate the coordinates, normal vector and texture coordinates for
// a hemisphere point at the given latitude and longitude (in degrees).
// The radius of the hemisphere is s, the center point (at the base of the
// hemisphere) is ox,oy,oz.
static void
setHemispherePoint(GLfloat *p, GLfloat* n, GLfloat* t, int latitude, int longitude,
    double s, double ox, double oy, double oz)
{
    double  dToR = M_PI / 180.0;
    double  len;

    // Set vertex position
    p[0] = ox + sin(longitude * dToR) * cos(latitude * dToR) * s;
    p[1] = oy + sin(latitude * dToR) * s;
    p[2] = oz + cos(longitude * dToR) * cos(latitude * dToR) * s;

    // Set texture coordinate
    t[0] = 0.0;
    t[1] = 0.0;

    // calculate normal
    n[0] = p[0] - ox;
    n[1] = p[1] - oy;
    n[2] = p[2] - oz;

    len = n[0]*n[0] + n[1]*n[1] + n[2]*n[2];
    n[0] /= len;
    n[1] /= len;
    n[2] /= len;
}

void
createHemisphere(polys * list, double s, double ox, double oy, double oz,
    double r, double g, double b)
{
    int     latitude, longitude;
    poly    p;

    // prepare poly, as these values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    GLuint buffers[3];

    GLfloat vertices[12];
    GLfloat normals[12];
    GLfloat tcoords[8];

    // sweep over sphere's surface and generate polys
    for (latitude = 0; latitude < 90; latitude += 10)
    {
        for (longitude = 0; longitude < 360; longitude += 10)
        {
            // Insert the values into the buffers at the right offsets.
            setHemispherePoint(&vertices[0], &normals[0], &tcoords[0],
                latitude, longitude, s, ox, oy, oz);
            setHemispherePoint(&vertices[3], &normals[3], &tcoords[2],
                latitude+10, longitude, s, ox, oy, oz);
            setHemispherePoint(&vertices[6], &normals[6], &tcoords[4],
                latitude+10, longitude+10, s, ox, oy, oz);
            setHemispherePoint(&vertices[9], &normals[9], &tcoords[6],
                latitude, longitude+10, s, ox, oy, oz);

            // Generate the buffers and copy the data to the GPU.
            glGenBuffers(3, buffers);
            p.vert_id = buffers[0];
            p.normal_id = buffers[1];
            p.texbuffer_id = buffers[2];

            glBindBuffer(GL_ARRAY_BUFFER, p.vert_id);
            glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.normal_id);
            glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.texbuffer_id);
            glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tcoords, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            AddPolyToPolylist(list, p);
        }
    }
}

// Calculate the coordinates, normal vector and texture coordinates for
// a cylinder point at the given height and longitude (in degrees).
// The radius of the hemisphere is s, the center point (at the base of the
// hemisphere) is ox,oy,oz.
static void
setCylinderPoint(GLfloat *p, GLfloat* n, GLfloat* t, int longitude, double height,
    double s, double ox, double oz)
{
    double  dToR = M_PI / 180.0;
    double  len;

    // Set vertex position
    p[0] = ox + sin(longitude * dToR) * s;
    p[1] = height;
    p[2] = oz + cos(longitude * dToR) * s;

    // Set texture coordinate
    t[0] = 0.0;
    t[1] = 0.0;

    // calculate normal
    n[0] = p[0] - ox;
    n[1] = 0.0;
    n[2] = p[2] - oz;

    len = n[0]*n[0] + n[2]*n[2];
    n[0] /= len;
    n[2] /= len;
}

// Create a cylinder along the Y axis whose base center point is
// at (ox, oy, oz), having the given radius and height.
// Use the given color for the generated polygons.
void
createCylinder(polys * list, double radius, double height,
    double ox, double oy, double oz,
    double r, double g, double b)
{
    int     longitude;
    poly    p;

    // prepare poly datastructure, as these values are the same for all generated polys
    p.points = 4;
    p.color[0] = r;
    p.color[1] = g;
    p.color[2] = b;
    p.color[3] = 0;

    GLuint buffers[3];

    GLfloat vertices[12];
    GLfloat normals[12];
    GLfloat tcoords[8];

    // sweep around cylinder axis
    for (longitude = 0; longitude < 360; longitude += 10)
    {
        // Insert the values into the buffers at the right offsets.
        setCylinderPoint(&vertices[0], &normals[0], &tcoords[0],
            longitude, oy, radius, ox, oz);
        setCylinderPoint(&vertices[3], &normals[3], &tcoords[2],
            longitude+10, oy, radius, ox, oz);
        setCylinderPoint(&vertices[6], &normals[6], &tcoords[4],
            longitude+10, oy+height, radius, ox, oz);
        setCylinderPoint(&vertices[9], &normals[9], &tcoords[6],
            longitude, oy+height, radius, ox, oz);

        // Generate the buffers and copy the data to the GPU.
        glGenBuffers(3, buffers);
        p.vert_id = buffers[0];
        p.normal_id = buffers[1];
        p.texbuffer_id = buffers[2];

        glBindBuffer(GL_ARRAY_BUFFER, p.vert_id);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, p.normal_id);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, p.texbuffer_id);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), tcoords, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        AddPolyToPolylist(list, p);
    }
}

// Read the vertex data for a single polygon and store it in 'p'.
// Called from loadPolygonalObject() after a 'p ...' line (+ color)
// has been read.
static void
readPolyVertices(poly *p, FILE *f, int num_vertices_to_read,
    int max_vertex_index, const vec3* all_verts, GLfloat* vertices,
    GLfloat* tcoords, GLuint texture_name)
{
    char    line[1024];
    int     num_read;
    int     pidx;
    int     vidx;
    float   s, t;

    p->points = num_vertices_to_read;

    num_read = 0;
    pidx = 0;
    while (num_read < num_vertices_to_read)
    {
        if (fgets(line, 1024, f) == NULL)
        {
            printf("Unexpected end-of-file reading poly vertices!\n");
            exit(-1);
        }

        if (sscanf(line, "%d %f %f\n", &vidx, &s, &t) == 3)
        {
            if (vidx > max_vertex_index)
            {
                printf("While reading %d polygon vertices, line:\n", num_vertices_to_read);
                printf("%s\n", line);
                printf("Vertex index %d is out of range (should be <= %d)\n", vidx, max_vertex_index);
                exit(-1);
            }

            //printf("%d %f %f %f %f %f\n",  vidx,  nx,  ny,  nz,  s,  t);

            vertices[pidx * 3] = all_verts[vidx].x;
            vertices[pidx * 3 + 1] = all_verts[vidx].y;
            vertices[pidx * 3 + 2] = all_verts[vidx].z;

            p->texture_id = texture_name;

            tcoords[pidx * 2] = s;
            tcoords[pidx * 2 + 1] = t;

            num_read++;
            pidx++;
        }
        else if (line[0] && line[0] != '#')
        {
            printf("Expected line declaring polygon vertex, got instead:\n");
            printf("%s\n", line);
            exit(-1);
        }

    }

}

// Read a polygonal object from a .OBJ file.
// Scale the input coordinates uniformly with a factor s followed by
// a translation (tx,ty,tz).
void
loadPolygonalObject(polys* list, const char *objfile, GLuint *texture_names,
    double s, double tx, double ty, double tz)
{
    FILE    *f;
    char    line[1024];
    float   x, y, z;
    vec3 *all_verts;
    GLuint buffers[3];
    int     num_vertices;
    int     num_vertices_allocated;
    poly    p;
    float   colr, colg, colb;
    int     num_poly_vertices, texture;
    int     i;
    float   len;
    vec3    v, w, n;

    f = fopen(objfile, "rt");

    if (!f)
    {
        fprintf(stderr, "loadPolygonalObject(): Could not open file '%s' for reading!\n", objfile);
        exit(-1);
    }

    printf("Reading %s\n", objfile);

    num_vertices = 0;
    num_vertices_allocated = 1024;
    all_verts = malloc(num_vertices_allocated * sizeof(vec3));

    while (fgets(line, 1024, f))
    {
        if (sscanf(line, "v %f %f %f\n", &x, &y, &z))
        {
            // vertex
            //printf("vertex: %f %f %f\n", x, y, z);

            // current vertex list is full, add more space
            if (num_vertices == num_vertices_allocated)
            {
                num_vertices_allocated = (int)(1.5 * num_vertices_allocated);
                all_verts = realloc(all_verts, num_vertices_allocated * sizeof(vec3));
                if (all_verts == NULL)
                {
                    printf("No memory to hold vertices!\n");
                    exit(-1);
                }
            }

            // store vertex, after scaling and translation
            all_verts[num_vertices].x = tx + s*x;
            all_verts[num_vertices].y = ty + s*y;
            all_verts[num_vertices].z = tz + s*z;

            num_vertices++;
        }
        else if (sscanf(line, "p %d %d\n", &num_poly_vertices, &texture))
        {
            if (num_poly_vertices < 3)
            {
                printf("In %s, line:\n", objfile);
                printf("%s\n", line);
                printf("Need at least 3 vertices to define a polygon!\n");
                exit(-1);
            }
            else if (num_poly_vertices > 4)
            {
                printf("In %s, line:\n", objfile);
                printf("%s\n", line);
                printf("Polygons may only consist of 3 or 4 vertices, sorry\n");
                printf("(this is due to OpenGL not handling concave polygons very well)\n");
                exit(-1);
            }

            // read polygon color
            if (fscanf(f, "%f %f %f\n", &colr, &colg, &colb) == EOF)
            {
                printf("Could not read polygon color!\n");
                exit(-1);
            }

            p.color[0] = colr;
            p.color[1] = colg;
            p.color[2] = colb;
            p.color[3] = 0.0;

            GLfloat vertices[3 * num_poly_vertices];
            GLfloat normals[3 * num_poly_vertices];
            GLfloat tcoords[2 * num_poly_vertices];

            // read vertices making up poly
            readPolyVertices(&p, f, num_poly_vertices, num_vertices-1, all_verts,
                             vertices, tcoords, texture_names[texture]);

            // Use first 3 polygon vertices to calculate the polygon normal vector
            // (i.e. assumes polygon is planar)

            v.x = vertices[6] - vertices[3];
            v.y = vertices[7] - vertices[4];
            v.z = vertices[8] - vertices[5];

            w.x = vertices[0] - vertices[3];
            w.y = vertices[1] - vertices[4];
            w.z = vertices[2] - vertices[5];

            n.x = v.y * w.z - v.z * w.y;
            n.y = v.z * w.x - v.x * w.z;
            n.z = v.x * w.y - v.y * w.x;

            len = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
            n.x /= len;
            n.y /= len;
            n.z /= len;

            // Set vertex normals to polygon normal

            for (i = 0; i < num_poly_vertices; i++) {
                normals[i * 3] = n.x;
                normals[i * 3 + 1] = n.y;
                normals[i * 3 + 2] = n.z;
            }

            // Generate the buffers and copy the data to the GPU.
            glGenBuffers(3, buffers);
            p.vert_id = buffers[0];
            p.normal_id = buffers[1];
            p.texbuffer_id = buffers[2];

            glBindBuffer(GL_ARRAY_BUFFER, p.vert_id);
            glBufferData(GL_ARRAY_BUFFER, num_vertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.normal_id);
            glBufferData(GL_ARRAY_BUFFER, num_vertices * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, p.texbuffer_id);
            glBufferData(GL_ARRAY_BUFFER, num_vertices * 2 * sizeof(GLfloat), tcoords, GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // Add polygon
            AddPolyToPolylist(list, p);
        }
    }

    //printf("%d vertices read\n", num_vertices);

    fclose(f);

    free(all_verts);
}
