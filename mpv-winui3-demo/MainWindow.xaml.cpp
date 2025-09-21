#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
    #include "MainWindow.g.cpp"
#endif
#include <microsoft.ui.xaml.window.h>

// for DisplayInformation
#include <windows.graphics.display.interop.h>
#include <winrt/windows.graphics.display.h>
//

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::mpv_winui3_demo::implementation
{
    MainWindow::MainWindow() : m_mpvPlayer(std::make_unique<mpv::MpvPlayer>())
    {
    }

    void MainWindow::InitializeComponent()
    {
        MainWindowT::InitializeComponent();

        m_mpvPlayer->Initialize(1, 1);
        m_mpvPlayer->SetSwapChainPanel(PlayerView());
    }

    void MainWindow::PlayButton_Click([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender,
                                      [[maybe_unused]] winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        auto url = UrlInput().Text();
        if (url.empty())
        {
            return;
        }

        ApplyColorSettings();

        m_mpvPlayer->Play(url);
    }

    void MainWindow::PlayerView_SizeChanged([[maybe_unused]] IInspectable const& sender,
                                            SizeChangedEventArgs const& args)
    {
        auto width = static_cast<uint32_t>(args.NewSize().Width * PlayerView().CompositionScaleX());
        auto height = static_cast<uint32_t>(args.NewSize().Height * PlayerView().CompositionScaleY());
        m_mpvPlayer->UpdateSize(width, height);
    }

    void MainWindow::PlayerView_CompositionScaleChanged([[maybe_unused]] SwapChainPanel const& sender,
                                                        [[maybe_unused]] IInspectable const& args)
    {
        auto scaleX = PlayerView().CompositionScaleX();
        auto scaleY = PlayerView().CompositionScaleY();
        m_mpvPlayer->UpdateSwapChainScale(scaleX, scaleY);
    }

    DisplayColorMode MainWindow::GetTargetColorInfoWinrt(HWND hwnd)
    {
        DisplayColorMode colorMode{};

        // must create winrt::Windows::System::DispatcherQueueController before
        // see App.xaml.cpp
        // TODO use displayInformation.AdvancedColorInfoChanged to listen color mode changed event
        winrt::Windows::Graphics::Display::DisplayInformation displayInformation{ nullptr };
        auto result = winrt::get_activation_factory<winrt::Windows::Graphics::Display::DisplayInformation,
                                                    ::IDisplayInformationStaticsInterop>()
                          ->GetForWindow(
                              hwnd, winrt::guid_of<decltype(displayInformation)>(), winrt::put_abi(displayInformation));
        if (FAILED(result))
        {
            return colorMode;
        }

        auto colorInfo = displayInformation.GetAdvancedColorInfo();
        auto kind = colorInfo.CurrentAdvancedColorKind();
        switch (kind)
        {
            case winrt::Windows::Graphics::Display::AdvancedColorKind::StandardDynamicRange:
                break;
            case winrt::Windows::Graphics::Display::AdvancedColorKind::WideColorGamut:
                colorMode.isWcg = true;
                break;
            case winrt::Windows::Graphics::Display::AdvancedColorKind::HighDynamicRange:
                colorMode.isHdr = true;
                break;
            default:
                break;
        }

        return colorMode;
    }

    DisplayColorMode MainWindow::GetTargetColorInfoWin32(HWND hwnd)
    {
        DisplayColorMode colorMode{};
        HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFOEXW monitorInfo;
        ZeroMemory(&monitorInfo, sizeof(monitorInfo));
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (!GetMonitorInfoW(hMonitor, &monitorInfo))
            return colorMode;

        UINT32 pathCount, modeCount;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS)
            return colorMode;

        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);

        if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), NULL) !=
            ERROR_SUCCESS)
            return colorMode;

        for (UINT32 i = 0; i < pathCount; i++)
        {
            const DISPLAYCONFIG_PATH_INFO& path = paths[i];
            LUID adapterId = path.targetInfo.adapterId;
            UINT32 targetId = path.targetInfo.id;

            DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
            sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            sourceName.header.size = sizeof(sourceName);
            sourceName.header.adapterId = adapterId;
            sourceName.header.id = path.sourceInfo.id;

            if (DisplayConfigGetDeviceInfo(&sourceName.header) == ERROR_SUCCESS)
            {
                if (wcscmp(sourceName.viewGdiDeviceName, monitorInfo.szDevice) == 0)
                {
                    DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO_2 colorInfo{};
                    colorInfo.header.type =
                        DISPLAYCONFIG_DEVICE_INFO_TYPE::DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO_2;
                    colorInfo.header.size = sizeof(colorInfo);
                    colorInfo.header.adapterId = adapterId;
                    colorInfo.header.id = targetId;

                    if (DisplayConfigGetDeviceInfo(&colorInfo.header) == ERROR_SUCCESS)
                    {
                        colorMode.isHdr = colorInfo.activeColorMode ==
                                          DISPLAYCONFIG_ADVANCED_COLOR_MODE::DISPLAYCONFIG_ADVANCED_COLOR_MODE_HDR;
                        colorMode.isWcg = colorInfo.activeColorMode ==
                                          DISPLAYCONFIG_ADVANCED_COLOR_MODE::DISPLAYCONFIG_ADVANCED_COLOR_MODE_WCG;
                        return colorMode;
                    }
                }
            }
        }

        return colorMode;
    }

    void MainWindow::ApplyColorSettings()
    {
        HWND hwnd{ nullptr };
        this->try_as<IWindowNative>()->get_WindowHandle(&hwnd);

        auto colorMode1 = GetTargetColorInfoWinrt(hwnd);
        auto colorMode = GetTargetColorInfoWin32(hwnd);

        if (colorMode.isHdr)
        {
            m_mpvPlayer->ApplyColorOptionsHDR();
        }
        else if (colorMode.isWcg)
        {
            m_mpvPlayer->ApplyColorOptionsWCG();
        }
        else
        {
            m_mpvPlayer->ApplyColorOptionsSDR();
        }
    }

} // namespace winrt::mpv_winui3_demo::implementation
