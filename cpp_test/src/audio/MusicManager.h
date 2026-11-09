#pragma once


#include "audio_enums.h"


class tStreamedSample
{

public:
	uint32 m_nLength;
	uint32 m_nPosition;
	uint32 m_nLastPosCheckTimer;
};


class CVehicle;

class CPed;


class cMusicManager
{

public:
	bool8 m_bIsInitialised;
	bool8 m_bDisabled;
	bool8 m_bSetNextStation;
	uint8 m_nVolumeLatency;
	uint8 m_nCurrentVolume;
	uint8 m_nMaxVolume;
	uint32 m_nAnnouncement;
	bool8 m_bAnnouncementInProgress;
	tStreamedSample m_aTracks[TOTAL_STREAMED_SOUNDS];
	bool8 m_bResetTimers;
	uint32 m_nResetTime;
	bool8 m_bRadioSetByScript;
	uint8 m_nRadioStationScript;
	int32 m_nRadioPosition;
	uint32 m_nRadioInCar;
	uint32 m_nFrontendTrack;
	uint32 m_nPlayingTrack;
	uint8 m_nUpcomingMusicMode;
	uint8 m_nMusicMode;
	bool8 m_FrontendLoopFlag;
	bool8 m_bTrackChangeStarted;
	uint32 m_nNextTrack;
	bool8 m_nNextLoopFlag;
	bool8 m_bVerifyNextTrackStartedToPlay;
	bool8 m_bGameplayAllowsRadio;
	bool8 m_bRadioStreamReady;
	int8 nFramesSinceCutsceneEnded;
	bool8 m_bUserResumedGame;
	bool8 m_bMusicModeChangeStarted;
	uint8 m_nMusicModeToBeSet;
	bool8 m_bEarlyFrontendTrack;
	float aListenTimeArray[NUM_RADIOS];
	float m_nLastTrackServiceTime;

public:
	cMusicManager();
	bool8 IsInitialised() { return m_bIsInitialised; }
	uint8 GetMusicMode() { return m_nMusicMode; }
	uint32 GetCurrentTrack() { return m_nPlayingTrack; }

	void ResetMusicAfterReload();
	void SetStartingTrackPositions(bool8 isNewGameTimer);
	bool8 Initialise();
	void Terminate();

	void ChangeMusicMode(uint8 mode);
	void StopFrontEndTrack();

	bool8 PlayerInCar();
	void DisplayRadioStationName();

	void PlayAnnouncement(uint32);
	void PlayFrontEndTrack(uint32, bool8);
	void PreloadCutSceneMusic(uint32);
	void PlayPreloadedCutSceneMusic(void);
	void StopCutSceneMusic(void);
	uint32 GetRadioInCar(void);
	void SetRadioInCar(uint32);
	void SetRadioChannelByScript(uint32, int32);

	void ResetTimers(uint32);
	void Service();
	void ServiceFrontEndMode();
	void ServiceGameMode();
	void ServiceAmbience();
	void ServiceTrack(CVehicle *veh, CPed *ped);

	bool8 UsesPoliceRadio(CVehicle *veh);
	bool8 UsesTaxiRadio(CVehicle *veh);
	uint32 GetTrackStartPos(uint32 track);

	void ComputeAmbienceVol(bool8 reset, uint8& outVolume);
	bool8 ServiceAnnouncement();

	uint32 GetCarTuning();
	uint32 GetNextCarTuning();
	bool8 ChangeRadioChannel();
	void RecordRadioStats();
	void SetUpCorrectAmbienceTrack();
	float *GetListenTimeArray();
	uint32 GetRadioPosition(uint32 station);
	uint32 GetFavouriteRadioStation();
	void SetMalibuClubTrackPos(uint8 pos);
	void SetStripClubTrackPos(uint8 pos);
	bool8 CheckForMusicInterruptions();

	void Enable();
	void Disable();
	uint8 m_nNextTrack;
	uint8 m_nPlayingTrack;
	bool8 m_bFrontendTrackFinished;
	bool8 m_bPlayInFrontend;
	uint8 m_nAnnouncement;
	bool8 m_bPreviousPlayerInCar;
	bool8 m_bPlayerInCar;
	uint32 m_nLastTrackServiceTime;
	uint32 m_nTimer;
	bool8 m_bDoTrackService;
	bool8 m_bIgnoreTimeDelay;
	bool8 m_bVerifyAmbienceTrackStartedToPlay;
	uint8 m_nRadioInCar;
	uint32 GetMusicMode() { return m_nMusicMode; }
	uint8 GetNextTrack() { return m_nNextTrack; }
	void PlayAnnouncement(uint8);
	void PlayFrontEndTrack(uint8, bool8);
	void PreloadCutSceneMusic(uint8);
	uint8 GetRadioInCar(void);
	void SetRadioChannelByScript(uint8, int32);
	void ServiceTrack();
	uint32 GetTrackStartPos(uint8);
	uint8 GetCarTuning();
	uint8 GetNextCarTuning();
};


VALIDATE_SIZE(cMusicManager, 0x95C);


extern cMusicManager MusicManager;

extern bool8 g_bAnnouncementReadPosAlready; // we have a symbol of this so it was declared in .h

float GetHeightScale();
