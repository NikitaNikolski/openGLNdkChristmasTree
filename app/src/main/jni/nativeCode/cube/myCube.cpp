#include "myShader.h"
#include "myCube.h"

MyCube::MyCube() {

    MyLOGD("MyCube::MyCube");
    initsDone = false;

    // create MyGLCamera object and set default position for the object
    myGLCamera = new MyGLCamera();
    float pos[]={0.,0.,0.,1,1,0.};
    std::copy(&pos[0], &pos[5], std::back_inserter(modelDefaultPosition));
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

MyCube::~MyCube() {

    MyLOGD("MyCube::~MyCube");
    if (myGLCamera) {
        delete myGLCamera;
    }
}

/**
 * Perform inits and load the triangle's vertices/colors to GLES
 */
void MyCube::PerformGLInits() {

    MyLOGD("MyCube::PerformGLInits");

    MyGLInits();

    GLfloat cubeVertices[] = {
            -0.2f, 0.0f, 0.0f,
            0.2f, 0.0f, 0.0f,
            0.0f, 0.2f, 0.0f,

            0.4f, -0.2f, 0.0f,
            -0.4f, -0.2f, 0.0f,
            0.0f, 0.0f, 0.0f,

            0.6f, -0.4f, 0.0f,
            -0.6f, -0.4f, 0.0f,
            0.0f, -0.2f, 0.0f,

            -0.1f, -0.4f, 0.0f,
            -0.1f, -0.6f, 0.0f,
            0.1f, -0.4f, 0.0f,

            -0.1f, -0.6f, 0.0f,
            0.1f, -0.6f, 0.0f,
            0.1f, -0.4f, 0.0f
    };

    // Generate a vertex buffer and load the vertices into it
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    GLfloat cubeFaceColors[] = {
            0.0f, 1.0f, 0.0f, // green
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, 1.0f, 0.0f, // green
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, 1.0f, 0.0f, // green
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.647059f, 0.164706f, 0.164706f, // brown
            0.647059f, 0.164706f, 0.164706f,
            0.647059f, 0.164706f, 0.164706f,

            0.647059f, 0.164706f, 0.164706f, // brown
            0.647059f, 0.164706f, 0.164706f,
            0.647059f, 0.164706f, 0.164706f,
    };

    // Generate a vertex buffer and load the colors into it
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeFaceColors), cubeFaceColors, GL_STATIC_DRAW);

    // shader related setup
    std::string vertexShader    = "shaders/cubeMVP.vsh";
    std::string fragmentShader  = "shaders/cubeMVP.fsh";

    // compile the vertex and fragment shaders, and link them together
    shaderProgramID = LoadShaders(vertexShader, fragmentShader);
    // fetch the locations of "vertexPosition" and "vertexColor" from the shader
    vertexAttribute = GetAttributeLocation(shaderProgramID, "vertexPosition");
    colorAttribute  = GetAttributeLocation(shaderProgramID, "vertexColor");
    MVPLocation     = GetUniformLocation(shaderProgramID, "mvpMat");

    CheckGLError("Cube::PerformGLInits");
    initsDone = true;
}

/**
 * Render our colorful cube
 */
void MyCube::RenderCube() {
    // use the shader
    glUseProgram(shaderProgramID);

    glm::mat4 mvpMat = myGLCamera->GetMVP();
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, (const GLfloat *) &mvpMat);

    // enable the vertex buffer
    glEnableVertexAttribArray(vertexAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer( vertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // enable the color buffer
    glEnableVertexAttribArray(colorAttribute);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer( colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );

    // Draw the colorful triangle
    glDrawArrays(GL_TRIANGLES, 0, 15);

    glDisableVertexAttribArray(vertexAttribute);
    glDisableVertexAttribArray(colorAttribute);
}

/**
 * Render to the display
 */
void MyCube::Render() {

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderCube();
    CheckGLError("Cube::Render");

}

/**
 * set the viewport, function is also called when user changes device orientation
 */
void MyCube::SetViewport(int width, int height) {

    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("Cube::SetViewport");

    myGLCamera->SetAspectRatio((float) width / height);
}


/**
 * reset model's position in double-tap
 */
void MyCube::DoubleTapAction() {

    myGLCamera->SetModelPosition(modelDefaultPosition);
}

/**
 * rotate the model if user scrolls with one finger
 */
void MyCube::ScrollAction(float distanceX, float distanceY, float positionX, float positionY) {

    myGLCamera->RotateModel(distanceX, distanceY, positionX, positionY);
}

/**
 * pinch-zoom: move the model closer or farther away
 */
void MyCube::ScaleAction(float scaleFactor) {

    myGLCamera->ScaleModel(scaleFactor);
}

/**
 * two-finger drag: displace the model by changing its x-y coordinates
 */
void MyCube::MoveAction(float distanceX, float distanceY) {

    myGLCamera->TranslateModel(distanceX, distanceY);
}