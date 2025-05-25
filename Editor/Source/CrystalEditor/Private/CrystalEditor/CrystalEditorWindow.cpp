#include "CrystalEditor.h"

namespace CE::Editor
{
    static CrystalEditorWindow* instance = nullptr;

	CrystalEditorWindow* CrystalEditorWindow::Get()
	{
        return instance;
	}

	void CrystalEditorWindow::Launch()
	{
		auto mainWindow = PlatformApplication::Get()->GetMainWindow();
		auto rootContext = FusionApplication::Get()->GetRootContext();

		FNativeContext* crystalEditorCtx = FNativeContext::Create(mainWindow, "CrystalEditor", rootContext);
		rootContext->AddChildContext(crystalEditorCtx);

		Ref<CrystalEditorWindow> crystalEditor = nullptr;

		FAssignNewOwned(CrystalEditorWindow, crystalEditor, crystalEditorCtx);
		crystalEditorCtx->SetOwningWidget(crystalEditor.Get());

		mainWindow->SetResizable(true);
		mainWindow->Show();
	}

	CrystalEditorWindow::CrystalEditorWindow()
    {

    }

    void CrystalEditorWindow::Construct()
    {
        Super::Construct();

        instance = this;

        Ref<SceneEditor> sceneEditor = nullptr;
        FAssignNew(SceneEditor, sceneEditor);
        AddDockTab(sceneEditor.Get());

        Ref<ProjectSettings> projectSettings = GetSettings<ProjectSettings>();

        projectLabelParent->Enabled(true);
        projectLabel->Text(projectSettings->projectName);

	    if (PlatformMisc::GetCurrentPlatform() == PlatformName::Mac)
	    {
	        logo->Enabled(false);
	    }

        Style("EditorDockspace");
    }

    void CrystalEditorWindow::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }
}

