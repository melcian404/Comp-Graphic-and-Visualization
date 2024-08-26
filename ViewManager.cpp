///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// declaration of the global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f; 
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager *pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();
	// default camera view parameters
	g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
	g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80;
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// tell GLFW to capture all mouse events
	//((((((((((((((((((((((((( Took off inline comment ))))))))))))))))))))
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// this callback is used to receive mouse moving events
	glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

	//(((((((((( This callback will receive mouse wheel scrolling events)))))))
	glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Wheel_Callback);


	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	//(((((((((((((((((((((( first mouse event recorded )))))))))))))))
	if (gFirstMouse) {
		gLastX = xMousePos;
		gLastY = yMousePos;
		gFirstMouse = false;
	}

	//(((((((((((((((((((((((( Movement recorded ))))))))))))))))))))))
	float xOffset = xMousePos - gLastX;
	float yOffset = gLastY - yMousePos;

	//((((((((((((((((((((( Set new position to last position ))))))))))))))))))))
	gLastX = xMousePos;
	gLastY = yMousePos;

	//(((((((((((((((((((( Move camera according to calculation offsets ))))))))))))
	g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}


//((((((((((((((((((((((((((((((Added a Mouse Scroll Callback function)))))))))))))))))))))))))))))))))))))))))))
//(((((((((((((((((((((((((((((( Allows for movement speed to change )))))))))))))))))))))))))))))))))))))

void ViewManager::Mouse_Scroll_Wheel_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	// when the first mouse move event is received, this needs to be recorded so that
	// all subsequent mouse moves can correctly calculate the X position offset and Y
	// position offset for proper operation
	if (gFirstMouse)
	{
		gLastX = xMousePos;
		gLastY = yMousePos;
		gFirstMouse = false;
	}

	// calculate the X offset and Y offset values for moving the 3D camera accordingly
	//float xOffset = xMousePos - gLastX;
	float yOffset = gLastY - yMousePos; // reversed since y-coordinates go from bottom to top

	// set the current positions into the last position variables
	gLastX = xMousePos;
	gLastY = yMousePos;

	// move the 3D camera according to the calculated offsets
	g_pCamera->ProcessMouseScroll(yOffset);
}


/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// close the window if the escape key has been pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}


	//(((((((((((((( Zoom in with W ))))))))))))))))))))

	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
	}

	//((((((((((((((( Zoom out with S )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
	}

	//((((((((((((((( Pan left with A )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
	}

	//((((((((((((((( Pan right with D )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
	}

	//((((((((((((((( Pan up with Q )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(UP, gDeltaTime);
	}

	//((((((((((((((( Pan down with E )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);
	}

	//(((((((((((((( Change to orthographic camera views with O )))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
	{
		bOrthographicProjection = true;

		g_pCamera->Position = glm::vec3(0.0f, 2.0, 16.0f); // ^ y = backs away
		g_pCamera->Up = glm::vec3(0.0f, 5.0f, 10.0f);
		g_pCamera->Front = glm::vec3(0.0f, 0.7f, -1.0f);
	}
	//((((((((((( Change to perspective projection )))))))))))))))))))))))))
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS) 
	{
		bOrthographicProjection = false;

		g_pCamera->Position = glm::vec3(0.0f, 12.0f, 22.0f);
		g_pCamera->Front = glm::vec3(0.0f, -2.0f, -8.0f);
		g_pCamera->Up = glm::vec3(0.0f, 4.0f, 0.0f);
		g_pCamera->Zoom = 80;
	}

}


/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the 
	// event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// define the current projection matrix
	if (bOrthographicProjection == false)
	{
		// perspective projection
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}
	else
	{
		// front-view orthographic projection with correct aspect ratio
		double scale = 0.0;
		if (WINDOW_WIDTH > WINDOW_HEIGHT)
		{
			scale = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
			projection = glm::ortho(-5.0f, 5.0f, -5.0f * (float)scale, 5.0f * (float)scale, 0.1f, 100.0f);
		}
		else if (WINDOW_WIDTH < WINDOW_HEIGHT)
		{
			scale = (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;
			projection = glm::ortho(-5.0f * (float)scale, 5.0f * (float)scale, -5.0f, 5.0f, 0.1f, 100.0f);
		}
		else
		{
			projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
		}
	}

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}
}