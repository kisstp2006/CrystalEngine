#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API CrystalEditorWindow : public EditorDockspace
    {
        CE_CLASS(CrystalEditorWindow, EditorDockspace)
    public:

        // - Public API -

        static CrystalEditorWindow* Get();

        //! Internal use only!
        static void Launch();

    protected:

        CrystalEditorWindow();

        void Construct() override;

        void OnBeginDestroy() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "CrystalEditorWindow.rtti.h"
