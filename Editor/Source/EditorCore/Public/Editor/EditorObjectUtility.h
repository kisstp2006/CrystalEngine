#pragma once

namespace CE
{
    struct FieldReference
    {
        WeakRef<Object> target;
        Name fieldPath;
    };

    CLASS()
    class EDITORCORE_API EditorObjectUtility : public Object
    {
        CE_CLASS(EditorObjectUtility, Object)
    protected:

        EditorObjectUtility();

        void OnAfterConstruct() override;
        
    public:

        virtual ~EditorObjectUtility();

        static Ref<EditorObjectUtility> Get();

        //! Remove references of 'object' from searchRoot's and its sub-object's fields.
        int RemoveObjectReference(Ref<Object> object, Ref<Object> searchRoot);

    };
    
} // namespace CE

#include "EditorObjectUtility.rtti.h"
