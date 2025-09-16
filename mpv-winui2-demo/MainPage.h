#pragma once

#include <MpvPlayer.h>
#include <mpv/client.h>
#include "MainPage.g.h"

namespace winrt::mpv_winui2_demo::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        void InitializeComponent();
        void PlayButton_Click(winrt::Windows::Foundation::IInspectable const&,
                              winrt::Windows::UI::Xaml::RoutedEventArgs const&);
        void PlayerView_SizeChanged(Windows::Foundation::IInspectable const&,
                                    winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args);
        void ApplyColorSettings();

    private:
        std::unique_ptr<mpv::MpvPlayer> m_mpvPlayer;
    };
} // namespace winrt::mpv_winui2_demo::implementation

namespace winrt::mpv_winui2_demo::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
} // namespace winrt::mpv_winui2_demo::factory_implementation
