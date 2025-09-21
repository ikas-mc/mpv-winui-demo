#pragma once

#include "MainWindow.g.h"
#include "MpvPlayer.h"

namespace winrt::mpv_winui3_demo::implementation
{
    struct DisplayColorMode
    {
        bool isWcg;
        bool isHdr;
    };

    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void InitializeComponent();

        void PlayButton_Click(winrt::Windows::Foundation::IInspectable const&,
                              winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);
        void PlayerView_SizeChanged(Windows::Foundation::IInspectable const&,
                                    winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const&);
        void PlayerView_CompositionScaleChanged(winrt::Microsoft::UI::Xaml::Controls::SwapChainPanel const&,
                                                winrt::Windows::Foundation::IInspectable const&);

        DisplayColorMode GetTargetColorInfoWin32(HWND hwnd);
        DisplayColorMode GetTargetColorInfoWinrt(HWND hwnd);
        void ApplyColorSettings();

    private:
        std::unique_ptr<mpv::MpvPlayer> m_mpvPlayer;
    };
}; // namespace winrt::mpv_winui3_demo::implementation

namespace winrt::mpv_winui3_demo::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
} // namespace winrt::mpv_winui3_demo::factory_implementation
