#include "CrystalEditor.h"

namespace CE::Editor
{

    AssetBrowserGridView::AssetBrowserGridView()
    {

    }

    void AssetBrowserGridView::Construct()
    {
        Super::Construct();


    }

    void AssetBrowserGridView::DeselectAll()
    {
        for (AssetBrowserItem* item : items)
        {
            item->Deselect();
        }
    }

    int AssetBrowserGridView::GetSelectedItemCount()
    {
        int count = 0;
        for (AssetBrowserItem* item : items)
        {
            if (item->IsActive())
                count++;
        }
        return count;
    }

    Array<AssetBrowserItem*> AssetBrowserGridView::GetSelectedItems()
    {
        Array<AssetBrowserItem*> selection;
        for (AssetBrowserItem* item : items)
        {
	        if (item->IsActive())
	        {
                selection.Add(item);
	        }
        }
        return selection;
    }

    void AssetBrowserGridView::OnModelUpdate()
    {
        QueueDestroyAllChildren();
        items.Clear();

        if (!m_Model)
            return;

        PathTreeNode* currentDirectory = m_Model->GetCurrentDirectory();

        if (currentDirectory != nullptr)
        {
            for (int i = 0; i < currentDirectory->children.GetSize(); i++)
            {
                AssetBrowserItem* item = nullptr;

                AddChild(
                    FAssignNew(AssetBrowserItem, item)
                    .Owner(this)
                    .OnDoubleClick([this](FSelectableButton* button)
                    {
                        if (auto registry = AssetRegistry::Get())
                        {
                            PathTree& tree = registry->GetCachedPathTree();
                            AssetBrowserItem* item = static_cast<AssetBrowserItem*>(button);
                            CE::Name path = item->GetFullPath();
                            PathTreeNode* curNode = tree.GetNode(path);
                            if (curNode == nullptr)
                                return;

                            if (auto owner = m_Owner.Lock())
                            {
                                if (curNode->nodeType == PathTreeNodeType::Directory)
                                {
                                    owner->SetCurrentPath(path);
                                }
                                else if (curNode->nodeType == PathTreeNodeType::Asset)
                                {
                                    // Do nothing when double-clicking an asset
                                }
                            }
                        }
                    })
                );

                items.Add(item);

                m_Model->SetData(*item, currentDirectory->children[i]);
            }
        }
    }

    void AssetBrowserGridView::OnSelectionUpdated()
    {
        for (AssetBrowserItem* item : items)
        {
            item->Deselect();
        }
    }

    void AssetBrowserGridView::OnBackgroundRightClicked(Vec2 globalMousePos)
    {
        Ref<EditorMenuPopup> contextMenu = BuildNoSelectionContextMenu();

        GetContext()->PushLocalPopup(contextMenu.Get(), globalMousePos, Vec2());
    }

    Ref<EditorMenuPopup> AssetBrowserGridView::BuildNoSelectionContextMenu()
    {
        Ref<AssetBrowser> owner = m_Owner.Lock();
        if (!owner || owner->IsCurrentDirectoryReadOnly())
            return nullptr;

        Ref<EditorMenuPopup> contextMenu = CreateObject<EditorMenuPopup>(this, "ContextMenu");

        contextMenu->AutoClose(true);

        contextMenu->Content(
            FNew(FMenuItemSeparator)
            .Title("FOLDER"),

            FNew(FMenuItem)
            .Text("New Folder")
            .IconEnabled(true)
            .Icon(FBrush("/Editor/Assets/Icons/NewFolder"))
            .FontSize(10)
            .ContentPadding(Vec4(5, 0, 5, 0))
            .OnClick([this]
            {
	            if (auto owner = m_Owner.Lock())
	            {
		            
	            }
            })
        );

        return contextMenu;
    }

    void AssetBrowserGridView::HandleEvent(FEvent* event)
    {
        KeyModifier ctrlMod = KeyModifier::Ctrl;
        if (PlatformMisc::GetCurrentPlatform() == PlatformName::Mac)
        {
            ctrlMod = KeyModifier::Gui; // Gui is Cmd on mac
        }

        if (event->IsMouseEvent())
        {
            auto mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->sender == this && mouseEvent->buttons == MouseButtonMask::Left &&
                mouseEvent->type == FEventType::MousePress)
            {
                mouseEvent->Consume(this);

                DeselectAll();
            }
            else if (mouseEvent->sender == this && mouseEvent->buttons == MouseButtonMask::Right && 
                mouseEvent->type == FEventType::MousePress)
            {
                mouseEvent->Consume(this);

                if (EnumHasAnyFlags(mouseEvent->keyModifiers, KeyModifier::Shift | ctrlMod))
                {
                    // Right-click on items
                }
                else
                {
                    DeselectAll();

                    OnBackgroundRightClicked(mouseEvent->mousePosition);
                }
            }
        }

        Super::HandleEvent(event);

        if (event->IsMouseEvent())
        {
            auto mouseEvent = static_cast<FMouseEvent*>(event);

            if (mouseEvent->sender != nullptr && mouseEvent->sender->IsOfType<AssetBrowserItem>() && 
                mouseEvent->buttons == MouseButtonMask::Right && mouseEvent->type == FEventType::MousePress)
            {
                Array<AssetBrowserItem*> selection = GetSelectedItemCount();

                if (selection.GetSize() == 1) // Single selection
                {
	                
                }
                else if (selection.GetSize() > 1) // Multiple selection
                {
	                
                }
            }
        }
    }

    
}

