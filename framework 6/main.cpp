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

/**********************************
 * Setup
 **********************************/

// Game properties (resolution, world size, framerate, colors, etc.).
unsigned int reso_x = 800, reso_y = 600;
const float world_x = 8.f, world_y = 6.f;
const float framerate = 1.0f / 60.0f;

// Physics parameters.
const int velocity_iters = 8;
const int position_iters = 3;
float gravity_force = 2.0;

// Colors.
float colors[4][3] = {{1.0, 0.0, 0.0},
		    		  {0.0, 1.0, 0.0},
		    		  {0.1, 1.0, 0.1},
		    		  {0.0, 0.0, 1.0}};

// References to important Box2D objects and OpenGL VBO identifiers.
b2World* world;
b2Body* ball;

GLuint ball_vbo;
GLuint finish_vbo;

std::vector<color3f> obj_colors;
std::vector<GLuint> obj_vbos;
std::vector<int> obj_num_verts;
std::vector<b2Body*> obj_bodies;

// Game state (paused?, level?, clicks?, etc.)
bool pause_game = true;
unsigned int current_level = 4;
unsigned int num_levels;
level_t *levels;
point_t finish_pos;

int num_clicks = 0;
float clicks[4][2];
int last_time;
int frame_count;

// variables for the scoring system
int amount_of_quadrilaterals = 0;
float amount_multipliers[6] = {1, 0.8, 0.6, 0.4, 0.3, 0.2};
float amount_multiplier;
int score = 0;
int level_scores[6] = {1000, 1500, 2000, 2500, 3000, 3500};
int level_score;
int death_penalty = 100;
bool finished = false;

float color_counter = 1;
float color_frames = 100;

// initialize the base color and the next color
float base_color[3] = {0, 0, 0};
float next_color[3] = {0.5, 0.5, 0.5};

// Characteristics of the ball.
float ball_radius = 0.3;
float ball_density = 1.0;
float ball_friction = 0.2;
int ball_res = 64;

// Characteristics of dynamic objects of the level.
float obj_density = 1.0;
float obj_friction = 0.2;

// Characteristics of finish line (physics-less circle).
float finish_radius = 0.15;
int finish_res = 32;


/**********************************
 * Level Initialization Functions.
 **********************************/
 
void init_ball(level_t level) {
	// Set up ball as Box2D object and keep a reference to the ball.
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(level.start.x, level.start.y);
	ball = world->CreateBody(&ballBodyDef);
	
	b2CircleShape ballShape;
	ballShape.m_radius = ball_radius;
	
	b2FixtureDef ballFixtureDef;
	ballFixtureDef.shape = &ballShape;
	ballFixtureDef.density = ball_density;
	ballFixtureDef.friction = ball_friction;
	ball->CreateFixture(&ballFixtureDef);
	
	// Initialize 1D array containing vertices of ball as triangle fan.
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
	// Clear objects from vectors of previous level (if they exist).
	if (obj_vbos.size()) {
		obj_vbos.clear();
		obj_num_verts.clear();
		obj_bodies.clear();
		obj_colors.clear();
	}
		
	// Loop through objects in level.
	for (unsigned int i = 0; i < level.num_polygons; i++) {
		poly_t poly = level.polygons[i];
		point_t pos = poly.position;
		
		// Convert point_t array to b2Vec2 array (preferred by Box2D).
		b2Vec2 *pts = new b2Vec2[poly.num_verts];
		for (unsigned int j = 0; j < poly.num_verts; j++) {
			pts[j] = b2Vec2(poly.verts[j].x, poly.verts[j].y);
		}
		
		// Create dynamic or static object depending on type provided by level.
		b2BodyDef objBodyDef;
		if (poly.is_dynamic) objBodyDef.type = b2_dynamicBody;
		objBodyDef.position.Set(pos.x, pos.y);
		b2Body* objBody = world->CreateBody(&objBodyDef);
			
		b2PolygonShape objShape;
		objShape.Set(pts, poly.num_verts);
			
		if (poly.is_dynamic) {
			b2FixtureDef objFixtureDef;
			objFixtureDef.shape = &objShape;
			objFixtureDef.density = obj_density;
			objFixtureDef.friction = obj_friction;
				
			objBody->CreateFixture(&objFixtureDef);
		} else {
			objBody->CreateFixture(&objShape, 0.0f);
		}
		
		// Obtain a 1D array of object vertices (each pair being a vertex).
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
		
		// Store new vbo-identifier, number of verts and Box2D bodies for later.
		obj_vbos.push_back(obj_vbo);
		obj_num_verts.push_back(poly.num_verts);
		obj_bodies.push_back(objBody);
		
		// Assign second color from color palette to object.
		color3f obj_color;
		obj_color.x = colors[1][0];
		obj_color.y = colors[1][1];
		obj_color.z = colors[1][2];
		obj_colors.push_back(obj_color);
	}
	
	// Join objects together using joints.
	for (unsigned int i = 0; i < level.num_joints; i++) {
	    joint_t joint = level.joints[i];
	    b2Body* body1 = obj_bodies[joint.objectA];
	    b2Body* body2 = obj_bodies[joint.objectB];
	    
	    // Add revolute joints.
	    if (joint.joint_type == JOINT_REVOLUTE) {
            b2RevoluteJointDef jointDef;
            jointDef.Initialize(body1, body2, b2Vec2(joint.anchor.x, joint.anchor.y));
            world->CreateJoint(&jointDef);

        // Add pulley joints.
	    } else {
	        b2Vec2 ground1 = b2Vec2(joint.pulley.ground1.x, joint.pulley.ground1.y);
	        b2Vec2 ground2 = b2Vec2(joint.pulley.ground2.x, joint.pulley.ground2.y);
	        b2Vec2 anchor1 = b2Vec2(joint.anchor.x, joint.anchor.y);
	        b2Vec2 anchor2 = b2Vec2(joint.pulley.anchor2.x, joint.pulley.anchor2.y);
	        float ratio = joint.pulley.ratio;
	        
	        b2PulleyJointDef jointDef;
	        jointDef.Initialize(body1, body2, ground1, ground2, anchor1, anchor2, ratio);
	        world->CreateJoint(&jointDef);
	    }
	} 
} 


void init_finish(level_t level) {
	// Set finish position.
	finish_pos = level.end;
	
	// Initialize 1D array with vertices of fisish (represented by a circle).
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
	b2Vec2 gravity(0.0, -gravity_force);
	world = new b2World(gravity);
	
	// Initialize each component of the level.
	init_ball(level);
	init_objects(level);
	init_finish(level);
	
	// Keep the game paused while user sets up other objects in the level.
	pause_game = true;

	amount_of_quadrilaterals = 0;
	amount_multiplier = amount_multipliers[level_id];
	level_score = level_scores[level_id];

}

std::vector<int> sort_indexes(const std::vector<float> &v) {

  // initialize original index locations
  std::vector<int> idx = {0,1,2,3};

  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(),
       [&v](float i1, float i2) {return v[i1] < v[i2];});

  return idx;
}

void add_new_object(void) {
	// Create b2Vec2 array with local vertex positions for Box2D.
	float mean_x = (clicks[0][0] + clicks[1][0] + clicks[2][0] + clicks[3][0]) / 4;
	float mean_y = (clicks[0][1] + clicks[1][1] + clicks[2][1] + clicks[3][1]) / 4;

	std::vector<float> angles;
	float angle;
	float vec[2];
	float len;
	for (int i = 0; i < 4; i++) {
		vec[0] = clicks[i][0] - mean_x;
		vec[1] = clicks[i][1] - mean_y;
		len = sqrt(vec[0]*vec[0] + vec[1] * vec[1]);
		angle = acos((vec[0])/(len));

		if (clicks[i][1] > mean_y) {
			angle = 2 * M_PI - angle;
		}

		angles.push_back(angle);
	}

	std::vector<int> sorted_angles = sort_indexes(angles);


	b2Vec2 pts[4] = {b2Vec2(clicks[sorted_angles[0]][0] - mean_x, clicks[sorted_angles[0]][1] - mean_y),
					 b2Vec2(clicks[sorted_angles[1]][0] - mean_x, clicks[sorted_angles[1]][1] - mean_y),
					 b2Vec2(clicks[sorted_angles[2]][0] - mean_x, clicks[sorted_angles[2]][1] - mean_y),
					 b2Vec2(clicks[sorted_angles[3]][0] - mean_x, clicks[sorted_angles[3]][1] - mean_y)};
					 
	// Set up dynamic body, shape and fixture.
	b2BodyDef objBodyDef;
	objBodyDef.type = b2_dynamicBody;
	objBodyDef.position.Set(mean_x, mean_y);
	b2Body* objBody = world->CreateBody(&objBodyDef);
			
	b2PolygonShape objShape;
	objShape.Set(pts, 4);
			
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
		
	// Add vbo-identifier, number of verts and body to level.
	obj_vbos.push_back(obj_vbo);
	obj_num_verts.push_back(4);
	obj_bodies.push_back(objBody);
	
	// Assign third color from color palette to new object.
	color3f obj_color;
	obj_color.x = colors[2][0];
	obj_color.y = colors[2][1];
	obj_color.z = colors[2][2];
	obj_colors.push_back(obj_color);

	// calculate the area size of the created quadrilateral
	float vec_1[2];
	float vec_2[2];
	vec_1[0] = clicks[sorted_angles[2]][0] - clicks[sorted_angles[0]][0];
	vec_1[1] = clicks[sorted_angles[2]][1] - clicks[sorted_angles[0]][1];
	vec_2[0] = clicks[sorted_angles[3]][0] - clicks[sorted_angles[1]][0];
	vec_2[1] = clicks[sorted_angles[3]][1] - clicks[sorted_angles[1]][1];

	float len_1 = sqrt(vec_1[0]*vec_1[0] + vec_1[1] * vec_1[1]);
	float len_2 = sqrt(vec_2[0]*vec_2[0] + vec_2[1] * vec_2[1]);
	float dot = vec_1[0]*vec_2[0] + vec_1[1]*vec_2[1];
	angle = acos(dot/(len_1*len_2));

	float size = ((len_1 * len_2)/2) * sin(angle);

	// remove points from the score based on the size of the quadrilateral 
	// and the amount of quadrilaterals already placed
	level_score -= (size * 100) * (1 + amount_multiplier * amount_of_quadrilaterals);
	
	// make sure the level score can not go below 0
	if (level_score < 0) level_score = 0;
	amount_of_quadrilaterals++;
}


/**********************************
 * Level Drawing Functions.
 **********************************/

void draw_ball(void) {
	// Push translation matrix onto matrix stack.
	glPushMatrix();
    glTranslatef(ball->GetPosition().x, ball->GetPosition().y, 0);	
        	
	// Draw ball on screen.
	glColor3f(colors[0][0], colors[0][1], colors[0][2]);
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
			glRotatef(360.0 * body->GetAngle() / (2 * M_PI), 0, 0, 1);
					
			// Draw ball on screen.
			glBindBuffer(GL_ARRAY_BUFFER, obj_vbos[i]);
			glVertexPointer(2, GL_FLOAT, 0, NULL);
			glColor3f(obj_colors[i].x, obj_colors[i].y, obj_colors[i].z);
			
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
	glColor3f(colors[3][0], colors[3][1], colors[3][2]);
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
		level_score -= death_penalty;
		if (level_score < 0) level_score = 0;
		load_world(current_level);
	}
	
	// If ball reaches finish, then go to next level or stop the game (end).
	float dist = sqrt(pow(finish_pos.x - x, 2) + pow(finish_pos.y - y, 2));
	if (dist < finish_radius + ball_radius) {
		if (!finished) score += level_score;

		// If this was the final level, then quit or something.
		if (current_level == num_levels-1) {
			pause_game = true;
			char window_title[128];
        	snprintf(window_title, 128,
                "You have finished the game with score: %d!!!! press 'q' to quit", score);
			glutSetWindowTitle(window_title);
			finished = true;	
			
		// Otherwise, go to level + 1.
		} else {
			load_world(++current_level);
		}
	}
    
	// EXTRA FEATURE: animate background color
	float k = color_counter / color_frames;
	glClearColor(next_color[0] * k + base_color[0] * (1-k),
				 next_color[1] * k + base_color[1] * (1-k),
				 next_color[2] * k + base_color[2] * (1-k),
				 1);

	color_counter++;

	// choose a new color to be the next color to animate to and make the 
	// old next color the base color
	if (color_counter == color_frames) {
		base_color[0] = next_color[0];
		base_color[1] = next_color[1];
		base_color[2] = next_color[2];

		// create a new color by choosing a random number between 0.25 and 0.75 
		// for the r, g and b.
		next_color[0] = ((float) rand() / RAND_MAX) * 0.5 + 0.25;
		next_color[1] = ((float) rand() / RAND_MAX) * 0.5 + 0.25;
		next_color[2] = ((float) rand() / RAND_MAX) * 0.5 + 0.25;

		color_counter = 0;
	}
	
    glClear(GL_COLOR_BUFFER_BIT);
    
	// Draw the level finish, ball and objects in that order.
    draw_finish();
    draw_ball();
    draw_objects();

    glutSwapBuffers();

	// Show level info and framerate.
    if (frametime >= 1000 && !finished)
    {
        char window_title[128];
        snprintf(window_title, 128,
                "Box2D: %f fps, level %d/%d || score: %d, levelscore: %d", 
				frame_count / (frametime / 1000.f), current_level+1, num_levels, 
				score, level_score);
        glutSetWindowTitle(window_title);
        last_time = time;
        frame_count = 0;
    }
    
    // Tick if game is not paused.
    if (!pause_game) world->Step(framerate, velocity_iters, position_iters);
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
            if (!finished) pause_game = false;
            break;
        case 'r':
            load_world(current_level);
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
		
		// If four points have been gathered, add a object to the level.
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
