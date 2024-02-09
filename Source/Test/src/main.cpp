#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Utility/FileSystem.h"
#include "SurvivantCore/Utility/Timer.h"

#include "SurvivantRendering/Core/Camera.h"
#include "SurvivantRendering/Core/Color.h"
#include "SurvivantRendering/Core/Vertex.h"
#include "SurvivantRendering/Core/VertexAttributes.h"
#include "SurvivantRendering/Core/Buffers/IndexBuffer.h"
#include "SurvivantRendering/Core/Buffers/VertexBuffer.h"
#include "SurvivantRendering/Geometry/BoundingBox.h"
#include "SurvivantRendering/Resources/Shader.h"

// TODO: Implement relevant parts in rendering lib to get rid of glad/glfw dependencies
#include <glad/gl.h>
#include <GLFW/glfw3.h>

using namespace LibMath;
using namespace SvCore::Utility;
using namespace SvRendering::Core;
using namespace SvRendering::Core::Buffers;
using namespace SvRendering::Geometry;
using namespace SvRendering::Resources;

constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";

std::pair<const Vertex*, const uint32_t*> MakeCube()
{
    constexpr float normalLength = 0.57735027f;	// normal length = 1 / sqrt(3) ~= 0.57735027f

    static const Vertex vertices[]
    {
        { { -.5f, -.5f, .5f }, { -normalLength, -normalLength, normalLength }, { 0, 0 } },   // (0) Front-bottom-left
        { { -.5f, .5f, .5f }, { -normalLength, normalLength, normalLength }, { 0, 1 } },     // (1) Front-top-left
        { { -.5f, -.5f, -.5f }, { -normalLength, -normalLength, -normalLength }, { 1, 0 } }, // (2) Back-bottom-left
        { { -.5f, .5f, -.5f }, { -normalLength, normalLength, -normalLength }, { 1, 1 } },   // (3) Back-top-left
        { { .5f, -.5f, .5f }, { normalLength, -normalLength, normalLength }, { 0, 0 } },     // (4) Front-bottom-right
        { { .5f, .5f, .5f }, { normalLength, normalLength, normalLength }, { 0, 1 } },       // (5) Front-top-right
        { { .5f, -.5f, -.5f }, { normalLength, -normalLength, -normalLength }, { 1, 0 } },   // (6) Back-bottom-right
        { { .5f, .5f, -.5f }, { normalLength, normalLength, -normalLength }, { 1, 1 } }      // (7) Back-top-right
    };

    static constexpr uint32_t indices[]
    {
        // Left
        0, 1, 3,
        0, 3, 2,

        // Back
        2, 3, 7,
        2, 7, 6,

        // Right
        6, 7, 5,
        6, 5, 4,

        // Front
        4, 5, 1,
        4, 1, 0,

        // Bottom
        2, 6, 4,
        2, 4, 0,

        // Top
        7, 3, 1,
        7, 1, 5
    };

    return { vertices, indices };
}

GLuint GetDefaultTexture()
{
    static GLuint textureId;

    if (textureId == 0)
    {
        constexpr GLsizei width  = 1;
        constexpr GLsizei height = 1;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        Vector4 color(1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, color.getArray());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    return textureId;
}

void DrawCube(const VertexAttributes& p_vao)
{
    p_vao.Bind();

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

BoundingBox GetCubeBoundingBox(const Matrix4& transform)
{
    BoundingBox boundingBox
    {
        { -.5f, -.5f, -.5f },
        { .5f, .5f, .5f }
    };

    Vector3 corners[8]
    {
        boundingBox.m_min,
        { boundingBox.m_min.m_x, boundingBox.m_max.m_y, boundingBox.m_min.m_z },
        { boundingBox.m_min.m_x, boundingBox.m_min.m_y, boundingBox.m_max.m_z },
        { boundingBox.m_min.m_x, boundingBox.m_max.m_y, boundingBox.m_max.m_z },
        { boundingBox.m_max.m_x, boundingBox.m_min.m_y, boundingBox.m_max.m_z },
        { boundingBox.m_max.m_x, boundingBox.m_max.m_y, boundingBox.m_min.m_z },
        { boundingBox.m_max.m_x, boundingBox.m_min.m_y, boundingBox.m_min.m_z },
        boundingBox.m_max
    };

    boundingBox.m_min = Vector3(std::numeric_limits<float>::max());
    boundingBox.m_max = Vector3(std::numeric_limits<float>::lowest());

    for (Vector3& corner : corners)
    {
        corner            = (transform * Vector4(corner, 1.f)).xyz();
        boundingBox.m_min = min(boundingBox.m_min, corner);
        boundingBox.m_max = max(boundingBox.m_max, corner);
    }

    return boundingBox;
}

int main()
{
    SvCore::Debug::Logger::GetInstance().SetFile("debug.log");

    ASSERT(SetWorkingDirectory(GetApplicationDirectory()), "Failed to update working directory");
    SV_LOG("Current working directory: \"%s\"", GetWorkingDirectory().c_str());

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Test", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    ASSERT(gladLoadGL(glfwGetProcAddress), "Failed to initialize glad");

    glEnable(GL_DEPTH_TEST);

    const auto [vertices, indices] = MakeCube();

    const VertexBuffer     vbo(vertices, 8);
    const IndexBuffer      ebo(indices, 36); // 6 indices * 6 faces
    const VertexAttributes vao(vbo, ebo);

    const GLuint textureId = GetDefaultTexture();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    Shader unlitShader;
    ASSERT(unlitShader.Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
    ASSERT(unlitShader.Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

    unlitShader.Use();
    unlitShader.SetUniformInt("u_diffuse", 0);

    const Matrix4 projMat = perspectiveProjection(90_deg, 4.f / 3.f, .01f, 14.f);

    Vector3       camPos(0.f, 1.8f, 2.f);
    const Vector3 testPos      = camPos + Vector3::front();
    const Matrix4 testModelMat = translation(testPos) * scaling(.1f, .5f, 1.5f);

    Camera cam(projMat);

    Degree angle;

    cam.SetClearColor(Color::black);

    Timer timer;

    while (!glfwWindowShouldClose(window))
    {
        timer.tick();
        glfwPollEvents();

        angle += 20_deg * timer.getDeltaTime();

        const Matrix4 modelRot  = rotation(angle, Vector3::up());
        const Matrix4 modelMat1 = translation(-1.f, .5f, 0.f);
        const Matrix4 modelMat2 = translation(1.f, .5f, 0.f) * modelRot;

        camPos.rotate(20_deg * timer.getDeltaTime(), Vector3::up());
        cam.SetView(lookAt(camPos, Vector3::zero(), Vector3::up()));
        cam.Clear();

        const Frustum camFrustum = cam.GetFrustum();

        if (camFrustum.Intersects(testPos))
            SV_LOG("Point %s is in cam frustum", testPos.string().c_str());

        const Matrix4 viewProjection = cam.GetViewProjection();

        unlitShader.Use();
        unlitShader.SetUniformMat4("u_mvp", viewProjection * modelMat1);
        unlitShader.SetUniformVec4("u_tint", Color::white);
        DrawCube(vao);

        unlitShader.SetUniformMat4("u_mvp", viewProjection * modelMat2);
        unlitShader.SetUniformVec4("u_tint", Color::red);
        DrawCube(vao);

        if (camFrustum.Intersects(GetCubeBoundingBox(testModelMat)))
        {
            SV_LOG("Cullable model is visible");
            unlitShader.SetUniformMat4("u_mvp", viewProjection * testModelMat);
            unlitShader.SetUniformVec4("u_tint", Color::yellow);
            DrawCube(vao);
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
