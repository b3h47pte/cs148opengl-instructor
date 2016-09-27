#include "assignment2/Assignment2.h"
#include "common/core.h" // <-- haha.
#include "common/Utility/Mesh/Simple/PrimitiveCreator.h"
#include "common/Utility/Mesh/Loading/MeshLoader.h"
#include <cmath>
#include <fstream>

namespace
{
const int SHADER_ERROR_LOG_SIZE = 500;
bool VerifyShaderCompile(GLuint newShaderObject)
{
    GLint compileStatus;
    OGL_CALL(glGetShaderiv(newShaderObject, GL_COMPILE_STATUS, &compileStatus));
    if (compileStatus != GL_TRUE) {
        char errorLogBuffer[SHADER_ERROR_LOG_SIZE];
        OGL_CALL(glGetShaderInfoLog(newShaderObject, SHADER_ERROR_LOG_SIZE, NULL, errorLogBuffer));
        std::cerr << "ERROR: Shader compilation failure -- " << std::endl << errorLogBuffer << std::endl;
        return false;
    }
    return true;
}

bool VerifyProgramLink(GLuint shaderProgram)
{
    GLint linkStatus;
    OGL_CALL(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus));
    if (linkStatus != GL_TRUE) {
        char errorLogBuffer[SHADER_ERROR_LOG_SIZE];
        OGL_CALL(glGetProgramInfoLog(shaderProgram, SHADER_ERROR_LOG_SIZE, NULL, errorLogBuffer));
        std::cerr << "ERROR: Program link compilation failure -- " << std::endl << errorLogBuffer << std::endl;
        return false;
    }
    return true;
}

GLuint LoadShader(const std::string& text, GLenum type)
{
    GLuint newShaderObject = OGL_CALL(glCreateShader(type));
    // Need to associate the actual shader text we loaded ('shaderText') with the shader we just
    // made so that the driver knows what to compile!
    const char* shaderTextStr = text.c_str();
    OGL_CALL(glShaderSource(newShaderObject, 1, &shaderTextStr, NULL));
    // Compile the shader and make sure compilation passes! Will spit out compilation debugging information if it does not.
    OGL_CALL(glCompileShader(newShaderObject));
    return newShaderObject;
}

GLuint CreateProgram(GLuint vertex, GLuint fragment)
{
    // Now link the vertex and fragment shader together to create the shader program.
    GLuint shaderProgram = OGL_CALL(glCreateProgram());
    OGL_CALL(glAttachShader(shaderProgram, vertex));
    OGL_CALL(glAttachShader(shaderProgram, fragment));
    OGL_CALL(glLinkProgram(shaderProgram));
    return shaderProgram;
}

}

Assignment2::Assignment2(std::shared_ptr<class Scene> inputScene, std::shared_ptr<class Camera> inputCamera):
    Application(std::move(inputScene), std::move(inputCamera))
{
    vertexPositions = 
        std::initializer_list<glm::vec4>({
            // Triangle 1
            {0.f, 0.f, 0.f, 1.f},
            {1.f, 0.f, 0.f, 1.f},
            {1.f, 1.f, 0.f, 1.f},
            // Triangle 2
            {0.f, 0.f, 0.f, 1.f},
            {-1.f, 1.f, 0.f, 1.f},
            {-1.f, 0.f, 0.f, 1.f}
        });

    time = 0.f;
}

std::unique_ptr<Application> Assignment2::CreateApplication(std::shared_ptr<class Scene> scene, std::shared_ptr<class Camera> camera)
{
    return make_unique<Assignment2>(std::move(scene), std::move(camera));
}

glm::vec2 Assignment2::GetWindowSize() const
{
    return glm::vec2(800.f, 800.f);
}

void Assignment2::SetupScene()
{
    SetupExample1();
}

void Assignment2::SetupCamera()
{
}

void Assignment2::HandleInput(SDL_Keysym key, Uint32 state, Uint8 repeat, double timestamp, double deltaTime)
{   
    Application::HandleInput(key, state, repeat, timestamp, deltaTime);
}

void Assignment2::HandleWindowResize(float x, float y)
{
    Application::HandleWindowResize(x, y);
}

void Assignment2::SetupExample1()
{
    // Insert "Load and Compile Shaders" code here.
    const std::string vertFilename = std::string(STRINGIFY(SHADER_PATH)) + "/hw2/hw2.vert";
    std::ifstream vertFs(vertFilename, std::ifstream::in);
    std::string vertText((std::istreambuf_iterator<char>(vertFs)), 
        std::istreambuf_iterator<char>());
    vertFs.close();

    const std::string fragFilename = std::string(STRINGIFY(SHADER_PATH)) + "/hw2/hw2.frag";
    std::ifstream fragFs(fragFilename, std::ifstream::in);
    std::string fragText((std::istreambuf_iterator<char>(fragFs)), 
        std::istreambuf_iterator<char>());
    fragFs.close();

    // Checkpoint 1.
    // Modify this part to contain your vertex shader ID, fragment shader ID, and shader program ID.
    const GLuint vertexShaderId = LoadShader(vertText, GL_VERTEX_SHADER);
    const GLuint fragmentShaderId = LoadShader(fragText, GL_FRAGMENT_SHADER);
    shaderProgram = CreateProgram(vertexShaderId, fragmentShaderId);
    const GLuint shaderProgramId = shaderProgram;

    // DO NOT EDIT OR REMOVE THE CODE IN THIS SECTION
    if (!VerifyShaderCompile(vertexShaderId) || !VerifyShaderCompile(fragmentShaderId) || !VerifyProgramLink(shaderProgramId)) {
        std::cout << "FAILURE: Checkpoint 1 failed." << std::endl;
    } else {
        std::cout << "SUCCESS: Checkpoint 1 completed." << std::endl;
    }

    OGL_CALL(glDetachShader(shaderProgramId, vertexShaderId));
    OGL_CALL(glDeleteShader(vertexShaderId));
    OGL_CALL(glDetachShader(shaderProgramId, fragmentShaderId));
    OGL_CALL(glDeleteShader(fragmentShaderId));
    // FINISH DO NOT EDIT OR REMOVE THE CODE IN THIS SECTION

    // Insert "Setup Buffers" code here.
    OGL_CALL(glGenVertexArrays(1, &vao));
    OGL_CALL(glBindVertexArray(vao));

    GLuint positionBufferId;
    OGL_CALL(glGenBuffers(1, &positionBufferId));
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, positionBufferId));
    OGL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertexPositions.size(), &vertexPositions[0], GL_STATIC_DRAW));
    OGL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
    OGL_CALL(glEnableVertexAttribArray(0));
}

void Assignment2::Tick(double deltaTime)
{
    // Insert "Send Buffers to the GPU" and "Slightly-More Advanced Shaders" code here.
    time += deltaTime;

    OGL_CALL(glUseProgram(shaderProgram));

    const GLint uniformLoc = OGL_CALL(glGetUniformLocation(shaderProgram, "inputTime"));
    OGL_CALL(glUniform1f(uniformLoc, time));

    OGL_CALL(glBindVertexArray(vao));
    OGL_CALL(glDrawArrays(GL_TRIANGLES, 0, vertexPositions.size()));
}
