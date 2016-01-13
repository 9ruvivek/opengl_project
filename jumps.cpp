/*********************************** Nothing to be Changed in this section **********************************/
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
typedef struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
}VAO;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
GLuint programID;
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	GLint Result = GL_FALSE;
	int InfoLogLength;
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
}
VAO* create3DObject(GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;
    glGenVertexArrays(1, &(vao->VertexArrayID));
    glGenBuffers (1, &(vao->VertexBuffer));
    glGenBuffers (1, &(vao->ColorBuffer));
    glBindVertexArray (vao->VertexArrayID); 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer);
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);
    return vao;
}
VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }
    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}
void draw3DObject (struct VAO* vao)
{
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
    glBindVertexArray (vao->VertexArrayID);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices);
}
/******************************************* Beginning of Main Code  *************************************************/
#define PI M_PI
float rectangle_rot_dir = 1;
bool rectangle_rot_status = false;
int width,height;
float xpos=-200.0,ypos=-200.0;
float xcenter=-200.0,ycenter=-200.0;
float radius=0.0,rotation=180.0;
bool atstart=true;
bool noconstraints=false;
float T=0;
VAO *rectangle;
float camera_rotation_angle = 90;
float rectangle_rotation = 0;

/* Executed when a regular key is pressed */
void keyboardDown(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'Q':
        case 'q':
        case 27: //ESC
            exit (0);
        default:
            break;
    }
}
/* Executed when a regular key is released */
void keyboardUp(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'c':
        case 'C':
            rectangle_rot_status = !rectangle_rot_status;
            break;
        case 'x':
            // do something
            break;
	case 32:
	    if(atstart)
	    {
	    	atstart=false;
	    	noconstraints=true;
		//cout << "Jump mode on!!! " << atstart << "\n";
	    }
	    break;
	case 'b':
	    //cout << "Buhahaha\n";
	    atstart=true;
	    T=0;
	    break;
        default:
            break;
    }
}
/* Executed when a special key is pressed */
void keyboardSpecialDown(int key, int x, int y)
{
	if(!atstart)
		return;

	switch(key)
	{
		case GLUT_KEY_DOWN:
			rotation+=20.0f;
			if(rotation>=360.0f)
				rotation-=360.0f;
			break;
		
		case GLUT_KEY_UP:
			rotation-=20.0f;
			if(rotation<0.0f)
				rotation+=360.0f;
			break;

		case GLUT_KEY_LEFT:
			radius+=2.0f;
			radius=min(radius,30.0f);
			break;
		
		case GLUT_KEY_RIGHT:
			radius-=2.0f;
			radius=max(radius,0.0f);
			break;

		default:
			break;
	}
}
/* Executed when a special key is released */
void keyboardSpecialUp(int key, int x, int y)
{
}
/* Executed when a mouse button 'button' is put into state 'state' at screen position ('x', 'y') */
void mouseClick(int button, int state, int x, int y)
{
    switch(button)
    {
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_UP)
                rectangle_rot_dir *= -1;
            break;
        default:
            break;
    }
}
/* Executed when the mouse moves to position ('x', 'y') */
void mouseMotion(int x, int y)
{
	cerr << x << y << "\n";
}
void cursorPos(int x, int y)
{
	cerr << x << y << "\n";
}
/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */

// To be understood !!
void reshapeWindow(int width, int height)
{
	GLfloat fov = 90.0f;
	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	// set the projection matrix as perspective/ortho
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) width / (GLfloat) height, 0.1f, 500.0f);
    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-width/2.0f,width/2.0f,-height/2.f,height/2.0f,0.1f, 500.0f);
}
VAO* createRectangle(float x,float y)
{
  // GL3 accepts only Triangles. Quads are not supported static
  const GLfloat vertex_buffer_data [] = {
    -x,-y,0.0, // vertex 1
    x,-y,0.0, // vertex 2
    x, y,0.0, // vertex 3

    x, y,0.0, // vertex 3
    -x, y,0.0, // vertex 4
    -x,-y,0.0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    0,0,1, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0.3,0.3,0.3, // color 4
    1,0,0  // color 1
  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  return create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
  // clear the color and depth in the frame buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  glUseProgram (programID);

  /*
  // Eye - Location of camera.
  glm::vec3 eye( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.
  glm::vec3 target(0, 0, 0);
  // Up - Up vector defines tilt of camera.
  glm::vec3 up(0, 1, 0);
  */
  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  glm::mat4 MVP;	// MVP = Projection * View * Model

  Matrices.model = glm::mat4(1.0f);
  //cout << atstart << "\n";
  if(atstart)
  {
	//cout << "At start !!!\n";
  	xpos=xcenter+radius*cos((rotation*PI)/180.0f);
  	ypos=ycenter+radius*sin((rotation*PI)/180.0f);
  }
  else
  {
	  float startx=xcenter+radius*cos((rotation*PI)/180.0f);
  	  float starty=ycenter+radius*sin((rotation*PI)/180.0f);
	  float velocity=(30.0f*radius)/30.0f;
	  float A=1.0;
	  float FT;
	  if(rotation<180.0f)
	  	FT=(2.0*velocity*sin((rotation*PI)/180.0f))/A;
	  else
	  	FT=(2.0*velocity*sin(((rotation-180.0f)*PI)/180.0f))/A;
	  //cout << "Entered projectile!!! with " << FT << "\n";
	  if(noconstraints)
	  {
		  T++;
		  if(rotation<180.0f)
		  {
		  	xpos=startx+velocity*cos((rotation*PI)/180.0f)*T;
		  	ypos=starty+velocity*sin((rotation*PI)/180.0f)*T-0.5*A*T*T;
		  }
		  else
		  {
		  	xpos=startx+velocity*cos(((rotation-180.0f)*PI)/180.0f)*T;
		  	ypos=starty+velocity*sin(((rotation-180.0f)*PI)/180.0f)*T-0.5*A*T*T;
		  }
		  if(T>=FT)
			  noconstraints=false;
	  }
  }
  glm::mat4 translateRectangle = glm::translate (glm::vec3(xpos, ypos, 0.0f));        // glTranslatef
  glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle * rotateRectangle);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle);

  // Swap the frame buffers
  glutSwapBuffers ();

  // Increment angles
  float increments = 0;

  //camera_rotation_angle++; // Simulating camera rotation
  // This basically does rotation if needed in the direction required
  rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;
}

/* Executed when the program is idle (no I/O activity) */
void idle () {
    // OpenGL should never stop drawing
    // can draw the same scene or a modified scene
    draw (); // drawing same scene
}


/* Initialise glut window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */

void initGLUT(int& argc, char** argv, int width, int height)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_CORE_PROFILE);
    glutInitWindowSize(width, height);
    glutCreateWindow("Learning OGL3.3");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
        exit (1);
    }
    // register glut callbacks
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    
    glutSpecialFunc (keyboardSpecialDown);
    glutSpecialUpFunc (keyboardSpecialUp);

    glutPassiveMotionFunc( cursorPos);
    glutMouseFunc (mouseClick);
    glutMotionFunc (mouseMotion);

    glutReshapeFunc (reshapeWindow);

    glutDisplayFunc (draw); // function to draw when active
    glutIdleFunc (idle); // function to draw when idle (no I/O activity)
    
    //glutIgnoreKeyRepeat (true); // Ignore keys held down*/
}

void initGL(int width, int height)
{
	// Create the models

	// Create and compile our GLSL program from the shaders
	rectangle = createRectangle (10.0,10.0);
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");

	reshapeWindow (width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);


	/*cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;*/
}
int main (int argc, char** argv)
{
	width = 1300;
	height = 700;
	initGLUT (argc, argv, width, height);
	initGL(width, height);
	glutMainLoop ();
	return 0;
}
