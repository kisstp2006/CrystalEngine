#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FWindow : public FCompoundWidget
    {
        CE_CLASS(FWindow, FCompoundWidget)
    public:

        FWindow();

        bool SupportsMouseEvents() const override { return true; }

        void ShowWindow();
        void HideWindow();

        PlatformWindow* GetPlatformWindow();

    protected:

        void OnPaint(FPainter* painter) override;

        virtual void OnMaximized() {}
        virtual void OnRestored() {}
        virtual void OnMinimized() {}

    public: // - Fusion Properties -

        FUSION_PROPERTY(f32, Opacity);

        FUSION_WIDGET;
    };
    
}

#include "FWindow.rtti.h"
