#include "EditorCore.h"

namespace CE
{
    constexpr u32 StampFileVersion = 2;

    static WeakRef<AssetProcessor> instance = nullptr;

    AssetProcessor::AssetProcessor()
    {

    }

    AssetProcessor::~AssetProcessor()
    {

    }

    void AssetProcessor::OnAfterConstruct()
    {
        Super::OnAfterConstruct();

        if (instance == nullptr)
        {
            instance = this;
        }
    }

    void AssetProcessor::OnBeginDestroy()
    {
        Super::OnBeginDestroy();

        if (instance == this)
        {
            instance = nullptr;
        }
    }

    void AssetProcessor::TerminateAllJobs()
    {

    }

    void AssetProcessor::Initialize()
    {
        scanPath = gProjectPath / "Game/Assets";
        validScanPath = true;
    	inputRoot = gProjectPath;

        if (!scanPath.Exists())
        {
            scanPath = "";
            validScanPath = false;
            return;
        }

        tempPath = gProjectPath / "Temp/AssetCache";

        if (!tempPath.Exists())
        {
            IO::Path::CreateDirectories(tempPath);
        }

    	includePaths = {
        	EngineDirectories::GetEngineInstallDirectory() / "Engine/Shaders"
    	};
    }

    void AssetProcessor::Shutdown()
    {
    }

    void AssetProcessor::RunAll()
    {
        if (!validScanPath)
            return;

    	totalScheduledJobs = 0;
    	totalFinishedJobs = 0;
    	totalSuccessfulJobs = 0;

        allSourceAssetPaths.Clear();
        allProductAssetPaths.Clear();

    	sourcePathsByAssetDef.Clear();
    	productPathsByAssetDef.Clear();

    	productAssetPaths.Clear();
    	importers.Clear();
    	assetDefinitions.Clear();

        scanPath.RecursivelyIterateChildren([&](const IO::Path& path)
        {
            if (path.IsDirectory())
                return;

            String fileName = path.GetFileName().GetString();
            String extension = path.GetExtension().GetString();
            if (extension == ".cmake" || fileName == "CMakeLists.txt" || extension == ".casset")
                return;

            DateTime lastWriteTime = path.GetLastWriteTime();

            IO::Path relativePath = IO::Path::GetRelative(path, inputRoot);
            String relativePathStr = relativePath.GetString().Replace({ '\\' }, '/');
            if (relativePathStr.NotEmpty() && relativePathStr[0] != '/')
                relativePathStr.InsertAt('/', 0);

            IO::Path stampFilePath = tempPath / relativePath.ReplaceExtension(".stamp");

            if (!stampFilePath.GetParentPath().Exists())
            {
                IO::Path::CreateDirectories(stampFilePath.GetParentPath());
            }

            IO::Path productPath = path.ReplaceExtension(".casset");

            AssetData* productAssetData = AssetRegistry::Get()->GetAssetBySourcePath(relativePathStr);

            if (productAssetData != nullptr)
            {
                productPath = Bundle::GetAbsoluteBundlePath(productAssetData->bundlePath);
            }

            if (!stampFilePath.Exists() || !productPath.Exists())
            {
                allSourceAssetPaths.Add(path);
                allProductAssetPaths.Add(productPath);
            }
            else
            {
                FileStream reader = FileStream(stampFilePath, Stream::Permissions::ReadOnly);
                reader.SetBinaryMode(true);

                bool needsProcessing = false;

                u32 version = 0;
                reader >> version;

                if (version != StampFileVersion)
                {
                    needsProcessing = true;
                }

                u64 stampedTimeInt = 0;
                reader >> stampedTimeInt;

                DateTime stampedTime = DateTime::FromNumber(stampedTimeInt);
                if (lastWriteTime != stampedTime)
                {
                    needsProcessing = true;
                }

                if (version >= 1)
                {
                    u32 major = 0, minor = 0, patch = 0;

                    reader >> major;
                    reader >> minor;
                    reader >> patch;

                    u32 assetDefinitionVersion = 0;
                    reader >> assetDefinitionVersion;

                    AssetDefinition* assetDefinition = GetAssetDefinitionRegistry()->FindAssetDefinition(extension);
                    if (assetDefinition && assetDefinition->GetAssetVersion() != assetDefinitionVersion)
                    {
                        needsProcessing = true;
                    }

                    if (major != Bundle::GetCurrentMajor() || minor != Bundle::GetCurrentMinor() || patch != Bundle::GetCurrentPatch())
                    {
                        needsProcessing = true;
                    }

                    if (version >= 2)
                    {
                        u32 assetImporterVersion = 0;
                        reader >> assetImporterVersion;

                        if (assetDefinition)
                        {
                            AssetImporter* importerCDI = (AssetImporter*)assetDefinition->GetAssetImporterClass()->GetDefaultInstance();
                            if (importerCDI != nullptr && importerCDI->GetImporterVersion() != assetImporterVersion)
                            {
                                needsProcessing = true;
                            }
                        }
                    }
                }

                if (needsProcessing)
                {
                    allSourceAssetPaths.Add(path);
                    allProductAssetPaths.Add(productPath);
                }
            }
        });

        AssetDefinitionRegistry* assetDefRegistry = GetAssetDefinitionRegistry();

        for (int i = 0; i < allSourceAssetPaths.GetSize(); i++)
        {
            auto sourcePath = allSourceAssetPaths[i];
            auto productPath = allProductAssetPaths[i];
            sourcePath = sourcePath.GetString().Replace({ '\\' }, '/');
            productPath = productPath.GetString().Replace({ '\\' }, '/');

            if (!sourcePath.Exists())
                continue;

            String extension = sourcePath.GetExtension().GetString().ToLower();

            AssetDefinition* assetDef = assetDefRegistry->FindAssetDefinition(extension);

            if (assetDef != nullptr)
            {
                sourcePathsByAssetDef[assetDef].Add(sourcePath);
                productPathsByAssetDef[assetDef].Add(productPath);
                productAssetPaths.Add(productPath);
            }
        }

        struct ImportJobEntry
        {
            HashSet<IO::Path> productAssetDependencies{};
            AssetImportJob* job = nullptr;
        };

        Array<ImportJobEntry> importJobs{};
        HashMap<IO::Path, AssetImportJob*> importJobsByProductPath{};

    	//JobCompletion jobCompletion = JobCompletion();

        for (const auto& [assetDef, sourcePaths] : sourcePathsByAssetDef)
		{
			if (assetDef == nullptr || sourcePaths.IsEmpty())
				continue;
			if (!productPathsByAssetDef.KeyExists(assetDef) ||
				productPathsByAssetDef[assetDef].GetSize() != sourcePaths.GetSize())
				continue;

			ClassType* assetImporterClass = assetDef->GetAssetImporterClass();
			if (assetImporterClass == nullptr || !assetImporterClass->CanBeInstantiated())
				continue;
			Ref<AssetImporter> assetImporter = CreateObject<AssetImporter>(nullptr, "AssetImporter", OF_Transient, assetImporterClass);
			if (assetImporter == nullptr)
				continue;

			// Get product asset dependencies required by the asset importer.

			Array<Name> productAssetDependencies = assetImporter->GetProductAssetDependencies();
			HashSet<IO::Path> productDependencies{};
			for (const Name& productAssetName : productAssetDependencies)
			{
				IO::Path assetPath = Bundle::GetAbsoluteBundlePath(productAssetName);
				if (productAssetPaths.Exists(assetPath))
				{
					productDependencies.Add(assetPath);
				}
				else if (assetPath.Exists()) // Nothing needed to be done
				{

				}
				else
				{
					CE_LOG(Warn, All, "Could not process assets of type {}. Product asset dependency not found: {}.",
						assetImporterClass->GetName(), productAssetName);
					continue;
				}
			}

			assetImporter->SetIncludePaths(includePaths);
			assetImporter->SetLogging(true);
			assetImporter->SetTargetPlatform(PlatformMisc::GetCurrentPlatform());
			assetImporter->SetTempDirectoryPath(tempPath);

			importers.Add(assetImporter);
			assetDefinitions.Add(assetDef);

			Array<AssetImportJob*> jobs = assetImporter->PrepareImportJobs(sourcePaths, productPathsByAssetDef[assetDef]);

			for (AssetImportJob* job : jobs)
			{
				job->SetAutoDelete(true);
				importJobsByProductPath[job->GetProductPath()] = job;

				totalScheduledJobs++;

				job->onFinish.Bind([this](AssetImportJob* job)
				{
					LockGuard guard{ mainThreadDispatcherLock };

					WeakRef<AssetProcessor> selfRef = this;
					bool success = job->Succeeded();

					mainThreadDispatcher.Add([selfRef, success]
					{
						if (Ref<AssetProcessor> self = selfRef.Lock())
						{
							self->OnJobFinished(success);
						}
					});
				});

				Array<Name> perJobProductDependencies = job->PrepareProductAssetDependencies();
				HashSet<IO::Path> finalProductDependencies{};

				for (const auto& dependency : perJobProductDependencies)
				{
					finalProductDependencies.Add(dependency.GetString());
				}
				for (const auto& dependency : productDependencies)
				{
					finalProductDependencies.Add(dependency);
				}

				importJobs.Add({ finalProductDependencies, job });

				//job->SetDependent(&jobCompletion);
			}
		}

		// Setup job dependencies
		for (const ImportJobEntry& entry : importJobs)
		{
			for (const IO::Path& productDependency : entry.productAssetDependencies)
			{
				if (importJobsByProductPath[productDependency] != nullptr)
				{
					importJobsByProductPath[productDependency]->SetDependent(entry.job);
				}
			}
		}

    	inProgress = true;

		// Launch the jobs
		for (const ImportJobEntry& entry : importJobs)
		{
			entry.job->Start();
		}

		// Wait for all jobs to complete
    	//jobCompletion.StartAndWaitForCompletion();
    }

    void AssetProcessor::Tick()
    {
    	LockGuard guard{ mainThreadDispatcherLock };

	    for (int i = mainThreadDispatcher.GetSize() - 1; i >= 0; --i)
	    {
	    	mainThreadDispatcher[i].InvokeIfValid();

		    mainThreadDispatcher.RemoveAt(i);
	    }
    }

    void AssetProcessor::OnJobFinished(bool success)
    {
    	totalFinishedJobs++;

    	if (success)
    	{
    		totalSuccessfulJobs++;
    	}

    	if (totalFinishedJobs == totalScheduledJobs)
    	{
    		inProgress = false;
    	}

    	onProgressUpdate.Broadcast(this);

    	if (totalFinishedJobs == totalScheduledJobs)
    	{
    		// We finished executing all the jobs!

    		Array<AssetImportJobResult> assetImportResults{};
    		Array<u32> assetDefinitionVersions{};
    		Array<u32> assetImporterVersions{};

    		for (int i = 0; i < importers.GetSize(); ++i)
    		{
    			int count = importers[i]->GetResults().GetSize();
    			assetImportResults.AddRange(importers[i]->GetResults());
    			for (int j = 0; j < count; ++j)
    			{
    				assetDefinitionVersions.Add(assetDefinitions[i]->GetAssetVersion());
    				assetImporterVersions.Add(importers[i]->GetImporterVersion());
    			}
    			importers[i]->BeginDestroy();
    		}
    		importers.Clear();

    		int failCounter = 0;

    		// Update time stamps
    		{
    			for (int i = 0; i < assetImportResults.GetSize(); i++)
    			{
    				const auto& [success, sourcePath, productPath, errorMessage] = assetImportResults[i];

    				IO::Path relativePath = IO::Path::GetRelative(sourcePath, inputRoot);
    				IO::Path stampFilePath = tempPath / relativePath.ReplaceExtension(".stamp");
    				if (success)
    				{
    					FileStream writer = FileStream(stampFilePath, Stream::Permissions::WriteOnly);
    					writer.SetBinaryMode(true);

    					writer << StampFileVersion;
    					writer << sourcePath.GetLastWriteTime().ToNumber();

    					writer << Bundle::GetCurrentMajor();
    					writer << Bundle::GetCurrentMinor();
    					writer << Bundle::GetCurrentPatch();

    					writer << assetDefinitionVersions[i];

    					writer << assetImporterVersions[i];
    				}
    				else
    				{
    					failCounter++;
    				}
    			}
    		}

    		totalFinishedJobs = 0;
    		totalScheduledJobs = 0;
    		totalSuccessfulJobs = 0;
    	}
    }
} // namespace CE

