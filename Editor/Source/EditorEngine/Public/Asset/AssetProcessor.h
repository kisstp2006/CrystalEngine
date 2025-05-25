#pragma once

namespace CE
{
    CLASS()
    class EDITORENGINE_API AssetProcessor : public Object
    {
        CE_CLASS(AssetProcessor, Object)
    protected:

        AssetProcessor();

        void OnAfterConstruct() override;

        void OnBeginDestroy() override;
        
    public:

        virtual ~AssetProcessor();

        bool IsInProgress() const { return inProgress; }

        void TerminateAllJobs();

        void Initialize();

        void Shutdown();

        void RunAll();

        void Tick();

        int GetTotalJobs() const { return totalScheduledJobs; }
        int GetFinishedJobs() const { return totalFinishedJobs; }
        int GetSuccessfulJobs() const { return totalSuccessfulJobs; }

        ScriptEvent<void(AssetProcessor*)> onProgressUpdate;

    protected:

        void OnJobFinished(bool success);

        bool inProgress = false;

        IO::Path inputRoot;
        IO::Path scanPath;
        IO::Path tempPath;

        Array<IO::Path> includePaths;

        Array<IO::Path> allSourceAssetPaths{};
        Array<IO::Path> allProductAssetPaths{};

        SharedMutex mainThreadDispatcherLock;
        Array<Delegate<void(void)>> mainThreadDispatcher;

        int totalScheduledJobs = 0;
        int totalFinishedJobs = 0;
        int totalSuccessfulJobs = 0;

        bool validScanPath = false;


        // - Import Processes -

        HashMap<AssetDefinition*, Array<IO::Path>> sourcePathsByAssetDef{};
        HashMap<AssetDefinition*, Array<IO::Path>> productPathsByAssetDef{};

        // Paths to product assets to be generated in this run
        HashSet<IO::Path> productAssetPaths{};

        Array<Ref<AssetImporter>> importers{};
        Array<AssetDefinition*> assetDefinitions{};
    };
    
} // namespace CE

#include "AssetProcessor.rtti.h"
