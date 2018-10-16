#pragma once
#include <windows.h>
#pragma comment(lib, "winmm.lib")

#define MAX_BUFFER_SIZE (1024 * 8 * 16)

/**
* Simple audio player for playing pcm data
* @class
*/
class PCMPlayer {
private:

	/**
	* Use event to control processing data
	*/
	HANDLE hEventPlay;

	/**
	* Wave out handler
	*/
	HWAVEOUT hWaveOut;

	/**
	* Double caching
	*/
	WAVEHDR wvHeader[2];

	/**
	* Buffer caching pcm data
	*/
	char* bufCaching;

	/**
	* Buffer offset
	*/
	int bufUsed;

	/**
	* wave header index
	*/
	int iCurPlaying;

	/**
	* Has played
	*/
	bool hasBegan;
public:
	PCMPlayer();
	~PCMPlayer();

	/**
	* Open | initialize
	* @param nSamplesPerSec
	* @param wBitsPerSample
	* @param nChannels
	* @return
	*/
	int Open(DWORD nSamplesPerSec, WORD wBitsPerSample, WORD nChannels);

	/**
	* Close
	*/
	void Close();
	int Push(const char* buf, size_t size);
	int Flush();
private:
	int Play(const char* buf, int size);
};