
#include "CoreApplication.h"
#include "MetalRHI.h"

#include "MetalRHITest.h"

#include <gtest/gtest.h>

using namespace CE;

#define WINDOW_TEST_BEGIN WindowTestBegin()
#define WINDOW_TEST_END WindowTestEnd()

static void WindowTestBegin()
{
    gProjectName = MODULE_NAME;
    gProjectPath = PlatformDirectories::GetLaunchDir();

    ModuleManager::Get().LoadModule("Core");
    ModuleManager::Get().LoadModule("CoreApplication");
    ModuleManager::Get().LoadModule("CoreMedia");
    ModuleManager::Get().LoadModule("CoreRHI");
    ModuleManager::Get().LoadModule("MetalRHI");

    auto app = PlatformApplication::Get();
    app->Initialize();
    
    InputManager::Get().Initialize(app);

    gDefaultWindowWidth = 1000;
    gDefaultWindowHeight = 600;
    
    PlatformWindowInfo windowInfo{
        .maximised = false,
        .fullscreen = false,
        .resizable = false,
        .hidden = false,
        .windowFlags = PlatformWindowFlags::DestroyOnClose
    };
    
    PlatformWindow* mainWindow = app->InitMainWindow(MODULE_NAME, gDefaultWindowWidth, gDefaultWindowHeight, windowInfo);

    RHI::gDynamicRHI = new CE::Metal::MetalRHI();
    //RHI::gDynamicRHI->AddValidationCallbackHandler(OnValidationMessage, RHI::ValidationMessageType::Warning);

    RHI::gDynamicRHI->Initialize();
    RHI::gDynamicRHI->PostInitialize();
}

static void WindowTestEnd()
{
    auto app = PlatformApplication::Get();

    RHI::gDynamicRHI->PreShutdown();

    InputManager::Get().Shutdown(app);
    app->PreShutdown();

    RHI::gDynamicRHI->Shutdown();

    app->Shutdown();

    delete RHI::gDynamicRHI;
    RHI::gDynamicRHI = nullptr;

    delete app;
    app = nullptr;

    ModuleManager::Get().UnloadModule("MetalRHI");
    ModuleManager::Get().UnloadModule("CoreRHI");
    ModuleManager::Get().UnloadModule("CoreMedia");
    ModuleManager::Get().UnloadModule("CoreApplication");
    ModuleManager::Get().UnloadModule("Core");
}

TEST(RHI, MetalBasic)
{
    WINDOW_TEST_BEGIN;
    
    PlatformApplication* app = PlatformApplication::Get();
    PlatformWindow* mainWindow = app->GetMainWindow();
    
    auto rhi = RHI::gDynamicRHI;
    
    u32 width = 0;
    u32 height = 0;
    
    while (!IsEngineRequestingExit())
    {
        app->Tick();
        InputManager::Get().Tick();
        
    }
    
    WINDOW_TEST_END;
}
