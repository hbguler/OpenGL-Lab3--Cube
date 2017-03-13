//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "Angel.h"
#include "glui.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//GLM variables
glm::mat4 TRS;
glm::mat4 TranslateMatrix;
glm::mat4 RotateMatrix;
glm::mat4 ScaleMatrix;

GLfloat centerCubeSpinConstant = 2.5;
GLfloat centerCubeBeatingConstant = 0.2;
GLfloat orbitingCubeSpinConstant = 2.5;
GLfloat orbitingCubeOrbitSpeedConstant = 0.05;

GLfloat centerSpinTemp = 0;
GLfloat centerBeatTemp = 0;
GLfloat orbitSpinTemp = 0;
GLfloat orbitOrbitTemp = 0;


GLfloat centerCubeSpinSpeed = 0.5;
GLfloat centerCubeBeatInterval = 0.025; 
GLfloat orbitingCubeSpinSpeed = 0.5;
GLfloat orbitingCubeOrbitSpeed = 0.05;

GLfloat thetaCenter;
GLfloat thetaOrbit;
GLfloat centerXbool = -1;
GLfloat centerYbool = -1;
GLfloat centerZbool = -1;
GLfloat orbitXbool = -1;
GLfloat orbitYbool = -1;
GLfloat orbitZbool = -1;


//GLUI variables
int   wireframe = 0;
int   segments = 8;
int   main_window; 
GLUI_Spinner *centerCubeSpinSpeedSpinner; 
GLUI_Spinner *centerCubeBeatIntervalSpinner;
GLUI_Spinner *orbitingCubeSpinSpeedSpinner;
GLUI_Spinner *orbitingCubeOrbitSpeedSpinner;


GLuint program;
GLuint vao;
GLuint vao2;


typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];
point4 points2[NumVertices];
color4 colors2[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {

	point4(-0.2, -0.2,  0.2, 1.0),
	point4(-0.2,  0.2,  0.2, 1.0),
	point4(0.2,  0.2,  0.2, 1.0),
	point4(0.2, -0.2,  0.2, 1.0),
	point4(-0.2, -0.2, -0.2, 1.0),
	point4(-0.2,  0.2, -0.2, 1.0),
	point4(0.2,  0.2, -0.2, 1.0),
	point4(0.2, -0.2, -0.2, 1.0)
};

point4 verticesOfOrbitingCube[8] = {

	point4(-0.1, -0.1,  0.1, 1.0),
	point4(-0.1,  0.1,  0.1, 1.0),
	point4(0.1,  0.1,  0.1, 1.0),
	point4(0.1, -0.1,  0.1, 1.0),
	point4(-0.1, -0.1, -0.1, 1.0),
	point4(-0.1,  0.1, -0.1, 1.0),
	point4(0.1,  0.1, -0.1, 1.0),
	point4(0.1, -0.1, -0.1, 1.0)
};

// RGBA olors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(0.0, 1.0, 1.0, 1.0),  // cyan
	color4(1.0, 1.0, 1.0, 1.0)  // white
	
};

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };

int      Axis2 = Xaxis;
GLfloat  Theta2[NumAxes] = { 0.0, 0.0, 0.0 };


GLuint  theta;  // The location of the "theta" shader uniform variable

				//----------------------------------------------------------------------------

				// quad generates two triangles for each face and assigns colors
				//    to the vertices
int Index = 0;



void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;


}

void
quad2(int a, int b, int c, int d)
{
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[a]; Index++;
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[b]; Index++;
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[c]; Index++;
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[a]; Index++;
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[c]; Index++;
	colors2[Index] = vertex_colors[a]; points2[Index] = verticesOfOrbitingCube[d]; Index++;


}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);

	Index = 0;

	quad2(1, 0, 3, 2);
	quad2(2, 3, 7, 6);
	quad2(3, 0, 4, 7);
	quad2(6, 5, 1, 2);
	quad2(4, 5, 6, 7);
	quad2(5, 4, 0, 1);

}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader36.glsl", "fshader36.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	//----------------------------------------------------


	// Create a vertex array object
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	// Create and initialize a buffer object
	GLuint buffer2;
	glGenBuffers(1, &buffer2);
	glBindBuffer(GL_ARRAY_BUFFER, buffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2) + sizeof(colors2),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points2), points2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2), sizeof(colors2), colors2);


	// Load shaders and use the resulting shader program
	program = InitShader("vshader36.glsl", "fshader36.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition2 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vColor2 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));






	theta = glGetUniformLocation(program, "theta");

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);

}



//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 translate;
	GLint transformLocation;



	/*--------CENTER CUBE----------- */
	//Beat value determined by sinCenterCube value, sin comes from -1 and goes to +1.
	//Beat ratio value comes from beat constant. Constant value is changing by idle function with beating interval.

	GLfloat sinCenterCube = sin(Theta[Axis]) * 0.25f + 1.0f;
	GLfloat xSpinCenterCube = cos(thetaCenter);
	GLfloat ySpinCenterCube = sin(thetaCenter);
	
	TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ScaleMatrix = glm::scale(TranslateMatrix, glm::vec3(sinCenterCube, sinCenterCube, sinCenterCube));
	RotateMatrix = glm::rotate(ScaleMatrix, glm::radians(thetaCenter), glm::vec3(centerXbool, centerYbool, centerZbool));
	TRS = TranslateMatrix  
		* RotateMatrix 
		* ScaleMatrix
		;


	transformLocation = glGetUniformLocation(program, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(TRS));

	glUseProgram(program);
	glBindVertexArray(vao);
	glUniform3fv(theta, 1, Theta);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);


	//--------------------------------------------------------------------------------------

	/*--------ORBITING CUBE----------- */

	GLfloat xOrbitingCube = cos(Theta2[Axis]) * 0.3f;
	GLfloat yOrbitingCube = sin(Theta2[Axis]) * 0.3f;

	TranslateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(xOrbitingCube, yOrbitingCube, 0.0f));
	ScaleMatrix = glm::scale(TranslateMatrix, glm::vec3(0.75, 0.75, 0.75));
	RotateMatrix = glm::rotate(ScaleMatrix, glm::radians(thetaOrbit), glm::vec3(orbitXbool, orbitYbool, orbitZbool));
	
	TRS = ScaleMatrix
		* TranslateMatrix
		* RotateMatrix
		;

	

	transformLocation = glGetUniformLocation(program, "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(TRS));

	glUseProgram(program);
	glBindVertexArray(vao);
	glUniform3fv(theta, 1, Theta);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);


	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
		case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
		case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
		}
	}
}

//----------------------------------------------------------------------------

void
idle(void)
{
	// Theta is used for spinning
	Theta[Axis] += centerCubeBeatingConstant;
	Theta2[Axis] += orbitingCubeOrbitSpeedConstant;
	
	thetaCenter += centerCubeSpinConstant;
	thetaOrbit += orbitingCubeSpinConstant;

	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}
	if (Theta2[Axis] > 360.0) {
		Theta2[Axis] -= 360.0;
	}
	if (thetaCenter > 360) {
		thetaCenter -= 360;
	}
	if (thetaOrbit > 360) {
		thetaOrbit -= 360;
	}


	GLUI_Master.auto_set_viewport();
	if(glutGetWindow() != main_window )
		glutSetWindow(main_window);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------
//GLUI FUNCTIONS

void changeCenterCubeSpinRotation(int i) {

	if (i == 0){
		centerXbool = 1;
		centerYbool = 0;
		centerZbool = 0;
	}
	else if (i == 1) {
		centerXbool = 0;
		centerYbool = 1;
		centerZbool = 0;
	}
	else if (i == 2){
		centerXbool = 0;
		centerYbool = 0;
		centerZbool = 1;
	}


	
}
void changeOrbitingCubeSpinRotation(int i) {

	if (i == 0) {
		orbitXbool = 1;
		orbitYbool = 0;
		orbitZbool = 0;
	}
	else if (i == 1) {
		orbitXbool = 0;
		orbitYbool = 1;
		orbitZbool = 0;
	}
	else if (i == 2) {
		orbitXbool = 0;
		orbitYbool = 0;
		orbitZbool = 1;
	}

}

void changeCenterCubeSpinSpeed(int i) {
	if (i == 0) {
		if (centerCubeSpinConstant != 0) {
			centerSpinTemp = centerCubeSpinConstant;
			centerCubeSpinConstant = 0;
		}
		else {
			centerCubeSpinConstant = centerSpinTemp;
		}
	}
	else if (i == 1) {
		if (centerCubeSpinConstant <= 5)
			centerCubeSpinConstant += centerCubeSpinSpeed;
	}
	else if (i == 2) {
		if (centerCubeSpinConstant >= -5)
			centerCubeSpinConstant -= centerCubeSpinSpeed;
	}
	centerCubeSpinSpeedSpinner -> set_float_val(centerCubeSpinConstant);

}
void changeOrbitingCubeSpinSpeed(int i) {
	if (i == 0) {
		if (orbitingCubeSpinConstant != 0) {
			orbitSpinTemp = orbitingCubeSpinConstant;
			orbitingCubeSpinConstant = 0;
		}
		else {
			orbitingCubeSpinConstant = orbitSpinTemp;
		}

	}
	else if (i == 1) {
		if (orbitingCubeSpinConstant <= 5)
			orbitingCubeSpinConstant += orbitingCubeSpinSpeed;
	}
	else if (i == 2) {
		if (orbitingCubeSpinConstant >= -5)
			orbitingCubeSpinConstant -= orbitingCubeSpinSpeed;
	}
	orbitingCubeSpinSpeedSpinner ->set_float_val(orbitingCubeSpinConstant);

}

void changeCenterCubeBeating(int i) {
	if (i == 0) {
		if (centerCubeBeatingConstant != 0) {
			centerBeatTemp = centerCubeBeatingConstant;
			centerCubeBeatingConstant = 0;
		}
		else {
			centerCubeBeatingConstant = centerBeatTemp;
		}
	}
	else if (i == 1) {
		if (centerCubeBeatingConstant <= 5)
			centerCubeBeatingConstant += centerCubeBeatInterval;
	}
	else if (i == 2) {
		if (centerCubeBeatingConstant >= -5)
			centerCubeBeatingConstant -= centerCubeBeatInterval;
	}
	centerCubeBeatIntervalSpinner->set_float_val(centerCubeBeatingConstant);
}
void changeOrbitingCubeOrbitSpeed(int i) {
	if (i == 0) {
		if (orbitingCubeOrbitSpeedConstant != 0) {
			orbitOrbitTemp = orbitingCubeOrbitSpeedConstant;
			orbitingCubeOrbitSpeedConstant = 0;
		}
		else {
			orbitingCubeOrbitSpeedConstant = orbitOrbitTemp;
		}
	}
	else if (i == 1) {
		if (orbitingCubeOrbitSpeedConstant <= 5)
			orbitingCubeOrbitSpeedConstant += orbitingCubeOrbitSpeed;
	}
	else if (i == 2) {
		if (orbitingCubeOrbitSpeedConstant >= -5)
			orbitingCubeOrbitSpeedConstant -= orbitingCubeOrbitSpeed;
	}
	orbitingCubeOrbitSpeedSpinner ->set_float_val(orbitingCubeOrbitSpeedConstant);


}



void gluiFunctions() {


	GLUI *glui = GLUI_Master.create_glui("GLUI");
	glui->set_main_gfx_window(main_window);
	
	GLUI_Panel *obj_panel = glui->add_panel("Origin Cube Buttons");


	/*--------CENTER CUBE----------- */

	/* Spinning Rotations */
	glui->add_button_to_panel(obj_panel, "Rotate X", 0, changeCenterCubeSpinRotation);	glui->add_button_to_panel(obj_panel, "Rotate Y", 1, changeCenterCubeSpinRotation);	glui->add_button_to_panel(obj_panel, "Rotate Z", 2, changeCenterCubeSpinRotation);		glui->add_column_to_panel(obj_panel, true);	/* Spinning Speed */	glui->add_button_to_panel(obj_panel, "Start/Stop Spinning", 0, changeCenterCubeSpinSpeed);	glui->add_button_to_panel(obj_panel, "Speed++", 1, changeCenterCubeSpinSpeed);	glui->add_button_to_panel(obj_panel, "Speed--", 2, changeCenterCubeSpinSpeed);	
	centerCubeSpinSpeedSpinner = glui->add_spinner_to_panel(obj_panel, "Spinning Speed: ", GLUI_SPINNER_FLOAT, &centerCubeSpinConstant);
	centerCubeSpinSpeedSpinner -> set_int_limits(-5, 5, GLUI_LIMIT_CLAMP);
	glui->add_column_to_panel(obj_panel, true);	/* Beating Speed */	glui->add_button_to_panel(obj_panel, "Start/Stop Beating", 0, changeCenterCubeBeating);	glui->add_button_to_panel(obj_panel, "Speed++", 1, changeCenterCubeBeating);	glui->add_button_to_panel(obj_panel, "Speed--", 2, changeCenterCubeBeating);	centerCubeBeatIntervalSpinner = glui->add_spinner_to_panel(obj_panel, "Beating Interval: ", GLUI_SPINNER_FLOAT, &centerCubeBeatingConstant);
	/*  --------ORBITING CUBE----------- */

	GLUI_Panel *obj_panel2 = glui->add_panel("Orbiting Cube Buttons");

	/* Spinning Rotations */
	glui->add_button_to_panel(obj_panel2, "Rotate X", 0, changeOrbitingCubeSpinRotation);	glui->add_button_to_panel(obj_panel2, "Rotate Y", 1, changeOrbitingCubeSpinRotation);	glui->add_button_to_panel(obj_panel2, "Rotate Z", 2, changeOrbitingCubeSpinRotation);	glui->add_column_to_panel(obj_panel2, true);	/* Spinning Speed */	glui->add_button_to_panel(obj_panel2, "Start/Stop Spinning", 0, changeOrbitingCubeSpinSpeed);	glui->add_button_to_panel(obj_panel2, "Speed++", 1, changeOrbitingCubeSpinSpeed);	glui->add_button_to_panel(obj_panel2, "Speed--", 2, changeOrbitingCubeSpinSpeed);	orbitingCubeSpinSpeedSpinner = glui->add_spinner_to_panel(obj_panel2, "Spinning Speed: ", GLUI_SPINNER_FLOAT, &orbitingCubeSpinConstant);
	orbitingCubeSpinSpeedSpinner-> set_int_limits(-5, 5, GLUI_LIMIT_CLAMP);
	glui->add_column_to_panel(obj_panel2, true);	/* Orbiting Speed */	glui->add_button_to_panel(obj_panel2, "Start/Stop Orbiting", 0, changeOrbitingCubeOrbitSpeed);	glui->add_button_to_panel(obj_panel2, "Speed++", 1, changeOrbitingCubeOrbitSpeed);	glui->add_button_to_panel(obj_panel2, "Speed--", 2, changeOrbitingCubeOrbitSpeed);	orbitingCubeOrbitSpeedSpinner = glui->add_spinner_to_panel(obj_panel2, "Orbiting Speed: ", GLUI_SPINNER_FLOAT, &orbitingCubeOrbitSpeedConstant);
	orbitingCubeOrbitSpeedSpinner->set_int_limits(-5, 5, GLUI_LIMIT_CLAMP);	/*  Quit. */	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);


	GLUI_Master.set_glutKeyboardFunc(keyboard);
	GLUI_Master.set_glutMouseFunc(mouse);
	GLUI_Master.set_glutDisplayFunc(display);
	GLUI_Master.set_glutIdleFunc(idle);

}


//----------------------------------------------------------------------------


int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	//glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	main_window = glutCreateWindow("Color Cube");
	glewExperimental = GL_TRUE;

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	gluiFunctions();

	glutMainLoop();
	return 0;
}
