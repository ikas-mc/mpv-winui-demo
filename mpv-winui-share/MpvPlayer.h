#pragma once
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <mpv/client.h>
#include <string>

#ifdef UWP
using namespace winrt::Windows::UI::Xaml::Controls;
#else
using namespace winrt::Microsoft::UI::Xaml::Controls;
#endif

namespace mpv
{
    class MpvPlayer
    {
    public:
        MpvPlayer();
        ~MpvPlayer();

        bool Initialize(uint32_t width, uint32_t height);
        void SetSwapChainPanel(SwapChainPanel panel);
        void UpdateSize(uint32_t width, uint32_t height);
        void Play(const std::wstring_view& url);

        void ApplyColorOptionsSDR();
        void ApplyColorOptionsWCG();
        void ApplyColorOptionsHDR();

        mpv_handle* m_mpv;
        IDXGISwapChain3* m_swapChain;
    };

} // namespace mpv
