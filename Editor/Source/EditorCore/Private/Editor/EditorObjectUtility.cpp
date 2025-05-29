#include "EditorCore.h"

namespace CE
{
    static WeakRef<EditorObjectUtility> instance = nullptr;

    EditorObjectUtility::EditorObjectUtility()
    {

    }

    void EditorObjectUtility::OnAfterConstruct()
    {
        Super::OnAfterConstruct();

        if (!IsDefaultInstance())
        {
            instance = this;
        }
    }

    EditorObjectUtility::~EditorObjectUtility()
    {
        
    }

    Ref<EditorObjectUtility> EditorObjectUtility::Get()
    {
        return instance.Lock();
    }

    int EditorObjectUtility::RemoveObjectReference(Ref<Object> searchRoot, Delegate<bool(Ref<Object>)> predicate)
    {
        ZoneScoped;

        if (!predicate.IsValid() || !searchRoot)
            return 0;

        int refCount = 0;

        for (int i = 0; i < searchRoot->GetSubObjectCount(); ++i)
        {
            Ref<Object> subObject = searchRoot->GetSubObject(i);
            if (subObject)
            {
                refCount += RemoveObjectReference(subObject, predicate);
            }
        }

        for (int i = searchRoot->GetSubObjectCount() - 1; i >= 0; i--)
        {
            Ref<Object> subObject = searchRoot->GetSubObject(i);
            if (subObject && predicate(subObject))
            {
                searchRoot->DetachSubobject(subObject.Get());
                refCount++;
            }
        }

        std::function<void(Ref<Object>, String)> visitor = [&](Ref<Object> curObject, String curFieldPath)
        {
            if (!curObject || curFieldPath.IsEmpty())
                return;

            void* curInstance = nullptr;
            Ptr<FieldType> curField = nullptr;
            bool found = curObject->GetClass()->FindFieldInstanceRelative(curFieldPath, curObject, curField, curInstance);
            if (!found)
                return;

            if (curField->IsObjectField())
            {
                if (Ref<Object> value = curField->GetFieldObjectValue(curInstance))
                {
                    if (predicate(value))
                    {
                        curField->SetFieldObjectValue(curInstance, nullptr);
                        curObject->OnFieldChanged(curFieldPath);

                        refCount++;
                    }
                }
            }
            else if (curField->IsArrayField())
            {
                if (TypeInfo* underlyingType = curField->GetUnderlyingType(); underlyingType->IsObject() || underlyingType->IsStruct())
                {
                    int arraySize = curField->GetArraySize(curInstance);

                    for (int j = 0; j < arraySize; ++j)
                    {
                        String elementFieldPath = String::Format("{}[{}]", curFieldPath, j);
                        visitor(curObject, elementFieldPath);
                    }
                }
            }
            else if (curField->IsStructField())
            {
                auto structType = (StructType*)curField->GetDeclarationType();

                for (int i = 0; i < structType->GetFieldCount(); ++i)
                {
                    Ptr<FieldType> field = structType->GetFieldAt(i);
                    if (!field->IsObjectField() && !field->IsStructField() && !field->IsArrayField())
                        continue;

                    String fieldPath = curFieldPath + "." + field->GetName().GetString();
                    visitor(curObject, fieldPath);
                }
            }
        };

        ClassType* clazz = searchRoot->GetClass();
        for (int i = 0; i < clazz->GetFieldCount(); ++i)
        {
            Ptr<FieldType> field = clazz->GetFieldAt(i);
            if (!field->IsObjectField() && !field->IsStructField() && !field->IsArrayField())
                continue;

            visitor(searchRoot, field->GetName().GetString());
        }

        return refCount;
    }
} // namespace CE

