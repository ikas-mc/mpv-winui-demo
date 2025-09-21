#include "pch.h"
#include "MainPage.h"
#include <winrt/Windows.Graphics.Display.h>
#include "MainPage.g.cpp"
using namespace winrt;
using namespace Windows::UI::Xaml;
#if defined(_MSC_VER)
    #include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
namespace winrt::mpv_winui2_demo::implementation
{
    MainPage::MainPage() : m_mpvPlayer(std::make_unique<mpv::MpvPlayer>())
    {
    }

    void MainPage::InitializeComponent()
    {
        MainPageT::InitializeComponent();

        m_mpvPlayer->Initialize(1, 1);
        m_mpvPlayer->SetSwapChainPanel(PlayerView());
    }

    void MainPage::PlayButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto url = UrlInput().Text();
        if (url.empty())
        {
            return;
        }

        ApplyColorSettings();

        m_mpvPlayer->Play(url);
    }

    void MainPage::PlayerView_SizeChanged(IInspectable const&, SizeChangedEventArgs const& args)
    {
        auto width = static_cast<uint32_t>(args.NewSize().Width * PlayerView().CompositionScaleX());
        auto height = static_cast<uint32_t>(args.NewSize().Height * PlayerView().CompositionScaleY());
        m_mpvPlayer->UpdateSize(width, height);
    }

    void MainPage::PlayerView_CompositionScaleChanged(SwapChainPanel const& sender, IInspectable const& args)
    {
        auto scaleX = PlayerView().CompositionScaleX();
        auto scaleY = PlayerView().CompositionScaleY();
        m_mpvPlayer->UpdateSwapChainScale(scaleX, scaleY);
    }

    void MainPage::ApplyColorSettings()
    {
        // TODO use displayInformation.AdvancedColorInfoChanged to listen color mode changed event
        auto displayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
        auto colorInfo = displayInformation.GetAdvancedColorInfo();

        switch (colorInfo.CurrentAdvancedColorKind())
        {
            case winrt::Windows::Graphics::Display::AdvancedColorKind::HighDynamicRange:
                m_mpvPlayer->ApplyColorOptionsHDR();
                break;
            case winrt::Windows::Graphics::Display::AdvancedColorKind::WideColorGamut:
                m_mpvPlayer->ApplyColorOptionsWCG();
                break;
            default:
                m_mpvPlayer->ApplyColorOptionsSDR();
                break;
        }
    }
} // namespace winrt::mpv_winui2_demo::implementation
