#include "EditorCore.h"

namespace CE::Editor
{

    EditorBase::EditorBase()
    {
        m_AllowedDockspaces = FDockspaceFilter().WithDockTypeMask(FDockTypeMask::Major);
    }

    void EditorBase::Construct()
    {
	    Super::Construct();

        history = CreateObject<EditorHistory>(this, "EditorHistory");

        history->onTargetModified.Bind([this]
        {
            SetAssetDirty(true);
        });
    }

    void EditorBase::HandleEvent(FEvent* event)
    {
        if (event->IsKeyEvent() && !event->isConsumed)
        {
            FKeyEvent* keyEvent = static_cast<FKeyEvent*>(event);

            if (keyEvent->type == FEventType::KeyPress)
            {
                KeyModifier ctrlMod = KeyModifier::Ctrl;
#if PLATFORM_MAC
                ctrlMod = KeyModifier::Cmd;
#endif

                if (EnumHasFlag(keyEvent->modifiers, ctrlMod))
                {
                    if (keyEvent->key == KeyCode::Z)
                    {
                        if (history.IsValid())
                        {
                            history->Undo();
                        }

                        event->Consume(this);
                    }
                    else if (keyEvent->key == KeyCode::Y)
                    {
                        if (history.IsValid())
                        {
                            history->Redo();
                        }
                        
                        event->Consume(this);
                    }
                    else if (keyEvent->key == KeyCode::S)
                    {
                        SaveChanges();

                        event->Consume(this);
                    }
                }
            }
        }

	    Super::HandleEvent(event);
    }

    void EditorBase::OnBeginDestroy()
    {
        Super::OnBeginDestroy();

        AssetRegistry::Get()->RemoveRegistryListener(this);
    }

    void EditorBase::OnAssetRenamed(Uuid bundleUuid, const CE::Name& oldName, const CE::Name& newName, const CE::Name& newPath)
    {
        if (this->bundleUuid == bundleUuid)
        {
            Title(newName.GetString());
        }
    }

    bool EditorBase::OpenEditor(Ref<Object> targetObject, Ref<Bundle> bundle)
    {
		this->bundle = bundle;

        return true;
    }

    void EditorBase::OnEditorOpened(Ref<Object> targetObject)
    {
        if (!targetObject)
            return;

        String title = targetObject->GetName().GetString();
        if (Ref<Bundle> bundle = targetObject->GetBundle())
        {
            title = bundle->GetName().GetString();
            bundleUuid = bundle->GetUuid();

            AssetRegistry::Get()->AddRegistryListener(this);
            registeredListener = true;
        }

        Title(title);
    }

    void EditorBase::SetAssetDirty(bool dirty)
    {
        isAssetDirty = dirty;

        String title = Title();

        if (isAssetDirty != title.EndsWith("*"))
        {
	        if (isAssetDirty)
            {
                title += "*";
            }
            else
            {
                title.RemoveAt(title.GetLength() - 1);
            }

			Title(title);
        }

    }

    void EditorBase::SaveChanges()
    {
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

			AssetRegistry::Get()->OnAssetUpdated(bundle->GetBundlePath());
        }
    }

}

