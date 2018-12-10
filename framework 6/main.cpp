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

unsigned int reso_x = 800, reso_y = 600; // Window size in pixels
const float world_x = 8.f, world_y = 6.f; // Level (world) size in meters

int last_time;
int frame_count;

// Information about the levels loaded from files will be available in these.
unsigned int num_levels;
level_t *levels;

/**********************************
 * Level / Game State Variables
 **********************************/

b2World* world;
b2Body* ball;

float gravity_force = 4.0;

int ball_resolution = 32;
float ball_friction = 0.5;
float ball_density = 1.2;
float ball_radius = 0.25;
GLuint ball_VAO;

float obj_friction = 0.5;
float obj_density = 1.2;

int current_level = 0;
int pause_game = 1;
point_t finish_pos;
float finish_radius = 0.1;

int n_clicks = 0;
int clicks[4][2];

/**********************************
 * Shaders.
 **********************************/
 
// TODO: add shaders!


/**********************************
 * Level Initialization Functions.
 **********************************/

// Loads the ball into Box2D and OpenGL VBO and VAO.
void init_ball(level_t level) {	
	// Set up body
	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(level.start.x, level.start.y);
	ball = world->CreateBody(&ballBodyDef);
	
	// Set up shape
	b2CircleShape ballShape;
	ballShape.m_radius = ball_radius;
	
	// Set up fixture
	b2FixtureDef ballFixtureDef;
	ballFixtureDef.shape = &ballShape;
	ballFixtureDef.density = ball_density;
	ballFixtureDef.friction = ball_friction;
	ball->CreateFixture(&ballFixtureDef);
	
	// Set up VBO for ball.
	GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 
						   1.0f, -1.0f, 0.0f, 
						   0.0f, 0.5f, 0.0f};
	GLuint ball_VBO[1];
	glGenBuffers(1, ball_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, ball_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Set up ball VAO.
	glGenVertexArrays(1, &ball_VAO);
	glBindVertexArray(ball_VAO);
	glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, ball_VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}


// Initializes level' objects. It is only called for setting up the level. 
void init_objects(level_t level) {
	// Loop through polygons in level.
	for (unsigned int i = 0; i < level.num_polygons; i++) {
		poly_t poly = level.polygons[i];
		point_t pos = poly.position;
		
		// Convert point_t array to b2Vec2 array.
		b2Vec2 *pts = new b2Vec2[poly.num_verts];
		for (unsigned int j = 0; j < poly.num_verts; j++) {
			pts[j] = b2Vec2(poly.verts[j].x, poly.verts[j].y);
		}
		
		// Create dynamic and static objects from level.
		b2BodyDef objBodyDef;
		if (poly.is_dynamic) objBodyDef.type = b2_dynamicBody;
		objBodyDef.position.Set(pos.x, pos.y);
		b2Body* objBody = world->CreateBody(&objBodyDef);
			
		// Set up shape.
		b2PolygonShape objShape;
		objShape.Set(pts, poly.num_verts);
			
		// Set up fixture.
		if (poly.is_dynamic) {
			b2FixtureDef objFixtureDef;
			objFixtureDef.shape = &objShape;
			objFixtureDef.density = obj_density;
			objFixtureDef.friction = obj_friction;
				
			objBody->CreateFixture(&objFixtureDef);
		} else {
			objBody->CreateFixture(&objShape, 0.0f);
		}
		
		// TODO: set up VBO and VAO for objects
	}
}


// Adds another object to the level using four user selected points.
void add_object(void) {	
	// Convert point_t array to b2Vec2 array.
	b2Vec2 *pts = new b2Vec2[4];
	for (unsigned int j = 0; j < 4; j++) {
		int x = world_x * clicks[j][0] / reso_x;
		int y = world_y - (world_y * clicks[j][1] / reso_y);
		pts[j] = b2Vec2(x, y);
	}
	
	// TODO: fix spawn!
		
	// Create a new dynamic object from these points.
	b2BodyDef objBodyDef;
	objBodyDef.type = b2_dynamicBody;
	objBodyDef.position.Set(0, 0);
	b2Body* objBody = world->CreateBody(&objBodyDef);
			
	b2PolygonShape objShape;
	objShape.Set(pts, 4);
			
	b2FixtureDef objFixtureDef;
	objFixtureDef.shape = &objShape;
	objFixtureDef.density = obj_density;
	objFixtureDef.friction = obj_friction;
			
	objBody->CreateFixture(&objFixtureDef);
	
	// TODO: set up VBO and VAO for new object (or copy new geometry).
}


// Load a given world and convert it into a Box2D world.
void load_world(unsigned int level_id) {
    if (level_id >= num_levels) {
        printf("Warning: level %d does not exist.\n", level_id);
        return;
    }
    
    // Read world from levels.
    level_t level = levels[level_id];

	// Initialize the world.
	b2Vec2 gravity(0.0f, -gravity_force);
	world = new b2World(gravity);
	
	// Initialize ball in world.
	init_ball(level);
	
	// Initialize remaining objects in the scene.
	init_objects(level);
	
	// Define the finish position and pause game.
	finish_pos = level.end;
	pause_game = 1;
}


/**********************************
 * Level Drawing Functions.
 **********************************/
 
// Draws the ball.
void draw_circle(float x, float y, float rad, float r, float g, float b) {
	/*float x2, y2;
	
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int i = 0; i <= ball_resolution; i++) {
		float angle = 2 * M_PI * i / ball_resolution;
		x2 = x + rad * cos(angle);
		y2 = y + rad * sin(angle);
		glVertex2f(x2, y2);
	}
	glEnd();*/
	glBindVertexArray(ball_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}


// Draws the other objects.
void draw_polygon(float x, float y, b2PolygonShape* shape, float r, float g, float b) {
    float x2, y2;

    // Get vertices and number of vertices.
	int n_vertices = shape->GetVertexCount();
	b2Vec2* vertices = (b2Vec2*) shape->m_vertices;
	
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
    for( int i = 0; i < n_vertices; i++) {
       x2 = x + vertices[i].x;
       y2 = y + vertices[i].y;
       glVertex2f(x2, y2);
    }
	glEnd();
}

// Wrapper to draw the entire level (called at each tick).
void draw_level(void) {
    // Otherwise, start drawing. Draw finish as a small circle.
    draw_circle(finish_pos.x, finish_pos.y, finish_radius, 0, 1, 0);
    
    // Draw physics objects (e.g. ball) of the level.
    b2Body* body = world->GetBodyList();
    while (body) {
    	// Get body shape, type and position.
    	const b2Vec2 pos = body->GetPosition();
    	b2Shape* shape = body->GetFixtureList()->GetShape();
    	int shapeType = shape->GetType();
    	
    	// If of type b2CircleShape, then draw its circle.
    	if (shapeType == 0) {
    		draw_circle(pos.x, pos.y, ball_radius, 1, 0, 0);
    		
    	// If of type b2PolygonShape, then draw its Polygon.
    	} else if (shapeType == 2) {
    		draw_polygon(pos.x, pos.y, (b2PolygonShape*) shape, 0, 0, 1);
    	}
    	
    	// Go to next object body.
    	body = body->GetNext();
    }
}


/**********************************
 * Game Logic
 **********************************/
 
// Called to update the game state.
void update_state(void) {
    int time = glutGet(GLUT_ELAPSED_TIME);
    int frametime = time - last_time;
    frame_count++;

    // Clear the buffer
    glColor3f(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // If ball reaches finish continue to next level and pause game.
    const b2Vec2 ball_pos = ball->GetPosition();
    float finish_dist = fabs(finish_pos.x - ball_pos.x) + fabs(finish_pos.y - ball_pos.y);
    if (finish_dist < finish_radius+ball_radius) {
    	load_world(++current_level);
    	return;
    }
    
    // If ball has fallen (y=0), then restart level (i.e. game over).
    if (ball_pos.y < 0) {
        load_world(current_level);
        return;
    }
    
    // Draw the current state of the level.
    draw_level();

    // Perform tick if user has previously pressed 's', keep on waiting.
    if (!pause_game) {
        world->Step(1.0f / 60.0f, 6, 2);
    }

    // Show rendered frame
    glutSwapBuffers();

    // Display fps in window title.
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
}


/**********************************
 * User Interaction.
 **********************************/
 
// Resizes window.
void resize_window(int width, int height) {
    glViewport(0, 0, width, height);
    reso_x = width;
    reso_y = height;
}


// Key binding.
void key_pressed(unsigned char key, int x, int y) {
    switch (key)
    {
        case 27: // Esc
        case 'q':
            exit(0);
            break;
        case 's':
            pause_game = 0;
            break;
        default:
            break;
    }
}


// Click binding.
void mouse_clicked(int button, int state, int x, int y) {
    // If user presses the left mouse button, then record mouse position.
	if (state == 0 && button == 0) {
		clicks[n_clicks][0] = x;
		clicks[n_clicks][1] = y;
		n_clicks++;
		
		// If four points have been gathered, add a new object to the level.
		if (n_clicks == 4) {
			add_object();
			n_clicks = 0;
		}
	}
}


// Move binding.
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
