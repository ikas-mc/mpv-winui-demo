#include "pch.h"
#include "MpvPlayer.h"
#include <stdexcept>
#ifdef UWP
    #include <windows.ui.xaml.media.dxinterop.h>
#else
    #include <microsoft.ui.xaml.media.dxinterop.h>
#endif

#ifdef UWP
using namespace winrt::Windows::UI::Xaml::Controls;
#else
using namespace winrt::Microsoft::UI::Xaml::Controls;
#endif
namespace mpv
{
    MpvPlayer::MpvPlayer() : m_mpv(nullptr), m_swapChain(nullptr)
    {
    }
    MpvPlayer::~MpvPlayer()
    {
        if (m_mpv)
        {
            mpv_terminate_destroy(m_mpv);
        }
    }

    bool MpvPlayer::Initialize(uint32_t width, uint32_t height)
    {
        m_mpv = mpv_create();
        if (!m_mpv)
        {
            OutputDebugStringA("mpv_create failed with");
            return false;
        }

        mpv_set_option_string(m_mpv, "hwdec", "auto");
        mpv_set_option_string(m_mpv, "vo", "gpu-next");
        mpv_set_option_string(m_mpv, "gpu-api", "d3d11");
        mpv_set_option_string(m_mpv, "d3d11-output-mode", "composition");
        mpv_set_option_string(m_mpv, "force-window", "immediate");

        // width and height must be > 0
        UpdateSize(width, height);

        int result = mpv_initialize(m_mpv);
        if (result < 0)
        {
            auto msg = mpv_error_string(result);
            OutputDebugStringA(std::format("mpv_initialize failed with error : {}", msg).c_str());
            return false;
        }

        mpv_get_property(m_mpv, "display-swapchain", MPV_FORMAT_INT64, &m_swapChain);
        if (!m_swapChain)
        {
            OutputDebugStringA("Failed to get swapchain");
            return false;
        }

        return true;
    }

    void MpvPlayer::SetSwapChainPanel(SwapChainPanel panel)
    {
        if (!m_swapChain)
        {
            OutputDebugStringA("swapchain is null, check Initialize");
            return;
        }

        winrt::com_ptr<ISwapChainPanelNative> panelNative;
        if (panel.try_as(panelNative))
        {
            panelNative->SetSwapChain(m_swapChain);
        }

        UpdateSwapChainScale(panel.CompositionScaleX(), panel.CompositionScaleY());
    }

    void MpvPlayer::UpdateSwapChainScale(float scaleX, float scaleY)
    {
        if (!m_swapChain)
        {
            return;
        }

        DXGI_MATRIX_3X2_F inverseScale{ 0 };
        inverseScale._11 = 1.0f / scaleX;
        inverseScale._22 = 1.0f / scaleY;
        m_swapChain->SetMatrixTransform(&inverseScale);
    }

    void MpvPlayer::UpdateSize(uint32_t width, uint32_t height)
    {
        if (!m_mpv)
        {
            return;
        }

        std::string size = std::format("{}x{}", width, height);

        // TODO
        mpv_set_option_string(m_mpv, "d3d11-composition-size", size.c_str());
    }

    void MpvPlayer::Play(const std::wstring_view& url)
    {
        if (!m_mpv)
        {
            return;
        }

        std::string path = winrt::to_string(url);
        const char* cmd[] = { "loadfile", path.c_str(), NULL };

        mpv_command(m_mpv, cmd);
    }

    void MpvPlayer::ApplyColorOptionsSDR()
    {
        if (!m_mpv)
        {
            return;
        }

        // set color info, here is an example for SDR
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "no");
        mpv_set_option_string(m_mpv, "target-prim", "bt.709");
        mpv_set_option_string(m_mpv, "target-trc", "srgb");
    }

    void MpvPlayer::ApplyColorOptionsWCG()
    {
        if (!m_mpv)
        {
            return;
        }

        // set color, here is an example for WCG
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "yes");
        mpv_set_option_string(m_mpv, "target-colorspace-hint-mode", "source");
    }

    void MpvPlayer::ApplyColorOptionsHDR()
    {
        if (!m_mpv)
        {
            return;
        }

        // set color, here is an example for HDR
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "yes");
        mpv_set_option_string(m_mpv, "target-colorspace-hint-mode", "source");
    }

} // namespace mpv
