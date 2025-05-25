#include "CrystalEditor.h"

namespace CE::Editor
{

    CrystalEditorSplashWindow::CrystalEditorSplashWindow()
    {

    }

    void CrystalEditorSplashWindow::Construct()
    {
        Super::Construct();

        titleBar->Height(30);
        titleBarLabel->FontSize(13);

        titleBar->Enabled(false);
        borderWidget->Padding(Vec4(1, 1, 1, 1));

        constexpr auto splashImage = "/Editor/Assets/UI/SplashScreen";

        FBrush splash = FBrush(splashImage);
        splash.SetBrushTiling(FBrushTiling::None);
        splash.SetImageFit(FImageFit::Cover);
        splash.SetBrushPosition(Vec2(0.5f, 0.5f));

        (*this)
        .Title("Splash Screen")
        .MinimizeEnabled(false)
        .MaximizeEnabled(false)
        .ContentPadding(Vec4(1, 1, 1, 1) * 0)
        .Content(
            FNew(FOverlayStack)
            .HAlign(HAlign::Fill)
            .FillRatio(1.0f)
            (
                FNew(FImage)
                .Background(splash)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill),

                FNew(FStyledWidget)
                .Background(Color::RGBHex(0x090C10))
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Bottom)
                .Padding(Vec4(1.5f, 1, 1.5f, 1) * 10)
                (
                    FNew(FVerticalStack)
                    .ContentHAlign(HAlign::Left)
                    .Gap(7.5f)
                    .HAlign(HAlign::Fill)
                    .VAlign(VAlign::Fill)
                    (
                        FNew(FLabel)
                        .Text("Crystal Engine - " + gProjectName)
                        .FontSize(14),

                        FNew(FLabel)
                        .Text("Crystal Editor " CE_ENGINE_VERSION_STRING_SHORT)
                        .FontSize(10)
                        .Foreground(Color::RGBA(134, 134, 134)),

                        FAssignNew(FLabel, progressLabel)
                        .Text("Loading...")
                        .FontSize(10)
                        .Foreground(Color::RGBA(134, 134, 134))
                    )
                )
            )
        );
    }

    void CrystalEditorSplashWindow::Launch()
    {
        Ref<AssetProcessor> assetProcessor = CrystalEditorModule::Get()->GetAssetProcessor();

        if (assetProcessor)
        {
            assetProcessor->onProgressUpdate.Bind(FUNCTION_BINDING(this, OnAssetProcessorUpdate));

            assetProcessor->RunAll();

            int totalJobs = assetProcessor->GetTotalJobs();
            int finishedJobs = assetProcessor->GetFinishedJobs();

            if (totalJobs > 0)
            {
                progressLabel->Text(String::Format("{}% - Processing Assets ({})...", (finishedJobs * 100) / totalJobs, totalJobs - finishedJobs));
            }
            else
            {
                timer = CreateObject<FTimer>(this, "Timer");
                timer->OnTimeOut(FUNCTION_BINDING(this, OnTimeOut));

                timer->SetSingleShot(true);
                timer->Start(1000);
            }
        }
    }

    void CrystalEditorSplashWindow::OnAssetProcessorUpdate(AssetProcessor* assetProcessor)
    {
        if (assetProcessor)
        {
            if (!assetProcessor->IsInProgress())
            {
                progressLabel->Text("100% - Processing Assets (0)...");

                assetProcessor->onProgressUpdate.Unbind(FUNCTION_BINDING(this, OnAssetProcessorUpdate));

                GetContext()->QueueDestroy();

                CrystalEditorWindow::Launch();
            }
            else
            {
                int totalJobs = assetProcessor->GetTotalJobs();
                int finishedJobs = assetProcessor->GetFinishedJobs();

                progressLabel->Text(String::Format("{}% - Processing Assets ({})...", (finishedJobs * 100) / totalJobs, totalJobs - finishedJobs));
            }
        }
    }

    void CrystalEditorSplashWindow::OnTimeOut()
    {
        GetContext()->QueueDestroy();

        CrystalEditorWindow::Launch();
    }
}

