#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneOutlinerTab::SceneOutlinerTab()
    {

    }

    void SceneOutlinerTab::Construct()
    {
        Super::Construct();

        ConstructMinorDockWindow();

        CE::Scene* scene = gEngine->GetActiveScene();

        const f32 fontSize = GetDefaults<EditorConfigs>()->GetFontSize();

        treeViewModel = CreateObject<SceneTreeViewModel>(this, "TreeViewModel");
        
        (*this)
		.Title("Scene Outliner")
		.Child(
            FAssignNew(SceneTreeView, treeView)
            .Header(
                FNew(FTreeViewHeader)
                .Columns(
                    FNew(FTreeViewHeaderColumn)
                    .Title("Name")
                    .FillRatio(0.5f),

                    FNew(FTreeViewHeaderColumn)
                    .Title("Type")
                    .FillRatio(0.5f)
                )
            )
            .RowHeight(25)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
        );

        if (scene)
        {
            SetScene(scene);
        }

        treeView->ApplyStyleRecursively();
    }

    void SceneOutlinerTab::OnBeginDestroy()
    {
	    Super::OnBeginDestroy();

        if (IsDefaultInstance())
            return;

        if (treeViewModel && treeViewModel->GetScene() != nullptr)
        {
            treeViewModel->GetScene()->RemoveSceneCallbacks(this);
        }
    }

    void SceneOutlinerTab::OnSceneHierarchyUpdated(CE::Scene* scene)
    {
        treeView->OnModelUpdate();
    }

    void SceneOutlinerTab::SetScene(CE::Scene* scene)
    {
        if (treeViewModel->GetScene() == scene)
            return;

        if (treeViewModel->GetScene() != nullptr)
        {
            treeViewModel->GetScene()->RemoveSceneCallbacks(this);
        }

        scene->AddSceneCallbacks(this);

        treeViewModel->SetScene(scene);
        treeView->Model(treeViewModel);
    }
    
}

