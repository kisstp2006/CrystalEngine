#include "CrystalEditor.h"

namespace CE::Editor
{

    AssetImportProgressPopup::AssetImportProgressPopup()
    {
        m_AutoClose = false;
        m_BlockInteraction = true;

        m_Padding = Vec4(1, 1, 1, 1) * 25;
    }

    void AssetImportProgressPopup::Construct()
    {
        Super::Construct();

        Child(
            FNew(FVerticalStack)
            .Gap(10)
            .HAlign(HAlign::Fill)
            .VAlign(VAlign::Fill)
            (
                FNew(FHorizontalStack)
                .HAlign(HAlign::Fill)
                (
                    FAssignNew(FLabel, leftLabel)
                    .Text("Loading...")
                    .FontSize(12),

                    FNew(FWidget)
                    .FillRatio(1.0f),

                    FAssignNew(FLabel, rightLabel)
                    .Text("")
                    .FontSize(12)
                ),

                FNew(FStyledWidget)
                .Background(Color::RGBA(16, 16, 16))
                .HAlign(HAlign::Fill)
                .Height(10)
                (
                    FNew(FHorizontalStack)
                    .HAlign(HAlign::Fill)
                    .VAlign(VAlign::Fill)
                    (
                        FAssignNew(FStyledWidget, progressBar)
                        .Background(Color::RGBA(0, 112, 224))
                        .VAlign(VAlign::Fill)
                        .HAlign(HAlign::Left)
                        .FillRatio(0.0f),

                        FAssignNew(FWidget, fillerWidget)
                        .FillRatio(1.0f)
                    )
                )
            )
        );

        Style("Popup.AssetImportProgress");
    }

    void AssetImportProgressPopup::UpdateProgress(AssetProcessor* assetProcessor)
    {
        if (assetProcessor->IsInProgress())
        {
            int totalJobs = assetProcessor->GetTotalJobs() + assetProcessor->GetImportQueueSize();
            int finishedJobs = assetProcessor->GetFinishedJobs();

            if (totalJobs == 0)
            {
                leftLabel->Text("Finishing...");
                rightLabel->Text("100%");
                progressBar->FillRatio(1.0f);
                fillerWidget->FillRatio(0.0f);

                ClosePopup();
            }
            else
            {
                leftLabel->Text(String::Format("Processing Asset {}/{}...", finishedJobs, totalJobs));
                rightLabel->Text(String::Format("{}%", finishedJobs * 100 / totalJobs));
                progressBar->FillRatio(Math::Clamp01((f32)finishedJobs / (f32)totalJobs));
                fillerWidget->FillRatio(1 - progressBar->FillRatio());
            }
        }
        else
        {
            leftLabel->Text("Finishing...");
            rightLabel->Text("100%");
            progressBar->FillRatio(1.0f);
            fillerWidget->FillRatio(0.0f);

            ClosePopup();
        }
    }
}

