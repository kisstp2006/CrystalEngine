#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API CrystalEditorSplashWindow : public FToolWindow
    {
        CE_CLASS(CrystalEditorSplashWindow, FToolWindow)
    protected:

        CrystalEditorSplashWindow();

        void Construct() override;

    public: // - Public API -

        void Launch();

    protected: // - Internal -

        FUNCTION()
        void OnAssetProcessorUpdate(AssetProcessor* assetProcessor);

        FUNCTION()
        void OnTimeOut();

        Ref<FTimer> timer;
        Ref<FLabel> progressLabel;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "CrystalEditorSplashWindow.rtti.h"
