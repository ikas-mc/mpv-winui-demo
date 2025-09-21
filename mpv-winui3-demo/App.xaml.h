#pragma once

#include "App.xaml.g.h"

namespace winrt::mpv_winui3_demo::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
    };
} // namespace winrt::mpv_winui3_demo::implementation
