
#include "Fusion.h"
#include "VulkanRHI.h"

#include "FusionTest.h"

#include <gtest/gtest.h>

#include "Fusion_Test.private.h"

#define TEST_BEGIN TestBegin(false)
#define TEST_END TestEnd(false)

#define TEST_BEGIN_GUI TestBegin(true)
#define TEST_END_GUI TestEnd(true)

static CE::JobManager* gJobManager = nullptr;
static CE::JobContext* gJobContext = nullptr;

using namespace CE;
using namespace WidgetTests;

static int windowWidth = 0;
static int windowHeight = 0;

static void TestBegin(bool gui)
{
	CERegisterModuleTypes();
}
static void TestEnd(bool gui)
{
	CEDeregisterModuleTypes();
}

TEST(Fusion, MainTest)
{
	TEST_BEGIN_GUI;
	using namespace WidgetTests;

	FusionStandaloneApplication app{};

	PlatformWindowInfo windowInfo{};
	windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
	windowInfo.resizable = true;
	windowInfo.hidden = true;
	windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

#if PLATFORM_MAC
	u32 w = 600, h = 500;
#elif PLATFORM_LINUX
	u32 w = 1024, h = 768;
#elif PLATFORM_WINDOWS
	u32 w = 1400, h = 1000;
#endif

	app.Launch<FusionTestWindow>(w, h, windowInfo);

	TEST_END_GUI;
}

