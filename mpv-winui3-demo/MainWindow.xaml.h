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

        void PlayButton_Click(winrt::Windows::Foundation::IInspectable const& sender,
                              winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PlayerView_SizeChanged(Windows::Foundation::IInspectable const&,
                                    winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& args);

        void ApplyColorSettings();

        DisplayColorMode GetTargetColorInfo(HWND hwnd);

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
