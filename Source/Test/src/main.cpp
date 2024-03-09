#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/Core/Color.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IFrameBuffer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/ITexture.h>
#include <SurvivantRendering/RHI/OpenGL/OpenGLTexture.h>

#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantApp/Inputs/KeyboardInputs.h"
#include "SurvivantApp/Inputs/MouseInputs.h"
#include "SurvivantApp/Windows/Window.h"
#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantUI/EditorWindow.h"
#include "SurvivantUI/UI.h"

#include <Transform.h>
// TODO: Implement relevant parts in corresponding libs to get rid of glfw dependency
#include <GLFW/glfw3.h>

using namespace LibMath;
using namespace SvCore::Utility;
using namespace SvRendering::Core;
using namespace SvRendering::Enums;
using namespace SvRendering::Geometry;
using namespace SvRendering::Resources;
using namespace SvRendering::RHI;

constexpr const char* UNLIT_SHADER_PATH  = "assets/shaders/Unlit.glsl";
constexpr float       CAM_MOVE_SPEED     = 3.f;
constexpr Radian      CAM_ROTATION_SPEED = 90_deg;

ITexture& GetTexture()
{
    static std::shared_ptr<ITexture> texture  = ITexture::Create();
    static bool                      isLoaded = false;

    if (!isLoaded)
    {
        ASSERT(texture->Load("assets/textures/grid.png"));
        ASSERT(texture->Init());

        texture->SetFilters(ETextureFilter::NEAREST, ETextureFilter::NEAREST);
        texture->SetWrapModes(ETextureWrapMode::REPEAT, ETextureWrapMode::REPEAT);

        isLoaded = true;
    }

    return *texture;
}

static std::shared_ptr<IFrameBuffer> frameBufferId;

ITexture& GetDefaultFrameBuffer()
{
    static std::shared_ptr<ITexture> texture = ITexture::Create(800, 600, EPixelDataFormat::RGB);
    static bool isInitialized = false;

    if (isInitialized)
        return *texture;

    frameBufferId = IFrameBuffer::Create();
    frameBufferId->attach(*texture, EFrameBufferAttachment::COLOR);

    texture->Bind(0);
    return *texture;

    //static GLuint textureId;

    //if (textureId == 0)
    //{
    //    glGenFramebuffers(1, &frameBufferId);
    //    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

    //    glGenTextures(1, &textureId);
    //    glBindTexture(GL_TEXTURE_2D, textureId);

    //    //screen width here
    //    constexpr GLsizei width = 800;
    //    constexpr GLsizei height = 600;

    //    //Vector4 c(0.5);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);


    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    //    {
    //        int i = 0; i;
    //    }
    //}

    //return textureId;
}


std::tuple<int, int> AddInputTranslate(char i)
{
    return { i, 10 };
}

std::tuple<int, int> AddMouseTranslate(float i, float j)
{
    return { (int)i, (int)j };
}

std::tuple<> SpaceTranslate(char c)
{
    c;
    return { };
}

void DrawModel(const Model& p_model)
{
    for (size_t i = 0; i < p_model.GetMeshCount(); ++i)
    {
        const Mesh& mesh = p_model.GetMesh(i);

        mesh.Bind();
        IRenderAPI::GetCurrent().DrawElements(EPrimitiveType::TRIANGLES, mesh.GetIndexCount());
    }
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

    //window
    UI::EditorWindow window;
    //GLFWwindow* windowPtr = window.GetWindow();

    IRenderAPI& renderAPI = IRenderAPI::SetCurrent(EGraphicsAPI::OPENGL);
    renderAPI.Init(true)
             .SetCapability(ERenderingCapability::DEPTH_TEST, true)
             .SetCullFace(ECullFace::BACK);

    Model model;

    ASSERT(model.Load("assets/models/cube.obj"), "Failed to load model");
    ASSERT(model.Init(), "Failed to initialize model");

    std::shared_ptr<IShader> unlitShader = IShader::Create();
    ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
    ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

    unlitShader->Bind();
    unlitShader->SetUniformTexture("u_diffuse", &GetTexture());

    const Matrix4 projMat = perspectiveProjection(90_deg, 4.f / 3.f, .01f, 14.f);

    Vector3   camPos(0.f, 1.8f, 2.f);
    Transform camTransform(camPos, Quaternion::identity(), Vector3::one());

    //const GLuint textureId = Texture();
    App::Window::m_textureId = dynamic_cast<OpenGLTexture&>(GetDefaultFrameBuffer()).GetId();

    const Vector3 testPos      = camPos + Vector3::front();
    const Matrix4 testModelMat = translation(testPos) * scaling(1.5f, .5f, .1f);

    Camera cam(projMat);

    Degree angle;

    cam.SetClearColor(Color::gray);

    Timer timer;

    //event and inputs
    using namespace Core;
    using namespace App;
    using AddEvent = Event<int, int>;
    class ToggleEvent : public Core::Event<> {};

    InputManager& im = InputManager::GetInstance();
    {
        InputManager::GetInstance().InitWindow(&window);

        EventManager& em = EventManager::GetInstance();

        AddEvent::EventDelegate printAdd = [](int i, int j) { std::cout << "Add = " << i + j << std::endl; };
        ToggleEvent::EventDelegate toggle = std::bind(&App::Window::ToggleFullScreenMode, &window);
        em.AddListenner<AddEvent>(printAdd);
        em.AddListenner<ToggleEvent>(toggle);

        InputManager::KeyboardKeyType   a(EKey::A, EKeyState::RELEASED, EInputModifier::MOD_ALT);
        InputManager::KeyboardKeyType   b(EKey::B, EKeyState::PRESSED, EInputModifier());
        InputManager::MouseKeyType      mouse(EMouseButton::MOUSE_1, EMouseButtonState::PRESSED, EInputModifier());
        InputManager::KeyboardKeyType   space(EKey::SPACE, EKeyState::PRESSED, EInputModifier());
        im.AddInputEventBinding<AddEvent>(a, &AddInputTranslate);
        im.AddInputEventBinding<AddEvent>(b, &AddInputTranslate);
        im.AddInputEventBinding<AddEvent>(mouse, &AddMouseTranslate);
        im.AddInputEventBinding<ToggleEvent>(space, &SpaceTranslate);
    }

    //ui
    UI::EditorUI ui;
    window.SetupUI(&ui);

    Vector2 moveInput, rotateInput;

    im.AddInputBinding({ EKey::W, EKeyState::PRESSED, EInputModifier() }, [&moveInput](const char)
    {
        ++moveInput.m_y;
    });

    im.AddInputBinding({ EKey::W, EKeyState::RELEASED, EInputModifier() }, [&moveInput](const char)
    {
        --moveInput.m_y;
    });

    im.AddInputBinding({ EKey::S, EKeyState::PRESSED, EInputModifier() }, [&moveInput](const char)
    {
        --moveInput.m_y;
    });

    im.AddInputBinding({ EKey::S, EKeyState::RELEASED, EInputModifier() }, [&moveInput](const char)
    {
        ++moveInput.m_y;
    });

    im.AddInputBinding({ EKey::A, EKeyState::PRESSED, EInputModifier() }, [&moveInput](const char)
    {
        --moveInput.m_x;
    });

    im.AddInputBinding({ EKey::A, EKeyState::RELEASED, EInputModifier() }, [&moveInput](const char)
    {
        ++moveInput.m_x;
    });

    im.AddInputBinding({ EKey::D, EKeyState::PRESSED, EInputModifier() }, [&moveInput](const char)
    {
        ++moveInput.m_x;
    });

    im.AddInputBinding({ EKey::D, EKeyState::RELEASED, EInputModifier() }, [&moveInput](const char)
    {
        --moveInput.m_x;
    });

    im.AddInputBinding({ EKey::UP, EKeyState::PRESSED, EInputModifier() }, [&rotateInput](const char)
    {
        ++rotateInput.m_y;
    });

    im.AddInputBinding({ EKey::UP, EKeyState::RELEASED, EInputModifier() }, [&rotateInput](const char)
    {
        --rotateInput.m_y;
    });

    im.AddInputBinding({ EKey::DOWN, EKeyState::PRESSED, EInputModifier() }, [&rotateInput](const char)
    {
        --rotateInput.m_y;
    });

    im.AddInputBinding({ EKey::DOWN, EKeyState::RELEASED, EInputModifier() }, [&rotateInput](const char)
    {
        ++rotateInput.m_y;
    });

    im.AddInputBinding({ EKey::LEFT, EKeyState::PRESSED, EInputModifier() }, [&rotateInput](const char)
    {
        --rotateInput.m_x;
    });

    im.AddInputBinding({ EKey::LEFT, EKeyState::RELEASED, EInputModifier() }, [&rotateInput](const char)
    {
        ++rotateInput.m_x;
    });

    im.AddInputBinding({ EKey::RIGHT, EKeyState::PRESSED, EInputModifier() }, [&rotateInput](const char)
    {
        ++rotateInput.m_x;
    });

    im.AddInputBinding({ EKey::RIGHT, EKeyState::RELEASED, EInputModifier() }, [&rotateInput](const char)
    {
        --rotateInput.m_x;
    });

    im.AddInputBinding({ EKey::R, EKeyState::RELEASED, {} }, [&camTransform, &camPos](const char)
    {
        camTransform.setAll(camPos, Quaternion::identity(), Vector3::one());
    });

    im.AddInputBinding({ EKey::ESCAPE, EKeyState::RELEASED, {} }, [&window](const char)
    {
        glfwSetWindowShouldClose(window.GetWindow(), true);
    });

    while (!window.ShouldClose())
    {
        timer.tick();
        //glfwPollEvents();
        window.StartRender();

        angle += 20_deg * timer.getDeltaTime();

        const Matrix4 modelRot  = rotation(angle, Vector3::up());
        const Matrix4 modelMat1 = translation(-1.f, .5f, 0.f);
        const Matrix4 modelMat2 = translation(1.f, .5f, 0.f) * modelRot;

        Vector3    newPos = camTransform.getPosition();
        Quaternion newRot = camTransform.getRotation();

        frameBufferId->bind();
        renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

        if (moveInput.magnitudeSquared() > 0.f)
        {
            const Vector3 moveDir = moveInput.m_x * camTransform.worldRight() + moveInput.m_y * camTransform.worldBack();
            newPos += moveDir.normalized() * (CAM_MOVE_SPEED * timer.getDeltaTime());
        }

        if (rotateInput.magnitudeSquared() > 0.f)
        {
            TVector3<Radian> angles = newRot.toEuler(ERotationOrder::YXZ);

            angles.m_y -= CAM_ROTATION_SPEED * rotateInput.m_x * timer.getDeltaTime();
            angles.m_x += CAM_ROTATION_SPEED * rotateInput.m_y * timer.getDeltaTime();
            angles.m_x = Degree(clamp(angles.m_x.degree(true), -85.f, 85.f));

            newRot = Quaternion::fromEuler(angles, ERotationOrder::YXZ);
        }

        camTransform.setAll(newPos, newRot, Vector3::one());

        cam.SetView(camTransform.getWorldMatrix().inverse());
        cam.Clear();

        const Frustum camFrustum     = cam.GetFrustum();
        const Matrix4 viewProjection = cam.GetViewProjection();

        unlitShader->Bind();
        unlitShader->SetUniformMat4("u_mvp", viewProjection * modelMat1);
        unlitShader->SetUniformVec4("u_tint", Color::white);
        DrawModel(model);

        unlitShader->SetUniformMat4("u_mvp", viewProjection * modelMat2);
        unlitShader->SetUniformVec4("u_tint", Color::red);
        DrawModel(model);

        if (camFrustum.Intersects(TransformBoundingBox(model.GetBoundingBox(), testModelMat)))
        {
            unlitShader->SetUniformMat4("u_mvp", viewProjection * testModelMat);
            unlitShader->SetUniformVec4("u_tint", Color::yellow);
            DrawModel(model);
        }

        frameBufferId->unbind();
        renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

        window.RenderUI();
        window.EndRender();

        //glfwSwapBuffers(window.GetWindow());
    }

    //glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
