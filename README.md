# MPV WinUI Demo

demo for mpv d3d11 composition mode

Most of the code is AI-generated and is for demo/testing purposes only.

When using composition mode, you need to set the color space information yourself.

<img src="https://raw.githubusercontent.com/ikas-mc/mpv-winui-demo/main/screenshots/winui.png" width=563 >

In UWP, use DisplayInformation GetAdvancedColorInfo to get color information.

```c++
void MainPage::ApplyColorSettings()
    {
        auto displayInformation = DisplayInformation::GetForCurrentView();
        auto colorInfo = displayInformation.GetAdvancedColorInfo();

        switch (colorInfo.CurrentAdvancedColorKind())
        {
            case winrt::Windows::Graphics::Display::AdvancedColorKind::HighDynamicRange:
                m_mpvPlayer->ApplyColorOptionsHDR();
                break;
            ...
        }
    }

```

In winui3(windows app sdk), use DisplayConfigGetDeviceInfo to get color information.
``` c++
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

```

