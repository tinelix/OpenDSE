#ifdef WIN32_WASAPI

#include <os/win32/wasapi.h>
#include <mmio/mmio.h>
#include <dsepriv.h>
#include <stdlib.h>

int                 wasapiCurrentFrame;
uint_t              wasapiSampleSize;
uint_t              wasapiFrameSize        = 1024;
uint_t              wasapiFramesCount      =   32;
uint_t              wasapiBitDepth         =   16;
uint_t              wasapiChannels         =    2;

IAudioClient2*      wasapiClient;
IAudioRenderClient* wasapiRenderClient;
HANDLE              wasapiEvent;

dse_result _dse_wasapi_open(DSE_OUTDEV* outdev, DSE_MMIO* mmio) {
    IMMDeviceEnumerator*   mmEnum;
    IMMDevice*             mmDev;
	IPropertyStore*        mmProps;
	PROPVARIANT            mmVarProductName;
    char                   mmVarProductNameCp[144];
    int                    result           = 0;
    WAVEFORMATEX           wavFormat;
    const long             REFTIMES_PER_MS  = 10000L;  // 100 ns
	REFERENCE_TIME         reqBuffDuration;
    ulong_t                initStreamFlags  = (AUDCLNT_STREAMFLAGS_RATEADJUST | 
                                               AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                                               AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);

    //wasapiEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	outdev->product_name = (char*)malloc(161 * sizeof(char*));

	reqBuffDuration = REFTIMES_PER_MS * 4000;

	ZeroMemory(&wavFormat, sizeof(WAVEFORMATEX));
    
    outdev->sample_rate     = mmio->audio.sample_rate;
    outdev->bit_depth       = mmio->audio.bit_depth;
    outdev->channels        = mmio->audio.channels;

	wasapiSampleSize        = (mmio->audio.bit_depth / 8) * mmio->audio.channels;

	result = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    
    result = CoCreateInstance(
                __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), 
                (void**)&mmEnum
             );
    
    if(result != S_OK)
        return DSE_OUTDEV_NOT_INITIALIZED;
    
    result = mmEnum->GetDefaultAudioEndpoint(
        eRender, eConsole, &mmDev
    );
    
    result = mmDev->Activate(
        __uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (void**)(&wasapiClient)
    );
    
    if(result != S_OK)
        return DSE_OUTDEV_NOT_INITIALIZED;

    mmDev->OpenPropertyStore(STGM_READ, &mmProps);
        
    PropVariantInit(&mmVarProductName);
    mmProps->GetValue(PKEY_Device_FriendlyName, &mmVarProductName);

    // UTF-8 to Legacy CP conversion
    WideCharToMultiByte(
        CP_ACP, 0,
        mmVarProductName.pwszVal, wcslen(mmVarProductName.pwszVal),
        mmVarProductNameCp, 144, 
        nullptr, nullptr
    );

    mmDev->Release();
    
    wavFormat.wFormatTag      = WAVE_FORMAT_PCM;
    wavFormat.nSamplesPerSec  = mmio->audio.sample_rate;
    wavFormat.wBitsPerSample  = mmio->audio.bit_depth;
    wavFormat.nChannels       = mmio->audio.channels;
    wavFormat.nBlockAlign     = (mmio->audio.bit_depth * wavFormat.nChannels) / 8;
    wavFormat.nAvgBytesPerSec = mmio->audio.sample_rate * wavFormat.nBlockAlign;
    
    wasapiBitDepth            = mmio->audio.bit_depth;
    wasapiChannels            = mmio->audio.channels;

	outdev->max_channels      = mmio->audio.channels > 2 ? 2 : mmio->audio.channels;

    result = wasapiClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED, initStreamFlags, 
        reqBuffDuration, 
		0,  &wavFormat, nullptr
    );

    //wasapiClient->SetEventHandle(wasapiEvent);
    
	#ifdef MSVC_GE_800
        sprintf_s(outdev->product_name, 100, "[WASAPI] %s", mmVarProductNameCp);
	#else
		sprintf(outdev->product_name, "[WASAPI] %s", mmVarProductNameCp);
	#endif

    if(result != S_OK)
        return DSE_OUTDEV_NOT_INITIALIZED;
        
    return DSE_OK;
}

dse_result _dse_wasapi_allocate() {
    int result = 0;

	result = wasapiClient->GetService(__uuidof(IAudioRenderClient), (void**)(&wasapiRenderClient));
	if(result != S_OK)
        return DSE_OUTDEV_NOT_INITIALIZED;

    result = wasapiClient->GetBufferSize(&wasapiFramesCount);
    
    if(result != S_OK)
        return DSE_ALLOCATION_ERROR;

    result = wasapiClient->Start();
    
    if(result != S_OK)
        return DSE_INTERNAL_ERROR;
    
    return DSE_OK;
}

void _dse_wasapi_write(LPSTR data, int size) {
	int     	result = 0;
	uchar_t*	buffer;

	Sleep(50);

	result = wasapiRenderClient->GetBuffer((double)size / wasapiSampleSize, (BYTE**)(&buffer));

	if (result != S_OK)
		return;

	memcpy(buffer, data, size);

	wasapiRenderClient->ReleaseBuffer((double)size / wasapiSampleSize, 0);

	if (result != S_OK)
		return;
}

void _dse_wasapi_free() {
    wasapiClient->Stop();
    return;
}

dse_result _dse_wasapi_close() {
    wasapiClient->Release();
    wasapiRenderClient->Release();
    return DSE_OK;
}

#endif