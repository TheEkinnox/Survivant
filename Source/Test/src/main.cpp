#include <SurvivantCore/Debug/Assertion.h>
#include <SurvivantCore/Utility/FileSystem.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Core/Camera.h>
#include <SurvivantRendering/Core/Color.h>
#include <SurvivantRendering/Core/Light.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IFrameBuffer.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>
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

constexpr const char* UNLIT_SHADER_PATH = "assets/shaders/Unlit.glsl";
constexpr const char* LIT_SHADER_PATH   = "assets/shaders/Lit.glsl";

constexpr float  CAM_MOVE_SPEED     = 3.f;
constexpr Radian CAM_ROTATION_SPEED = 90_deg;

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

std::shared_ptr<IFrameBuffer> g_frameBuffer;

ITexture& GetDefaultFrameBuffer()
{
    static std::shared_ptr<ITexture> color = ITexture::Create(800, 600, EPixelDataFormat::RGB);
    static std::shared_ptr<ITexture> depth = ITexture::Create(800, 600, EPixelDataFormat::DEPTH_COMPONENT);

    static bool isInitialized = false;

    if (isInitialized)
        return *color;

    color->Bind(0);

    g_frameBuffer = IFrameBuffer::Create();
    g_frameBuffer->Attach(*color, EFrameBufferAttachment::COLOR);
    g_frameBuffer->Attach(*depth, EFrameBufferAttachment::DEPTH);

    isInitialized = true;
    return *color;

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

std::tuple<> SpaceTranslate(char /*c*/)
{
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

    //const GLuint textureId = Texture();
    App::Window::m_textureId = dynamic_cast<OpenGLTexture&>(GetDefaultFrameBuffer()).GetId();

    Model model;

    ASSERT(model.Load("assets/models/cube.obj"), "Failed to load model");
    ASSERT(model.Init(), "Failed to initialize model");

    std::shared_ptr<IShader> unlitShader = IShader::Create();
    ASSERT(unlitShader->Load(UNLIT_SHADER_PATH), "Failed to load shader at path \"%s\"", UNLIT_SHADER_PATH);
    ASSERT(unlitShader->Init(), "Failed to initialize shader at path \"%s\"", UNLIT_SHADER_PATH);

    unlitShader->SetUniformTexture("u_diffuse", &GetTexture());

    std::shared_ptr<IShader> litShader = IShader::Create();
    ASSERT(litShader->Load(LIT_SHADER_PATH), "Failed to load shader at path \"%s\"", LIT_SHADER_PATH);
    ASSERT(litShader->Init(), "Failed to initialize shader at path \"%s\"", LIT_SHADER_PATH);

    litShader->SetUniformTexture("u_diffuse", &GetTexture());

    const Matrix4 projMat = perspectiveProjection(90_deg, 4.f / 3.f, .01f, 14.f);

    Vector3   camPos(0.f, 1.8f, 2.f);
    Transform camTransform(camPos, Quaternion::identity(), Vector3::one());

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

    std::vector<Matrix4> lightMatrices;
    lightMatrices.emplace_back(Light(cam.GetClearColor()).getMatrix());
    lightMatrices.emplace_back(DirectionalLight(Color::magenta, Vector3::back()).getMatrix());
    lightMatrices.emplace_back(SpotLight(Color(0.f, 1.f, 0.f, 3.f), camPos, Vector3::front(), Attenuation(10),
        { cos(0_deg), cos(30_deg) }).getMatrix());
    lightMatrices.emplace_back(PointLight(Light{ Color::red }, Vector3{ -1, 1, 1 }, Attenuation(16)).getMatrix());

    std::shared_ptr<IShaderStorageBuffer> lightsSSBO = IShaderStorageBuffer::Create(EAccessMode::STREAM_DRAW, 0);
    lightsSSBO->Bind();
    lightsSSBO->SetData(lightMatrices.data(), lightMatrices.size());

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

        g_frameBuffer->Bind();
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
            litShader->Bind();
            litShader->SetUniformMat4("u_mvp", viewProjection * testModelMat);
            litShader->SetUniformMat4("u_modelMat", testModelMat);
            litShader->SetUniformMat4("u_normalMat", testModelMat.inverse().transposed());
            litShader->SetUniformVec4("u_tint", Color::white);
            litShader->SetUniformVec3("u_viewPos", camTransform.getWorldPosition());
            litShader->SetUniformVec4("u_specularColor", Color(.2f, .2f, .2f));
            litShader->SetUniformFloat("u_shininess", 32.f);
            DrawModel(model);
        }

        g_frameBuffer->Unbind();
        renderAPI.SetViewport({ 0, 0 }, { 800, 600 });

        window.RenderUI();
        window.EndRender();

        //glfwSwapBuffers(window.GetWindow());
    }

    //glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
