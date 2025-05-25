#include "CrystalEditor.h"

namespace CE::Editor
{
    HashMap<Uuid, WeakRef<MaterialEditor>> MaterialEditor::materialEditorsBySourceAssetUuid{};

    MaterialEditor::MaterialEditor()
    {

    }

    void MaterialEditor::Construct()
    {
        Super::Construct();

        Title("Material");

        (*content)
        .Child(
            FAssignNew(FSplitBox, rootSplitBox)
            .Direction(FSplitDirection::Horizontal)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FNew(FSplitBox)
                .Direction(FSplitDirection::Vertical)
                .VAlign(VAlign::Fill)
                .FillRatio(0.6f)
                (
                    FAssignNew(EditorMinorDockspace, center)
                    .DockTabs(
                        FAssignNew(EditorViewportTab, viewportTab)

                    )
                    .HAlign(HAlign::Fill)
                    .FillRatio(0.6f),

                    FAssignNew(EditorMinorDockspace, bottom)
                    .DockTabs(
                        FAssignNew(AssetBrowser, assetBrowserTab)

                    )
                    .HAlign(HAlign::Fill)
                    .FillRatio(0.4f)
                ),

                FAssignNew(EditorMinorDockspace, right)
                .DockTabs(
                    FAssignNew(MaterialDetailsTab, detailsTab)

                )
                .HAlign(HAlign::Fill)
                .FillRatio(0.4f)
            )
        )
    	.Padding(Vec4(0, 5, 0, 0));

        toolBar->Content(
            EditorToolBar::NewImageButton("/Editor/Assets/Icons/Save")
            .OnClicked(FUNCTION_BINDING(this, SaveMaterialToDisk))
        );

        detailsTab->SetOwnerEditor(this);

        viewportScene = CreateObject<CE::Scene>(this, "MaterialScene");
        viewportTab->GetViewport()->SetScene(viewportScene->GetRpiScene());

        EditorViewport* viewport = viewportTab->GetViewport();
        viewport->SetName("MaterialEditorViewport");

        auto assetManager = gEngine->GetAssetManager();

        Ref<TextureCube> skybox = assetManager->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_day");

        Ref<CE::Shader> standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
        Ref<CE::Shader> skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");

        viewportScene->SetSkyboxCubeMap(skybox.Get());

        CE::Material* aluminumMaterial = CreateObject<CE::Material>(viewportScene.Get(), "AluminumMaterial");
        aluminumMaterial->SetShader(standardShader.Get());
        {
            Ref<CE::Texture> albedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
            Ref<CE::Texture> normalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
            Ref<CE::Texture> metallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
            Ref<CE::Texture> roughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

            aluminumMaterial->SetProperty("_AlbedoTex", albedoTex);
            aluminumMaterial->SetProperty("_NormalTex", normalTex);
            aluminumMaterial->SetProperty("_MetallicTex", metallicTex);
            aluminumMaterial->SetProperty("_RoughnessTex", roughnessTex);
            aluminumMaterial->ApplyProperties();
        }

        Ref<StaticMesh> sphereMesh = CreateObject<StaticMesh>(viewportScene.Get(), "Mat_SphereMesh");
        {
            RPI::ModelAsset* sphereModel = CreateObject<RPI::ModelAsset>(sphereMesh.Get(), "Mat_SphereModel");
            RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
            sphereModel->AddModelLod(sphereLodAsset);

            sphereMesh->SetModelAsset(sphereModel);
        }

        Ref<StaticMeshActor> sphereActor = CreateObject<StaticMeshActor>(viewportScene.Get(), "Mat_SphereMesh");
        viewportScene->AddActor(sphereActor.Get());
        {
            sphereMeshComponent = sphereActor->GetMeshComponent();
            sphereMeshComponent->SetStaticMesh(sphereMesh);
            sphereMeshComponent->SetLocalPosition(Vec3(0, 0, 1.5f));
            sphereMeshComponent->SetLocalEulerAngles(Vec3(0, 0, 0));
            sphereMeshComponent->SetMaterial(aluminumMaterial, 0, 0);
        }

        Ref<CameraActor> camera = CreateObject<CameraActor>(viewportScene.Get(), "Mat_Camera");
        camera->GetCameraComponent()->SetLocalPosition(Vec3(0, 0, 0));
        viewportScene->AddActor(camera.Get());

        Ref<StaticMeshActor> skyboxActor = CreateObject<StaticMeshActor>(viewportScene.Get(), "Mat_SkyboxActor");
        viewportScene->AddActor(skyboxActor.Get());
        {
            StaticMeshComponent* skyboxMeshComponent = skyboxActor->GetMeshComponent();
            skyboxMeshComponent->SetStaticMesh(sphereMesh);

            skyboxMeshComponent->SetLocalPosition(Vec3(0, 0, 0));
            skyboxMeshComponent->SetLocalScale(Vec3(1, 1, 1) * 1000);

            {
                Ref<CE::Material> skyboxMaterial = CreateObject<CE::Material>(skyboxMeshComponent, "Mat_Material");
                skyboxMaterial->SetShader(skyboxShader.Get());
                skyboxMeshComponent->SetMaterial(skyboxMaterial.Get(), 0, 0);

                skyboxMaterial->SetProperty("_CubeMap", skybox.Get());
                skyboxMaterial->ApplyProperties();
            }
        }

        // For debugging only
        viewportScene->GetRpiScene()->SetName("MaterialScene");

        gEngine->AddScene(viewportScene.Get());
        gEditor->AddRenderViewport(viewportTab->GetViewport());
    }

    ClassType* MaterialEditor::GetTargetObjectType() const
    {
        return CE::Material::StaticClass();
    }

    bool MaterialEditor::CanEdit(Ref<Object> targetObject) const
    {
        return targetObject.IsValid() && targetObject->IsOfType<CE::Material>();
    }

    bool MaterialEditor::OpenEditor(Ref<Object> targetObject)
    {
        if (!targetObject)
            return false;
        if (!targetObject->IsOfType<CE::Material>())
            return false;

        Ref<CE::Material> material = (Ref<CE::Material>)targetObject;
        if (this->targetMaterial == material)
            return true;

        this->targetMaterial = material;
        SetMaterial(material);

        return true;
    }

    Ref<MaterialEditor> MaterialEditor::Open(const CE::Name& materialAssetPath)
    {
        AssetRegistry* registry = AssetRegistry::Get();
        AssetData* assetData = registry->GetPrimaryAssetByPath(materialAssetPath);
        if (!assetData)
            return nullptr;

        Ref<CE::Material> material = gEditor->GetAssetManager()->LoadAssetAtPath<CE::Material>(materialAssetPath);

        CrystalEditorWindow* crystalEditorWindow = CrystalEditorWindow::Get();

        if (auto existingEditor = materialEditorsBySourceAssetUuid[assetData->assetUuid].Lock())
        {
            crystalEditorWindow->SelectTab(existingEditor.Get());
            existingEditor->SetMaterial(material);
            return existingEditor;
        }

        Ref<MaterialEditor> editor = CreateObject<MaterialEditor>(crystalEditorWindow, "MaterialEditor");
        materialEditorsBySourceAssetUuid[assetData->assetUuid] = editor;

        editor->SetMaterial(material);

        crystalEditorWindow->AddDockTab(editor.Get());
        crystalEditorWindow->SelectTab(editor.Get());

        return editor;
    }

    void MaterialEditor::SaveMaterialToDisk()
    {
        if (!targetMaterial)
            return;

        Ref<Bundle> bundle = targetMaterial->GetBundle();
        if (!bundle)
            return;

        if (bundle->IsTransient())
            return;

        BundleSaveResult result = Bundle::SaveToDisk(bundle);

        if (result != BundleSaveResult::Success)
        {
            CE_LOG(Error, All, "Failed to save material to disk! Error in Bundle::SaveToDisk(); ErrorCode: {}", (int)result);
        }
        else
        {
            SetAssetDirty(false);
        }
    }

    void MaterialEditor::SetMaterial(Ref<CE::Material> material)
    {
        if (!material)
            return;

        String title = material->GetName().GetString();
        if (Ref<Bundle> bundle = material->GetBundle())
        {
            title = bundle->GetName().GetString();
        }

        Title(title);

        sphereMeshComponent->SetMaterial(material.Get(), 0, 0);

        detailsTab->SetupEditor(material);

        UpdateDockspaceTabWell();
    }
}

