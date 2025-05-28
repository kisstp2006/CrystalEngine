#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API AssetImportProgressPopup : public FPopup
    {
        CE_CLASS(AssetImportProgressPopup, FPopup)
    protected:

        AssetImportProgressPopup();

        void Construct() override;

    public: // - Public API -

        void UpdateProgress(AssetProcessor* assetProcessor);

    protected: // - Internal -

        Ref<FLabel> leftLabel, rightLabel;
        Ref<FStyledWidget> progressBar;
        Ref<FWidget> fillerWidget;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "AssetImportProgressPopup.rtti.h"
