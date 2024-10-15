#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>
#include <xaudio2.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "user32.lib")

#define MINIMP3_ONLY_MP3
#define MINIMP3_ONLY_SIMD
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

#define PRINT_DEBUG_INFO            1

#define	WAVE_FORMAT_PCM				1
#define sndMono						0x00
#define sndStereo					0x01
#define sndRate5K					5512
#define sndRate11K					11025
#define sndRate22K					22050
#define sndRate44K					44100
#define snd8Bit						1
#define snd16Bit					2

#define BUUFERS_PADDING				(1<<13)
#define BUFFERS_SIZE_IN_SECONDS		16		// in second
#define PCMSIZE                     ((((sndRate44K * BUFFERS_SIZE_IN_SECONDS) / 1152 + 1) * 1152) << 2)

typedef struct PcmHeader
{
    uint8_t* data;
    uint32_t byte;
    uint8_t  idx;
} PcmHeader;

uint8_t* pcmBuf0[PCMSIZE + BUUFERS_PADDING];
uint8_t* pcmBuf1[PCMSIZE + BUUFERS_PADDING];

PcmHeader pcmHeader[2];

uint8_t* mp3Buf = nullptr;
uint32_t mp3Size = 0;
uint32_t mp3Pos = 0;
uint8_t  nBlockAlign = 4;

WAVEFORMATEX pcmFormat = { 0 };

IXAudio2* xaudio = nullptr;
IXAudio2MasteringVoice* xaudioMasterVoice = nullptr;
IXAudio2SourceVoice* xaudioSourceVoice = nullptr;

HANDLE hThread = nullptr;
HANDLE hEvent[2] = { 0 };

int FillSingleBuffer(PcmHeader* ph)
{
    int samples, mp3_bytes;
    uint32_t bytePCM;
    uint8_t *pcm, *mp3;
    mp3dec_frame_info_t info;
    mp3dec_t  mp3d;
        
    if(mp3Pos >= mp3Size) // no more data to decode
    {
#ifdef PRINT_DEBUG_INFO
        printf("NO data now: mp3Pos=%d, mp3Size=%d\n", mp3Pos, mp3Size);
#endif
        return 0;
    }
        
    pcm = ph->data;
    bytePCM = 0;
    mp3 = mp3Buf + mp3Pos;
    mp3_bytes = mp3Size - mp3Pos;

    mp3dec_init(&mp3d);
    while(true)
    {
        samples = mp3dec_decode_frame(&mp3d, mp3, mp3_bytes, (mp3d_sample_t*)(pcm + bytePCM), &info);
        mp3 += info.frame_bytes;
        mp3_bytes -= info.frame_bytes;
        mp3Pos += info.frame_bytes;
        bytePCM += (samples * nBlockAlign);
        if (bytePCM >= PCMSIZE || mp3_bytes <= 0)
            break;
    }
#ifdef PRINT_DEBUG_INFO
    DWORD tid = GetCurrentThreadId();
    printf("MP3 Thread[%d]==== mp3Size: %d, mp3Pos: %d mp3_bytes: %d | Fill Buf[%d]: %d -> %d bytes\n", 
            tid, mp3Size, mp3Pos, mp3_bytes, ph->idx, ph->byte, bytePCM);
#endif
    ph->byte = bytePCM;
    
#ifdef PRINT_DEBUG_INFO
    for(int i=0; i<2; i++)
        printf("---->Buf[%d] has %d bytes\n", i, pcmHeader[i].byte); 
#endif
    return 0;
}

int PlayBuffer(PcmHeader* ph)
{
	XAUDIO2_BUFFER xaudioBuffer = { 0 };

    if(0 != ph->byte)
    {
        xaudioBuffer.pContext = ph;
        xaudioBuffer.AudioBytes = ph->byte;
        xaudioBuffer.pAudioData = ph->data;
        if (ph->byte < PCMSIZE)		// it is the last buffer to play
            xaudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
        // here is the real action to play the sound in XAudio2 device
        xaudioSourceVoice->SubmitSourceBuffer(&xaudioBuffer, nullptr);
    }
    return 0;
}

DWORD WINAPI MP3PlayThread(LPVOID w)
{
	DWORD dwRet, i;
        
    mp3Pos = 0;
    // fill the first buffer at first
    FillSingleBuffer(&(pcmHeader[0]));
    PlayBuffer(&(pcmHeader[0]));
    FillSingleBuffer(&(pcmHeader[1]));

	while (true)
	{
		dwRet = MsgWaitForMultipleObjects(2, hEvent, FALSE, INFINITE, QS_ALLINPUT);
		switch (dwRet)
		{
		case WAIT_OBJECT_0 :
            for(i=0; i<2; i++) {
                if(0 == pcmHeader[i].byte) 
                    FillSingleBuffer(&(pcmHeader[i]));
            }
			break;
		case WAIT_OBJECT_0 + 1:
			return 0;
		default:
			break;
		}
	}
    return 0;
}

class VoiceCallback : public IXAudio2VoiceCallback
{
public:
    VoiceCallback() {}
    ~VoiceCallback() 
    {
        if(nullptr != m_hBufferEndEvent)
        {
            CloseHandle(m_hBufferEndEvent);
            m_hBufferEndEvent = nullptr;
        }
    }

    bool Init() 
    { 
        m_hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        return (m_hBufferEndEvent != nullptr)? true : false;
    }
    
    void WaitPlayEnd()
    {
        DWORD dwWaitResult = WaitForSingleObject(m_hBufferEndEvent, INFINITE);
    }
    
    void OnStreamEnd() 
    {
#ifdef PRINT_DEBUG_INFO        
            printf("All Buffers are empty!!!!!!\n");
#endif
        SetEvent(m_hBufferEndEvent);
    }
    void OnVoiceProcessingPassEnd() {}
    void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}

    void OnBufferEnd(void* pBufferContext) 
    {
        uint8_t idx;
        uint32_t byte;
        PcmHeader* ph = (PcmHeader*)pBufferContext;

        byte = ph->byte;
        ph->byte = 0; // set this buffer to empty
        idx = (ph->idx + 1) & 1;

#ifdef PRINT_DEBUG_INFO
        DWORD tid = GetCurrentThreadId();
        printf("XAudio Thread[%d] --> Buffer[%d](%d bytes) is empty, now play Buffer[%d](%d bytes)\n", 
                    tid, ph->idx, byte, idx, pcmHeader[idx].byte );
#endif   
        PlayBuffer(&(pcmHeader[idx]));
        SetEvent(hEvent[0]); // tell the sound playing thread to fill the next buffer
    }
    void OnBufferStart(void* pBufferContext) {}
    void OnLoopEnd(void* pBufferContext) {}
    void OnVoiceError(void* pBufferContext, HRESULT Error) {}
private:
    HANDLE m_hBufferEndEvent = nullptr;
};

VoiceCallback voiceCallback;

bool XAudio2Init(WAVEFORMATEX* pwaveFormat)
{
    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if(S_OK != hr)
        return false;
    hr = XAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if(S_OK != hr)
    {
        CoUninitialize();
        return false;
    }
    hr = xaudio->CreateMasteringVoice(&xaudioMasterVoice,
            XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, NULL, NULL, AudioCategory_GameEffects);
    if(S_OK != hr)
    {
		if (nullptr != xaudio)
		{
			xaudio->StopEngine();
			xaudio->Release();
			xaudio = nullptr;
		}
        CoUninitialize();
        return false;
    }
    hr = xaudio->CreateSourceVoice(&xaudioSourceVoice, pwaveFormat, XAUDIO2_VOICE_NOPITCH, XAUDIO2_DEFAULT_FREQ_RATIO,
                                reinterpret_cast<IXAudio2VoiceCallback*>(&voiceCallback), NULL, NULL);
    if(S_OK != hr)
    {
        if (nullptr != xaudioMasterVoice)
        {
            xaudioMasterVoice = nullptr;
        }
		if (nullptr != xaudio)
		{
			xaudio->StopEngine();
			xaudio->Release();
			xaudio = nullptr;
		}
        CoUninitialize();
        return false;
    }
    hr = xaudioSourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
    if(S_OK != hr)
    {
		if (nullptr != xaudioSourceVoice)
		{
			xaudioSourceVoice->Discontinuity();
			xaudioSourceVoice = nullptr;
		}
        if (nullptr != xaudioMasterVoice)
        {
            xaudioMasterVoice = nullptr;
        }
		if (nullptr != xaudio)
		{
			xaudio->StopEngine();
			xaudio->Release();
			xaudio = nullptr;
		}
        CoUninitialize();
        return false;
    }
    return true;
}

void XAudio2Release()
{
	if (nullptr != xaudioSourceVoice)
	{
		xaudioSourceVoice->Stop();
		xaudioSourceVoice->FlushSourceBuffers();
		xaudioSourceVoice->Discontinuity();
		xaudioSourceVoice = nullptr;
	}
	if (nullptr != xaudioMasterVoice)
	{
		xaudioMasterVoice = nullptr;
	}
	if (nullptr != xaudio)
	{
		xaudio->StopEngine();
		xaudio->Release();
		xaudio = nullptr;
	}
	CoUninitialize();
}

int main(int argc, const char* argv[])
{
int samples;
size_t ret;
FILE* fmp3 = nullptr;
mp3dec_t mp3d;
mp3dec_frame_info_t info;
    
    if(argc < 2)
    {
        printf("Usuage: %s mp3_file\n", argv[0]);
        return 0;
    }
  
    fmp3 = fopen(argv[1], "rb");
    if(nullptr == fmp3) 
    {
		printf("cannot open mp3 file [%s]!\n", argv[1]);
        return 0;
    }
    fseek(fmp3, 0L, SEEK_END);
    mp3Size = ftell(fmp3);
    fseek(fmp3, 0L, SEEK_SET);

    mp3Buf = (uint8_t*)malloc(mp3Size);
    if(nullptr == mp3Buf) 
    {
        printf("Cannot allocate %d bytes memory!\n", mp3Size);
        fclose(fmp3);
        return 0;
    }
    ret = fread(mp3Buf, mp3Size, 1, fmp3);
	if(1 != ret) 
    {
        printf("Cannot read %d bytes from mp3 file[%s]!\n", mp3Size, argv[1]);
        fclose(fmp3);
        goto _exit_app;
    }
    fclose(fmp3);

    // try to detect the PCM format from the first MP3 frame
    mp3dec_init(&mp3d);
    samples = mp3dec_decode_frame(&mp3d, mp3Buf, mp3Size, (mp3d_sample_t*)pcmBuf0, &info);
#ifdef PRINT_DEBUG_INFO
    DWORD tid = GetCurrentThreadId();
    printf("Main Thread[%d] -- samples=%d, frame_bytes=%d, frame_offset=%d, channels=%d, hz=%d, layer=%d, bitrate_kbps=%d\n",
        tid, samples, info.frame_bytes, info.frame_offset, info.channels, info.hz, info.layer, info.bitrate_kbps);
#endif
    pcmFormat.wBitsPerSample  = snd16Bit << 3;  // maybe detect from some place
    pcmFormat.nChannels       = info.channels;
    pcmFormat.nSamplesPerSec  = info.hz;
    pcmFormat.nBlockAlign     = info.channels * snd16Bit;
    pcmFormat.nAvgBytesPerSec = info.hz * info.channels * snd16Bit;
    pcmFormat.wFormatTag      = WAVE_FORMAT_PCM;
    pcmFormat.cbSize          = 0;
    nBlockAlign = info.channels * snd16Bit;
    
    if(false == voiceCallback.Init())
    {
        printf("Cannot Initalize voiceCallback Interface.\n");
        goto _exit_app;
    }
    if(false == XAudio2Init(&pcmFormat))
    {
        printf("Cannot Initalize XAudio2 Interface.\n");
        goto _exit_app;
    }

    pcmHeader[0].data = (uint8_t*)pcmBuf0;
    pcmHeader[0].byte = 0;
    pcmHeader[0].idx = 0;
    pcmHeader[1].data = (uint8_t*)pcmBuf1;
    pcmHeader[1].byte = 0;
    pcmHeader[1].idx = 1;
    hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(nullptr == hEvent[0] || nullptr == hEvent[1])
    {
        printf("Cannot creat hEvents!\n");
        goto _exit_app;
    }
    hThread = CreateThread(0, 0, MP3PlayThread, nullptr, 0, NULL);
    if(nullptr == hThread)
    {
        printf("Cannot Create MP3 Playing Thread.\n");
        goto _exit_app;
    }
    voiceCallback.WaitPlayEnd();

    if(nullptr != hEvent[1])
        SetEvent(hEvent[1]); // tell the sound play thread to quit gracefully
   
    XAudio2Release();
    
_exit_app:

	if(nullptr != mp3Buf) 
        free(mp3Buf);
    return 0;
}
