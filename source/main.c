#include <3ds.h>
#include <stdio.h>
#include <stdint.h>

static const char *getModelName(u8 model)
{
    switch (model)
    {
        case CFG_MODEL_3DS:   return "Old Nintendo 3DS";
        case CFG_MODEL_3DSXL: return "Old Nintendo 3DS XL";
        case CFG_MODEL_N3DS:  return "New Nintendo 3DS";
        case CFG_MODEL_2DS:   return "Nintendo 2DS";
        case CFG_MODEL_N3DSXL:return "New Nintendo 3DS XL";
        case CFG_MODEL_N2DSXL:return "New Nintendo 2DS XL";
        default:              return "Unknown model";
    }
}

static void formatSize(u64 bytes, char *out, size_t outSize)
{
    const double mib = (double)bytes / (1024.0 * 1024.0);
    const double gib = mib / 1024.0;

    if (gib >= 1.0)
        snprintf(out, outSize, "%.2f GB", gib);
    else
        snprintf(out, outSize, "%.1f MB", mib);
}

static void drawSystemInfo(void)
{
    u8 model = 0xFF;
    char firmware[64] = "Unavailable";
    OS_VersionBin nver = {0}, cver = {0};

    Result modelResult = CFGU_GetSystemModel(&model);
    Result firmwareResult =
        osGetSystemVersionDataString(&nver, &cver, firmware, sizeof(firmware));

    FS_ArchiveResource sd = {0};
    Result sdResult = FSUSER_GetSdmcArchiveResource(&sd);

    const u8 batteryState = OS_SharedConfig->led_battery;
    const u8 batteryLevel = (batteryState >> 2) & 0x07;
    const bool adapterConnected = (batteryState & 0x01) != 0;
    const bool charging = (batteryState & 0x02) != 0;

    consoleClear();

    printf("\x1b[36;1m");
    printf("          3DS SYSTEM INFO\n");
    printf("\x1b[37m");
    printf("----------------------------------------\n\n");

    printf("\x1b[33;1mSYSTEM\n\x1b[37m");
    if (R_SUCCEEDED(modelResult))
        printf("Model:       %s\n", getModelName(model));
    else
        printf("Model:       Could not read\n");

    if (R_SUCCEEDED(firmwareResult))
        printf("Firmware:    %s\n", firmware);
    else
        printf("Firmware:    Could not read\n");

    printf("FIRM version: %lu.%lu.%lu\n\n",\n           (unsigned long)GET_VERSION_MAJOR(osGetFirmVersion()),\n           (unsigned long)GET_VERSION_MINOR(osGetFirmVersion()),\n           (unsigned long)GET_VERSION_REVISION(osGetFirmVersion()));
    printf("\x1b[33;1mSD CARD\n\x1b[37m");

    if (R_SUCCEEDED(sdResult) && sd.sectorSize > 0 && sd.clusterSize > 0)
    {
        const u64 totalBytes = (u64)sd.clusterSize * sd.totalClusters;
        const u64 freeBytes = (u64)sd.clusterSize * sd.freeClusters;
        const u64 usedBytes = totalBytes - freeBytes;
        char totalText[24], freeText[24], usedText[24];

        formatSize(totalBytes, totalText, sizeof(totalText));
        formatSize(freeBytes, freeText, sizeof(freeText));
        formatSize(usedBytes, usedText, sizeof(usedText));

        printf("Total:       %s\n", totalText);
        printf("Used:        %s\n", usedText);
        printf("Free:        %s\n\n", freeText);
    }
    else
    {
        printf("SD card:     Not detected / unavailable\n\n");
    }

    printf("\x1b[33;1mPOWER\n\x1b[37m");
    printf("Battery bars: %u / 5 (system indicator)\n", batteryLevel);
    if (charging)
        printf("Power:        Charging\n");
    else if (adapterConnected)
        printf("Power:        Adapter connected\n");
    else
        printf("Power:        Running on battery\n");

    printf("\n\x1b[33;1mOTHER\n\x1b[37m");
    printf("Wi-Fi signal: %u / 3\n", osGetWifiStrength());
    printf("3D slider:    %u%%\n",
           (unsigned)(osGet3DSliderState() * 100.0f + 0.5f));

    printf("\n----------------------------------------\n");
    printf("A: Refresh information\n");
    printf("B: Exit to Homebrew Launcher\n");
}

int main(void)
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    const bool cfgReady = R_SUCCEEDED(cfguInit());
    const bool fsReady = R_SUCCEEDED(fsInit());

    drawSystemInfo();

    while (aptMainLoop())
    {
        hidScanInput();
        const u32 keys = hidKeysDown();

        if (keys & KEY_A)
            drawSystemInfo();

        if (keys & KEY_B)
            break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    if (fsReady) fsExit();
    if (cfgReady) cfguExit();
    gfxExit();
    return 0;
}
