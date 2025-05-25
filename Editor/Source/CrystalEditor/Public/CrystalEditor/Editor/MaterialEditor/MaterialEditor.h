#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API MaterialEditor : public EditorBase
    {
        CE_CLASS(MaterialEditor, EditorBase)
    protected:

        MaterialEditor();

        void Construct() override;

    public: // - Public API -

        ClassType* GetTargetObjectType() const override;

        bool CanEdit(Ref<Object> targetObject) const override;

        bool OpenEditor(Ref<Object> targetObject) override;

        static Ref<MaterialEditor> Open(const CE::Name& materialAssetPath);

        Ref<Object> GetTargetObject() const override { return targetMaterial; }

    private: // - Internal -

        FUNCTION()
        void SaveMaterialToDisk();

        void SetMaterial(Ref<CE::Material> material);

        Ref<CE::Scene> viewportScene;

        Ref<CE::Material> targetMaterial;

        Ref<FSplitBox> rootSplitBox;
        Ref<EditorMinorDockspace> center;
        Ref<EditorMinorDockspace> bottom;
        Ref<EditorMinorDockspace> right;

        Ref<EditorViewportTab> viewportTab;
        Ref<MaterialDetailsTab> detailsTab;
        Ref<AssetBrowser> assetBrowserTab;

        Ref<StaticMeshComponent> sphereMeshComponent;

        static HashMap<Uuid, WeakRef<MaterialEditor>> materialEditorsBySourceAssetUuid;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "MaterialEditor.rtti.h"
