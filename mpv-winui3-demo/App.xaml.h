#pragma once

#include "App.xaml.g.h"
#include <winrt/Windows.System.h>

namespace winrt::mpv_winui3_demo::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        winrt::Windows::System::DispatcherQueueController dispatcherQueueController{ nullptr };
    };
} // namespace winrt::mpv_winui3_demo::implementation
