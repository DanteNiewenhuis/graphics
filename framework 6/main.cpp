/* Computer Graphics and Game Technology, Assignment Box2D game
 *
 * Student name ....
 * Student email ...
 * Collegekaart ....
 * Date ............
 * Comments ........
 */

#include <cstdio>
#include <math.h>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <Box2D/Box2D.h>

#include "levels.h"

unsigned int reso_x = 800, reso_y = 600;
const float world_x = 8.f, world_y = 6.f;
const float framerate = 1.0f / 60.0f;
const int num_iters = 3;

int last_time;
int frame_count;


/**********************************
 * Game State Variables
 **********************************/

// Define 'entities' for Box2D.
b2World* world;
b2Body* ball;
float gravity_force = -2.0;

// Define current state of the game (paused?, level?, clicks?, etc.)
int current_level = 0;
bool pause_game = true;
unsigned int num_levels;
level_t *levels;

int num_clicks = 0;
float clicks[4][2];

// Define properties of the ball.
float ball_radius = 0.3;
float ball_density = 0.2;
float ball_friction = 0.2;
int ball_res = 64;
float ball_color[3] = {1.0, 0.0, 0.0};
GLuint ball_vbo;

// Define properties of the static and dynamic objects of the level.
float obj_density = 0.2;
float obj_friction = 0.2;
float obj_color[3] = {0.0, 1.0, 0.0};
std::vector<GLuint> obj_vbos;
std::vector<int> obj_num_verts;
std::vector<b2Body*> obj_bodies;

// Define properties of finish line (modelled as physics-less circle).
float finish_color[3] = {0.0, 0.0, 1.0};
float finish_radius = 0.15;
int finish_res = 32;
GLuint finish_vbo;
point_t finish_pos;


/**********************************
 * Level Initialization Functions.
 **********************************/
 
void init_ball(level_t level) {
	// Set up ball body.
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(level.start.x, level.start.y);
	ball = world->CreateBody(&ballBodyDef);
	
	// Set up ball shape.
	b2CircleShape ballShape;
	ballShape.m_radius = ball_radius;
	
	// Set up ball fixture.
	b2FixtureDef ballFixtureDef;
	ballFixtureDef.shape = &ballShape;
	ballFixtureDef.density = ball_density;
	ballFixtureDef.friction = ball_friction;
	ball->CreateFixture(&ballFixtureDef);
	
	// Initialize array with 2D vertices of ball.
	GLfloat *ball_verts = new GLfloat[2 * ball_res];
	ball_verts[0] = 0.0f;
	ball_verts[1] = 0.0f;
	for (int i = 2; i < ball_res * 2; i += 2) {
		float angle = 2 * M_PI * i / ball_res; 
		ball_verts[i] = ball_radius * cos(angle);
		ball_verts[i+1] = ball_radius * sin(angle);
	}
			  
	// Allocate a small piece of memory for ball on GPU (VBO).
	glGenBuffers(1, &ball_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
	glBufferData(GL_ARRAY_BUFFER, ball_res * 2 * sizeof(GLfloat), 
				 ball_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void init_objects(level_t level) {
	// Clear previous level (if it exists).
	obj_vbos.clear();
	obj_num_verts.clear();
	obj_bodies.clear();
		
	// Loop through objects in level.
	for (unsigned int i = 0; i < level.num_polygons; i++) {
		poly_t poly = level.polygons[i];
		point_t pos = poly.position;
		
		// Convert point_t array to b2Vec2 array.
		b2Vec2 *pts = new b2Vec2[poly.num_verts];
		for (unsigned int j = 0; j < poly.num_verts; j++) {
			pts[j] = b2Vec2(poly.verts[j].x, poly.verts[j].y);
		}
		
		// Create dynamic or static body.
		b2BodyDef objBodyDef;
		if (poly.is_dynamic) objBodyDef.type = b2_dynamicBody;
		objBodyDef.position.Set(pos.x, pos.y);
		b2Body* objBody = world->CreateBody(&objBodyDef);
			
		// Set up obj shape.
		b2PolygonShape objShape;
		objShape.Set(pts, poly.num_verts);
			
		// Set up obj fixture.
		if (poly.is_dynamic) {
			b2FixtureDef objFixtureDef;
			objFixtureDef.shape = &objShape;
			objFixtureDef.density = obj_density;
			objFixtureDef.friction = obj_friction;
				
			objBody->CreateFixture(&objFixtureDef);
		} else {
			objBody->CreateFixture(&objShape, 0.0f);
		}
		
		// Flatten 2D array of point_t structs to 1D array of floats.
		GLfloat *obj_verts = new GLfloat[2 * poly.num_verts];
		int n = 0;
		for (unsigned int j = 0; j < poly.num_verts; j++, n++) {
			obj_verts[n] = poly.verts[j].x;
			obj_verts[++n] = poly.verts[j].y;
		}
		
		
		// Allocate a small piece of memory for object on GPU (VBO).
		GLuint obj_vbo;
		glGenBuffers(1, &obj_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, obj_vbo);
		glBufferData(GL_ARRAY_BUFFER, poly.num_verts * 2 * sizeof(GLfloat), 
					 obj_verts, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// Store new vbo-identifier, number of verts and bodies for later.
		obj_vbos.push_back(obj_vbo);
		obj_num_verts.push_back(poly.num_verts);
		obj_bodies.push_back(objBody);
	}
} 


void init_finish(level_t level) {
	// Set finish position.
	finish_pos = level.end;
	
	// Initialize array with 2D vertices of ball.
	GLfloat *finish_verts = new GLfloat[2 * finish_res];
	finish_verts[0] = 0.0f;
	finish_verts[1] = 0.0f;
	for (int i = 2; i < finish_res * 2; i += 2) {
		float angle = 2 * M_PI * i / finish_res; 
		finish_verts[i] = finish_radius * cos(angle);
		finish_verts[i+1] = finish_radius * sin(angle);
	}
			  
	// Allocate a small piece of memory for ball on GPU (VBO).
	glGenBuffers(1, &finish_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, finish_vbo);
	glBufferData(GL_ARRAY_BUFFER, finish_res * 2 * sizeof(GLfloat), 
				 finish_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void load_world(unsigned int level_id) {
	// Get level struct from levels array.
	level_t level = levels[level_id];
	
	// Initialize the world.
	b2Vec2 gravity(0.0, gravity_force);
	world = new b2World(gravity);
	
	// Initialize each object component of the level.
	init_ball(level);
	init_objects(level);
	init_finish(level);
	
	// Keep the game paused while user sets up his level.
	pause_game = true;
}


int is_counterclockwise(float a[2], float b[2], float c[2]) {
	float area = (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]);
	return area < 0;
}


void add_new_object(void) {
	// Check whether recorded four points are wounded counter-clockwise.
	// in terms of their triangle fan specification ((0, 1, 2) and (0, 2, 3)).
	int area1 = is_counterclockwise(clicks[0], clicks[1], clicks[2]);
	int area2 = is_counterclockwise(clicks[0], clicks[2], clicks[3]);
	
	// Swap if necessary.
	if (!area1 && !area2) {
		std::swap(clicks[1][0], clicks[3][0]);
		std::swap(clicks[1][1], clicks[3][1]);
	} else if (!area1) {
		// TODO: other case
	} else if (!area2) {
		// TODO: other case
	} 
	
	// Create b2Vec2 array with (local) vertex positions for Box2D.
	float mean_x = (clicks[0][0] + clicks[1][0] + clicks[2][0] + clicks[3][0]) / 4;
	float mean_y = (clicks[0][1] + clicks[1][1] + clicks[2][1] + clicks[3][1]) / 4;
	b2Vec2 pts[4] = {b2Vec2(clicks[0][0] - mean_x, clicks[0][1] - mean_y),
					 b2Vec2(clicks[1][0] - mean_x, clicks[1][1] - mean_y),
					 b2Vec2(clicks[2][0] - mean_x, clicks[2][1] - mean_y),
					 b2Vec2(clicks[3][0] - mean_x, clicks[3][1] - mean_y)};
					 
	// Set up dynamic body, shape and ficture.
	b2BodyDef objBodyDef;
	objBodyDef.type = b2_dynamicBody;
	objBodyDef.position.Set(mean_x, mean_y);
	b2Body* objBody = world->CreateBody(&objBodyDef);
			
	// Set up obj shape.
	b2PolygonShape objShape;
	objShape.Set(pts, 4);
			
	// Set up obj fixture.
	b2FixtureDef objFixtureDef;
	objFixtureDef.shape = &objShape;
	objFixtureDef.density = obj_density;
	objFixtureDef.friction = obj_friction;			
	objBody->CreateFixture(&objFixtureDef);
	
	// Flatten 2D array of point_t structs to 1D array of floats.
	GLfloat *obj_verts = new GLfloat[8];
	int n = 0;
	for (unsigned int j = 0; j < 4; j++, n++) {
		obj_verts[n] = pts[j].x;
		obj_verts[++n] = pts[j].y;
	}
		
	// Allocate a small piece of memory for object on GPU (VBO).
	GLuint obj_vbo;
	glGenBuffers(1, &obj_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, obj_vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), obj_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	// Store new vbo-identifier, number of verts and bodies for later.
	obj_vbos.push_back(obj_vbo);
	obj_num_verts.push_back(4);
	obj_bodies.push_back(objBody);
	
}


/**********************************
 * Level Drawing Functions.
 **********************************/

void draw_ball(void) {
	// Push translation matrix onto matrix stack.
	glPushMatrix();
    glTranslatef(ball->GetPosition().x, ball->GetPosition().y, 0);	
        	
	// Draw ball on screen.
	glColor3f(ball_color[0], ball_color[1], ball_color[2]);
	glBindBuffer(GL_ARRAY_BUFFER, ball_vbo);
	glVertexPointer(2, GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_FAN, 0, ball_res);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Pop translation matrix to return to original state.
	glPopMatrix();
}


void draw_objects(void) {  
    for (unsigned int i = 0; i < obj_bodies.size(); i++) {
    	// Get body shape type.
    	b2Body* body = obj_bodies[i];
    	int shapeType = body->GetFixtureList()->GetShape()->GetType();
    	
    	// If of type b2PolygonShape, then draw object as polygon.
    	if (shapeType == 2) {   		
    		// Push translation and rotation matrix onto matrix stack.
			glPushMatrix();
			glTranslatef(body->GetPosition().x, body->GetPosition().y, 0);
			glRotatef(body->GetAngle(), 0, 0, 1);
					
			// Draw ball on screen.
			glColor3f(obj_color[0], obj_color[1], obj_color[2]);
			glBindBuffer(GL_ARRAY_BUFFER, obj_vbos[i]);
			glVertexPointer(2, GL_FLOAT, 0, NULL);
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glDrawArrays(GL_TRIANGLE_FAN, 0, obj_num_verts[i]);
			glDisableClientState(GL_VERTEX_ARRAY);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			// Pop translation matrix to return to original state.
			glPopMatrix();
    	}
    	
    	// Go to next object body.
    	body = body->GetNext();
    }
}


void draw_finish(void) {
	// Push translation matrix onto matrix stack.
	glPushMatrix();
    glTranslatef(finish_pos.x, finish_pos.y, 0);	
        	
	// Draw finish circle on screen.
	glColor3f(finish_color[0], finish_color[1], finish_color[2]);
	glBindBuffer(GL_ARRAY_BUFFER, finish_vbo);
	glVertexPointer(2, GL_FLOAT, 0, NULL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_TRIANGLE_FAN, 0, finish_res);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Pop translation matrix to return to original state.
	glPopMatrix();
}


/**********************************
 * Game Logic
 **********************************/
 
 
// Called to update the game state.
void update_state(void) {
    int time = glutGet(GLUT_ELAPSED_TIME);
    int frametime = time - last_time;
    frame_count++;

	// If ball goes out of the window, then restart.
	float x = ball->GetPosition().x;
	float y = ball->GetPosition().y;
	if (x < 0 || x > world_x || y < 0 || y > world_y) {
		load_world(current_level);
	}
	
	// If ball reaches finish, then go to next level.
	float dist = sqrt(pow(finish_pos.x - x, 2) + pow(finish_pos.y - y, 2));
	if (dist < finish_radius + ball_radius) {
		load_world(++current_level);
	}
    
    // Draw the level finish, ball and objects in that order.
    glColor3f(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    draw_finish();
    draw_ball();
    draw_objects();

    glutSwapBuffers();

	// Show level info and framerate.
    if (frametime >= 1000)
    {
        char window_title[128];
        snprintf(window_title, 128,
                "Box2D: %f fps, level %d/%d",
                frame_count / (frametime / 1000.f), current_level+1, num_levels);
        glutSetWindowTitle(window_title);
        last_time = time;
        frame_count = 0;
    }
    
    // Tick if game is not paused.
    if (!pause_game) world->Step(framerate, num_iters, num_iters);
}


/**********************************
 * User Interaction.
 **********************************/


void resize_window(int width, int height) {
    glViewport(0, 0, width, height);
    reso_x = width;
    reso_y = height;
}


void key_pressed(unsigned char key, int x, int y) {
    switch (key)
    {
        case 27: // Esc
        case 'q':
            exit(0);
            break;
        case 's':
            pause_game = false;
            break;
        default:
            break;
    }
}


void mouse_clicked(int button, int state, int x, int y) {
    // If user presses the left mouse button, then record mouse position.
	if (state == 0 && button == 0) {
		clicks[num_clicks][0] = world_x * x / reso_x;
		clicks[num_clicks][1] = world_y - (world_y * y / reso_y);
		num_clicks++;
		
		// If four points have been gathered, add a new object to the level.
		if (num_clicks == 4) {
			add_new_object();
			num_clicks = 0;
		}
	}
}


void mouse_moved(int x, int y) {
    return;
}


/**********************************
 * Main Loop
 **********************************/
 
 
int main(int argc, char **argv) {
    // Create an OpenGL context and a GLUT window.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(reso_x, reso_y);
    glutCreateWindow("Box2D");

    glewInit();

    // Bind all GLUT events do callback function.
    glutDisplayFunc(&update_state);
    glutIdleFunc(&update_state);
    glutReshapeFunc(&resize_window);
    glutKeyboardFunc(&key_pressed);
    glutMouseFunc(&mouse_clicked);
    glutMotionFunc(&mouse_moved);
    glutPassiveMotionFunc(&mouse_moved);

    // Initialise the matrices so we have an orthogonal world.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, world_x, 0, world_y, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Read the levels into a bunch of structs.
    num_levels = load_levels(&levels);
    printf("Loaded %d levels.\n", num_levels);
    
    // Load the first level.
    load_world(current_level);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    frame_count = 0;
    glutMainLoop();

    return 0;
}
