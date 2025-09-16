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

    void MpvPlayer::Initialize(uint32_t width, uint32_t height)
    {
        m_mpv = mpv_create();
        if (!m_mpv)
        {
            throw std::runtime_error("Failed to create mpv context");
        }

        mpv_set_option_string(m_mpv, "hwdec", "auto");
        mpv_set_option_string(m_mpv, "vo", "gpu-next");
        mpv_set_option_string(m_mpv, "gpu-api", "d3d11");
        mpv_set_option_string(m_mpv, "d3d11-output-mode", "composition");
        mpv_set_option_string(m_mpv, "force-window", "immediate");

        UpdateSize(width, height);

        if (mpv_initialize(m_mpv) < 0)
        {
            throw std::runtime_error("Failed to initialize mpv");
        }

        mpv_get_property(m_mpv, "display-swapchain", MPV_FORMAT_INT64, &m_swapChain);
        if (!m_swapChain)
        {
            throw std::runtime_error("Failed to get swapchain");
        }
    }

    void MpvPlayer::SetSwapChainPanel(SwapChainPanel panel)
    {
        winrt::com_ptr<ISwapChainPanelNative> panelNative;
        if (panel.try_as(panelNative))
        {
            panelNative->SetSwapChain(m_swapChain);
        }

        DXGI_MATRIX_3X2_F inverseScale{ 0 };
        inverseScale._11 = 1.0f / panel.CompositionScaleX();
        inverseScale._22 = 1.0f / panel.CompositionScaleY();
        m_swapChain->SetMatrixTransform(&inverseScale);
    }

    void MpvPlayer::UpdateSize(uint32_t width, uint32_t height)
    {
        if (!m_mpv)
        {
            return;
        }
        std::string size = std::format("{}x{}", width, height);
        // mpv_set_option_string(m_mpv, "d3d11-composition-size", size.c_str());
    }

    void MpvPlayer::Play(const std::wstring_view& url)
    {
        std::string path = winrt::to_string(url);
        const char* cmd[] = { "loadfile", path.c_str(), NULL };

        mpv_command(m_mpv, cmd);
    }

    void MpvPlayer::ApplyColorOptionsSDR()
    {
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "no");
        mpv_set_option_string(m_mpv, "target-prim", "bt.709");
        mpv_set_option_string(m_mpv, "target-trc", "srgb");
    }

    void MpvPlayer::ApplyColorOptionsWCG()
    {
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "yes");
        mpv_set_option_string(m_mpv, "target-colorspace-hint-mode", "source");
    }

    void MpvPlayer::ApplyColorOptionsHDR()
    {
        mpv_set_option_string(m_mpv, "target-colorspace-hint", "yes");
        mpv_set_option_string(m_mpv, "target-colorspace-hint-mode", "source");
    }

} // namespace mpv
