#include "pcm_player.h"

PCMPlayer::PCMPlayer() {
	hWaveOut = nullptr;
	wvHeader[0].dwFlags = 0;
	wvHeader[1].dwFlags = 0;
	wvHeader[0].lpData = (CHAR*)malloc(MAX_BUFFER_SIZE);
	wvHeader[1].lpData = (CHAR*)malloc(MAX_BUFFER_SIZE);
	wvHeader[0].dwBufferLength = MAX_BUFFER_SIZE;
	wvHeader[1].dwBufferLength = MAX_BUFFER_SIZE;

	bufCaching = (CHAR*)malloc(MAX_BUFFER_SIZE);
	hEventPlay = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

PCMPlayer::~PCMPlayer() {
	free(wvHeader[0].lpData);
	free(wvHeader[1].lpData);
	free(bufCaching);
	CloseHandle(hEventPlay);
}

int PCMPlayer::Open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels) {
	WAVEFORMATEX wfx;
	if (!bufCaching || !hEventPlay || !wvHeader[0].lpData || !wvHeader[1].lpData) {
		return -1;
	}
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = nChannels;
	wfx.nSamplesPerSec = nSamplesPerSec;
	wfx.wBitsPerSample = wBitsPerSample;
	wfx.cbSize = 0;
	wfx.nBlockAlign = (WORD)(wfx.wBitsPerSample * wfx.nChannels / 8);
	wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wfx.wBitsPerSample / 8;
	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD_PTR)hEventPlay, 0, CALLBACK_EVENT)) {
		return -1;
	}
	waveOutPrepareHeader(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
	waveOutPrepareHeader(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));
	if (!(wvHeader[0].dwFlags & WHDR_PREPARED) || !(wvHeader[1].dwFlags & WHDR_PREPARED)) {
		return -1;
	}
	bufUsed = 0;
	iCurPlaying = 0;
	hasBegan = false;
	return 0;
}

void PCMPlayer::Close() {
	waveOutUnprepareHeader(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));
	waveOutClose(hWaveOut);
	hWaveOut = nullptr;
}

int PCMPlayer::Push(const char* buf, size_t size) {
again:
	if (bufUsed + size < MAX_BUFFER_SIZE) {
		memcpy(bufCaching + bufUsed, buf, size);
		bufUsed += size;
	}
	else {
		memcpy(bufCaching + bufUsed, buf, MAX_BUFFER_SIZE - bufUsed);
		if (!hasBegan) {
			if (0 == iCurPlaying) {
				memcpy(wvHeader[0].lpData, bufCaching, MAX_BUFFER_SIZE);
				iCurPlaying = 1;
			}
			else {
				ResetEvent(hEventPlay);
				memcpy(wvHeader[1].lpData, bufCaching, MAX_BUFFER_SIZE);
				waveOutWrite(hWaveOut, &wvHeader[0], sizeof(WAVEHDR));
				waveOutWrite(hWaveOut, &wvHeader[1], sizeof(WAVEHDR));
				hasBegan = true;
				iCurPlaying = 0;
			}
		}
		else if (Play(bufCaching, MAX_BUFFER_SIZE) < 0) {
			return -1;
		}
		size -= MAX_BUFFER_SIZE - bufUsed;
		buf += MAX_BUFFER_SIZE - bufUsed;
		bufUsed = 0;
		if (size > 0) goto again;
	}
	return 0;
}

int PCMPlayer::Flush() {
	if (bufUsed > 0 && Play(bufCaching, bufUsed) < 0) {
		return -1;
	}
	return 0;
}

int PCMPlayer::Play(const char* buf, int size) {
	WaitForSingleObject(hEventPlay, INFINITE);
	wvHeader[iCurPlaying].dwBufferLength = size;
	memcpy(wvHeader[iCurPlaying].lpData, buf, size);
	if (waveOutWrite(hWaveOut, &wvHeader[iCurPlaying], sizeof(WAVEHDR))) {
		SetEvent(hEventPlay);
		return -1;
	}
	iCurPlaying = !iCurPlaying;
	return 0;
}