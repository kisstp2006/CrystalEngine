#pragma once

namespace CE
{

    class FUSION_API FusionStandaloneApplication : public ApplicationMessageHandler
    {
    public:
        FusionStandaloneApplication();

        virtual ~FusionStandaloneApplication();

		void Run(Ref<FWindow> mainWindow, u32 width, u32 height, const PlatformWindowInfo& windowInfo);

    private:

		void SetupDefaultStyle();

		void Init();
		void Shutdown();

		void Render();

		void RebuildFrameGraph();

		void BuildFrameGraph();
		void CompileFrameGraph();

		void OnWindowRestored(PlatformWindow* window) override;
		void OnWindowDestroyed(PlatformWindow* window) override;
		void OnWindowClosed(PlatformWindow* window) override;
		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
		void OnWindowMinimized(PlatformWindow* window) override;
		void OnWindowCreated(PlatformWindow* window) override;
		void OnWindowExposed(PlatformWindow* window) override;

		HashMap<u64, Vec2i> windowSizesById;
		RHI::FrameScheduler* scheduler = nullptr;

		bool rebuildFrameGraph = true;
		bool recompileFrameGraph = true;
		int curImageIndex = 0;
		f32 deltaTime = 0;
		clock_t previousTime = {};

		RHI::DrawListContext drawList{};
		CE::JobManager* jobManager = nullptr;
		CE::JobContext* jobContext = nullptr;
    };
    
} // namespace CE
