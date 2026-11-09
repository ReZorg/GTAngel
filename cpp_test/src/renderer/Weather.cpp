#include "common.h"


#include "Weather.h"


#include "Camera.h"

#include "Clock.h"

#include "CutsceneMgr.h"

#include "DMAudio.h"

#include "General.h"

#include "Pad.h"

#include "PlayerPed.h"

#include "Particle.h"

#include "RenderBuffer.h"

#include "Stats.h"

#include "Shadows.h"

#include "Timecycle.h"

#include "Timer.h"

#include "Vehicle.h"

#include "World.h"

#include "ZoneCull.h"

#include "SpecialFX.h"

#include "Replay.h"


int32 CWeather::SoundHandle = -1;


int32 CWeather::WeatherTypeInList;

int16 CWeather::OldWeatherType;

int16 CWeather::NewWeatherType;

int16 CWeather::ForcedWeatherType;


bool CWeather::LightningFlash;

bool CWeather::LightningBurst;

uint32 CWeather::LightningStart;

uint32 CWeather::LightningFlashLastChange;

uint32 CWeather::WhenToPlayLightningSound;

uint32 CWeather::LightningDuration;

int32 CWeather::StreamAfterRainTimer;


float CWeather::ExtraSunnyness;

float CWeather::Foggyness;

float CWeather::CloudCoverage;

float CWeather::Wind;

float CWeather::Rain;

float CWeather::InterpolationValue;

float CWeather::WetRoads;

float CWeather::Rainbow;

float CWeather::SunGlare;

float CWeather::WindClipped;

float CWeather::TrafficLightBrightness;


bool CWeather::bScriptsForceRain;

bool CWeather::Stored_StateStored;

float CWeather::Stored_InterpolationValue;

int16 CWeather::Stored_OldWeatherType;

int16 CWeather::Stored_NewWeatherType;

float CWeather::Stored_Rain;


tRainStreak Streaks[NUM_RAIN_STREAKS];


int16 WeatherTypesList[] = {
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_CLOUDY,
	WEATHER_RAINY, WEATHER_RAINY, WEATHER_RAINY, WEATHER_RAINY,
	WEATHER_CLOUDY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY
};


int16 WeatherTypesList_WithHurricanes[] = {
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_CLOUDY,
	WEATHER_HURRICANE, WEATHER_HURRICANE, WEATHER_CLOUDY, WEATHER_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_CLOUDY, WEATHER_HURRICANE, WEATHER_HURRICANE, WEATHER_HURRICANE,
	WEATHER_CLOUDY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY,
	WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY, WEATHER_EXTRA_SUNNY
};


const float Windyness[] = {
	0.25f,// WEATHER_SUNNY
	0.7f, // WEATHER_CLOUDY
	1.0f, // WEATHER_RAINY
	0.0f, // WEATHER_FOGGY
	0.0f, // WEATHER_EXTRA_SUNNY
	2.0f, // WEATHER_HURRICANE
	0.0f
};


#define MIN_TIME_BETWEEN_LIGHTNING_FLASH_CHANGES (50)


#define RAIN_CHANGE_SPEED (0.003f)


#define DROPLETS_LEFT_OFFSET (10.0f)

#define DROPLETS_RIGHT_OFFSET (10.0f)

#define DROPLETS_TOP_OFFSET (10.0f)

#define DROPLETS_BOTTOM_OFFSET (10.0f)


#define STREAK_U (10.0f)

#define STREAK_V (18.0f)

#define LARGE_STREAK_COEFFICIENT (1.23f)

#define STREAK_MIN_DISTANCE (8.0f)

#define STREAK_MAX_DISTANCE (16.0f)


#define SPLASH_CHECK_RADIUS (7.0f)

#define SPLASH_OFFSET_RADIUS (2.0f)


#define STREAK_LIFETIME (4.0f)

#define STREAK_INTEROLATION_TIME (0.3f)


#define RAIN_COLOUR_R (200)

#define RAIN_COLOUR_G (200)

#define RAIN_COLOUR_B (256)

#define RAIN_ALPHA (255)


void CWeather::Init(void)
{
	NewWeatherType = WEATHER_SUNNY;
	bScriptsForceRain = false;
	OldWeatherType = WEATHER_CLOUDY;
	Stored_StateStored = false;
	InterpolationValue = 0.0f;
	WhenToPlayLightningSound = 0;
	WeatherTypeInList = 0;
	ForcedWeatherType = WEATHER_RANDOM;
	SoundHandle = DMAudio.CreateEntity(AUDIOTYPE_WEATHER, (void*)1);
	if (SoundHandle >= 0)
		DMAudio.SetEntityStatus(SoundHandle, TRUE);
}


void CWeather::Update(void)
{
	if(!CReplay::IsPlayingBack()){
		float fNewInterpolation = (CClock::GetMinutes() + CClock::GetSeconds()/60.0f)/60.0f;
		if (fNewInterpolation < InterpolationValue) {
			// new hour
			OldWeatherType = NewWeatherType;
			if (ForcedWeatherType >= 0)
				NewWeatherType = ForcedWeatherType;
			else {
				WeatherTypeInList = (WeatherTypeInList + 1) % ARRAY_SIZE(WeatherTypesList);
				NewWeatherType = CStats::NoMoreHurricanes ? WeatherTypesList[WeatherTypeInList] : WeatherTypesList_WithHurricanes[WeatherTypeInList];
			}
		}
		InterpolationValue = fNewInterpolation;
	}

#ifndef FINAL
	if (CPad::GetPad(1)->GetRightShockJustDown()) {
		NewWeatherType = (NewWeatherType + 1) % WEATHER_TOTAL;
		OldWeatherType = NewWeatherType;
	}
#endif

	// Lightning
	if (NewWeatherType != WEATHER_RAINY || OldWeatherType != WEATHER_RAINY) {
		LightningFlash = false;
		LightningBurst = false;
	}
	else{
		if (LightningBurst) {
			if ((CGeneral::GetRandomNumber() & 255) >= 32) {
				// 0.875 probability
				if (CTimer::GetTimeInMilliseconds() - LightningFlashLastChange > MIN_TIME_BETWEEN_LIGHTNING_FLASH_CHANGES) {
					bool bOldLightningFlash = LightningFlash;
					LightningFlash = CGeneral::GetRandomTrueFalse();
					if (LightningFlash != bOldLightningFlash)
						LightningFlashLastChange = CTimer::GetTimeInMilliseconds();
				}
			}
			else {
				// 0.125 probability
				LightningBurst = false;
				LightningDuration = Min(CTimer::GetFrameCounter() - LightningStart, 20);
				LightningFlash = false;
				WhenToPlayLightningSound = CTimer::GetTimeInMilliseconds() + 150 * (20 - LightningDuration);
			}
		}
		else {
			if (CGeneral::GetRandomNumber() >= 200) {
				// lower probability on PC due to randomness bug
				LightningFlash = false;
			}
			else {
				LightningBurst = true;
				LightningStart = CTimer::GetFrameCounter();
				LightningFlashLastChange = CTimer::GetTimeInMilliseconds();
				LightningFlash = true;
			}
		}
	}
	if (WhenToPlayLightningSound && CTimer::GetTimeInMilliseconds() > WhenToPlayLightningSound) {
		DMAudio.PlayOneShot(SoundHandle, SOUND_LIGHTNING, LightningDuration);
		CPad::GetPad(0)->StartShake(40 * LightningDuration + 100, 2 * LightningDuration + 80);
		WhenToPlayLightningSound = 0;
	}

	// Wet roads
	if (OldWeatherType == WEATHER_RAINY || OldWeatherType == WEATHER_HURRICANE) {
		if (NewWeatherType == WEATHER_RAINY || NewWeatherType == WEATHER_HURRICANE)
			WetRoads = 1.0f;
		else
			WetRoads = 1.0f - InterpolationValue;
	}
	else {
		if (NewWeatherType == WEATHER_RAINY || NewWeatherType == WEATHER_HURRICANE)
			WetRoads = InterpolationValue;
		else
			WetRoads = 0.0f;
	}

	// Rain
	float fNewRain;
	if (NewWeatherType == WEATHER_RAINY || NewWeatherType == WEATHER_HURRICANE) {
		// if raining for >1 hour, values: 0, 0.33, switching every ~16.5s
		fNewRain = (((uint16)CTimer::GetTimeInMilliseconds() >> 14) & 1) * 0.33f;
		if (OldWeatherType != WEATHER_RAINY && OldWeatherType != WEATHER_HURRICANE) {
			if (InterpolationValue < 0.4f)
				// if rain has just started (<24 minutes), always 0.5
				fNewRain = 0.5f;
			else
				// if rain is ongoing for >24 minutes, values: 0.25, 0.5, switching every ~16.5s
				fNewRain = 0.25f + (((uint16)CTimer::GetTimeInMilliseconds() >> 14) & 1) * 0.25f;
		}
		fNewRain = Max(fNewRain, 0.5f);
	}
	else
		fNewRain = 0.0f;
	Rain = fNewRain;

	// Clouds
	if (OldWeatherType != WEATHER_SUNNY && OldWeatherType != WEATHER_EXTRA_SUNNY)
		CloudCoverage = 1.0f - InterpolationValue;
	else
		CloudCoverage = 0.0f;
	if (NewWeatherType != WEATHER_SUNNY && OldWeatherType != WEATHER_EXTRA_SUNNY)
		CloudCoverage += InterpolationValue;
	
	// Fog
	if (OldWeatherType == WEATHER_FOGGY)
		Foggyness = 1.0f - InterpolationValue;
	else
		Foggyness = 0.0f;
	if (NewWeatherType == WEATHER_FOGGY)
		Foggyness += InterpolationValue;

	// Extra Sunnyness
	if (OldWeatherType == WEATHER_EXTRA_SUNNY)
		ExtraSunnyness = 1.0f - InterpolationValue;
	else
		ExtraSunnyness = 0.0f;
	if (NewWeatherType == WEATHER_EXTRA_SUNNY)
		ExtraSunnyness += InterpolationValue;

	// Rainbow
	if (OldWeatherType == WEATHER_CLOUDY && (NewWeatherType == WEATHER_SUNNY || NewWeatherType == WEATHER_EXTRA_SUNNY) &&
		InterpolationValue < 0.5f && CClock::GetHours() > 6 && CClock::GetHours() < 21)
		Rainbow = 1.0f - 4.0f * Abs(InterpolationValue - 0.25f) / 4.0f;
	else
		Rainbow = 0.0f;

	// Sun Glare
	if (OldWeatherType == WEATHER_EXTRA_SUNNY)
		SunGlare = 1.0f - InterpolationValue;
	else
		SunGlare = 0.0f;
	if (NewWeatherType == WEATHER_EXTRA_SUNNY)
		SunGlare += InterpolationValue;

	if (SunGlare > 0.0f) {
		SunGlare *= Min(1.0f, 7.0 * CTimeCycle::GetSunDirection().z);
		SunGlare = Clamp(SunGlare, 0.0f, 1.0f);
		if (!CSpecialFX::bSnapShotActive)
			SunGlare *= (1.0f - (CGeneral::GetRandomNumber()&0x1F)*0.007f);
	}

	Wind = InterpolationValue * Windyness[NewWeatherType] + (1.0f - InterpolationValue) * Windyness[OldWeatherType];
	WindClipped = Min(1.0f, Wind);

	if (CClock::GetHours() > 20)
		TrafficLightBrightness = 1.0f;
	else if (CClock::GetHours() > 19)
		TrafficLightBrightness = CClock::GetMinutes() / 60.0f;
	else if (CClock::GetHours() > 6)
		TrafficLightBrightness = 0.0f;
	else if (CClock::GetHours() > 5)
		TrafficLightBrightness = 1.0f - CClock::GetMinutes() / 60.0f;
	else
		TrafficLightBrightness = 1.0f;
	TrafficLightBrightness = Max(WetRoads, TrafficLightBrightness);
	TrafficLightBrightness = Max(Foggyness, TrafficLightBrightness);
	TrafficLightBrightness = Max(Rain, TrafficLightBrightness);

	AddRain();

	if ((NewWeatherType == WEATHER_SUNNY || NewWeatherType == WEATHER_EXTRA_SUNNY) &&
		!CGame::IsInInterior() && !CCutsceneMgr::IsRunning() && (CTimer::GetFrameCounter() & 7) == 0) {
#ifdef FIX_BUGS
		if (FindPlayerPed() && (!FindPlayerPed()->CheckIfInTheAir() || FindPlayerPed()->CheckIfInTheAir() && FindPlayerPed()->GetPosition().z < 7.5f &&
			CClock::GetHours() > 6 && CClock::GetHours() < 18))
#else
		if (!FindPlayerPed()->CheckIfInTheAir() || FindPlayerPed()->CheckIfInTheAir() && FindPlayerPed()->GetPosition().z < 7.5f &&
			CClock::GetHours() > 6 && CClock::GetHours() < 18)
#endif
			AddHeatHaze();
	}

	if ((NewWeatherType == WEATHER_SUNNY || NewWeatherType == WEATHER_EXTRA_SUNNY) && !CGame::IsInInterior() && !CCutsceneMgr::IsRunning())
		AddBeastie();
}


void CWeather::AddHeatHaze()
{
	/*
	if(TheCamera.Cams[TheCamera.ActiveCam].Mode == CCam::MODE_TOPDOWN ||
	   TheCamera.Cams[TheCamera.ActiveCam].Mode == CCam::MODE_TOP_DOWN_PED)
		return;
	CVector pos;
	pos.x = SCREEN_WIDTH*0.5f;
	if(TheCamera.GetLookingForwardFirstPerson())
		pos.y = CGeneral::GetRandomNumberInRange(SCREEN_HEIGHT*0.25f, SCREEN_HEIGHT*0.9f);
	else
		pos.y = CGeneral::GetRandomNumberInRange(SCREEN_HEIGHT*0.4f, SCREEN_HEIGHT*0.9f);
	pos.z = 100.0f;
	CParticle::AddParticle(PARTICLE_HEATHAZE_IN_DIST, pos, CVector(0.0f, 0.0f, 0.0f));
	*/
}


void CWeather::AddBeastie()
{
	if(FindPlayerVehicle() || CTimer::GetFrameCounter()%10 || (CGeneral::GetRandomNumber()&5) == 0)
		return;
	CVector pos = TheCamera.GetPosition();
	float dist = CGeneral::GetRandomNumberInRange(90.0f, 60.0f);
	int angle = CGeneral::GetRandomNumber() % CParticle::SIN_COS_TABLE_SIZE;
	float c = CParticle::m_CosTable[angle];
	float s = CParticle::m_SinTable[angle];
	pos.x += dist*(c - s);
	pos.y += dist*(c + s);
	pos.z += CGeneral::GetRandomNumberInRange(7.5f, 30.0f);
	CParticle::AddParticle(PARTICLE_BEASTIE, pos, CVector(0.0f, 0.0f, 0.0f));
}

}

void CWeather::ForceWeather(int16 weather)
{
	ForcedWeatherType = weather;
}


void CWeather::ForceWeatherNow(int16 weather)
{
	OldWeatherType = weather;
	NewWeatherType = weather;
	ForcedWeatherType = weather;
}


void CWeather::ReleaseWeather()
{
	ForcedWeatherType = -1;
}


void CWeather::AddSplashesDuringHurricane()
{
	RwRGBA colour = { 255, 255, 255, 32 };
	CVector pos = TheCamera.pTargetEntity ? TheCamera.pTargetEntity->GetPosition() : TheCamera.GetPosition();
	bool foundGround;
	float groundZ = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &foundGround) + 0.1f;
	if(!foundGround)
		groundZ = pos.z + 0.5f;
	for(int i = 0; i < 20; i++){
		float dist = (CGeneral::GetRandomNumber()&0xFF)/255.0f +
			CGeneral::GetRandomNumberInRange(-10.0f, 30.0f);
		float angle;
		uint8 rnd = CGeneral::GetRandomNumber();
		if(rnd&1)
			angle = (CGeneral::GetRandomNumber()&0x7F)/128.0f * TWOPI;
		else
			angle = TheCamera.Orientation + (rnd-128)/160.0f;
		pos.x = TheCamera.GetPosition().x + dist*Sin(angle);
		pos.y = TheCamera.GetPosition().y + dist*Cos(angle);
		pos.z = groundZ;
		if(foundGround)
			CParticle::AddParticle(PARTICLE_GROUND_STEAM, pos, CVector(-0.002f, -0.002f, 0.015f), nil, 0.0f, colour);
	}
}


static int startStreamAfterRain;


void CWeather::AddStreamAfterRain()
{
	if(CClock::GetHours() > 6 && CClock::GetHours() < 18){
		RwRGBA colour = { 255, 255, 255, 24 };
		CVector pos = TheCamera.pTargetEntity ? TheCamera.pTargetEntity->GetPosition() : TheCamera.GetPosition();
		bool foundGround;
		float groundZ = CWorld::FindGroundZFor3DCoord(pos.x, pos.y, pos.z, &foundGround) + 0.2f;
		if(!foundGround)
			groundZ = pos.z + 0.75f;
		for(int i = 0; i < 20; i++){
			float dist = (CGeneral::GetRandomNumber()&0xFF)/255.0f +
				CGeneral::GetRandomNumberInRange(-10.0f, 30.0f);
			float angle;
			uint8 rnd = CGeneral::GetRandomNumber();
			if(rnd&1)
				angle = (CGeneral::GetRandomNumber()&0x7F)/128.0f * TWOPI;
			else
				angle = TheCamera.Orientation + (rnd-128)/160.0f;
			pos.x = TheCamera.GetPosition().x + dist*Sin(angle);
			pos.y = TheCamera.GetPosition().y + dist*Cos(angle);
			pos.z = groundZ;
			CParticle::AddParticle(PARTICLE_GROUND_STEAM, pos, CVector(0.0f, 0.0f, 0.015f), nil, 0.0f, colour);
		}
	}else{
		startStreamAfterRain = 0;
		StreamAfterRainTimer = 800;
	}
}


void CWeather::AddRain()
{
	if (CCullZones::CamNoRain() || CCullZones::PlayerNoRain())
		return;
	if (TheCamera.GetLookingLRBFirstPerson()) {
		CVehicle* pVehicle = FindPlayerVehicle();
		if (pVehicle && pVehicle->CarHasRoof()) {
			CParticle::RemovePSystem(PARTICLE_RAINDROP_2D);
			return;
		}
	}
	if (Rain <= 0.1f)
		return;
	static RwRGBA colour;
	float screen_width = SCREEN_WIDTH;
	float screen_height = SCREEN_HEIGHT;
	int cur_frame = (int)(3 * Rain) & 3;
	int num_drops = (int)(2 * Rain) + 2;
	static int STATIC_RAIN_ANGLE = -45;
	static int count = 1500;
	static int add_angle = 1;
	if (--count == 0) {
		count = 1;
		if (add_angle) {
			STATIC_RAIN_ANGLE += 12;
			if (STATIC_RAIN_ANGLE > 45) {
				count = 1500;
				add_angle = !add_angle;
			}
		}
		else {
			STATIC_RAIN_ANGLE -= 12;
			if (STATIC_RAIN_ANGLE < -45) {
				count = 1500;
				add_angle = !add_angle;
			}
		}
	}
	float rain_angle = DEGTORAD(STATIC_RAIN_ANGLE + ((STATIC_RAIN_ANGLE < 0) ? 360 : 0));
	float sin_angle = Sin(rain_angle);
	float cos_angle = Cos(rain_angle);
	float base_x = 0.0f * cos_angle - 1.0f * sin_angle;
	float base_y = 1.0f * cos_angle + 0.0f * sin_angle;
	CVector xpos(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 2 * num_drops; i++) {
		CVector dir;
		dir.x = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_x) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.y = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_y) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.z = 0;
		CParticle::AddParticle(PARTICLE_RAINDROP_2D, xpos, dir, nil, CGeneral::GetRandomNumberInRange(0.5f, 0.9f),
			colour, 0, rain_angle + CGeneral::GetRandomNumberInRange(-10, 10), cur_frame);
		xpos.x += screen_width / (2 * num_drops);
		xpos.x += CGeneral::GetRandomNumberInRange(-25.0f, 25.0f);
	}
	CVector ypos(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < num_drops; i++) {
		CVector dir;
		dir.x = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_x) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.y = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_y) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.z = 0;
		CParticle::AddParticle(PARTICLE_RAINDROP_2D, ypos, dir, nil, CGeneral::GetRandomNumberInRange(0.5f, 0.9f),
			colour, 0, rain_angle + CGeneral::GetRandomNumberInRange(-10, 10), cur_frame);
		ypos.y += screen_width / num_drops;
		ypos.y += CGeneral::GetRandomNumberInRange(-25.0f, 25.0f);
	}
	CVector ypos2(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < num_drops; i++) {
		CVector dir;
		dir.x = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_x) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.y = (CGeneral::GetRandomNumberInRange(-0.5f, 0.5f) + base_y) * CGeneral::GetRandomNumberInRange(10.0f, 25.0f);
		dir.z = 0;
		CParticle::AddParticle(PARTICLE_RAINDROP_2D, ypos2, dir, nil, CGeneral::GetRandomNumberInRange(0.5f, 0.9f),
			colour, 0, rain_angle + CGeneral::GetRandomNumberInRange(-10, 10), cur_frame);
		ypos2.y += screen_width / num_drops;
		ypos2.y += CGeneral::GetRandomNumberInRange(-25.0f, 25.0f);
	}
	for (int i = 0; i < num_drops; i++) {
		CVector pos;
		pos.x = CGeneral::GetRandomNumberInRange(DROPLETS_LEFT_OFFSET, screen_width - DROPLETS_RIGHT_OFFSET);
		pos.y = CGeneral::GetRandomNumberInRange(DROPLETS_TOP_OFFSET, screen_height - DROPLETS_TOP_OFFSET);
		pos.z = 0.0f;
		CParticle::AddParticle(PARTICLE_RAINDROP_2D, pos, CVector(0.0f, 0.0f, 0.0f), nil, CGeneral::GetRandomNumberInRange(0.5f, 0.9f),
			colour, CGeneral::GetRandomNumberInRange(-10, 10), 360 - rain_angle + CGeneral::GetRandomNumberInRange(-30, 30), cur_frame, 50);
	}
	int num_splash_attempts = (int)(3 * Rain) + 1;
	int num_splashes = (int)(3 * Rain) + 4;
	CVector splash_points[4];
	splash_points[0] = CVector(-RwCameraGetViewWindow(TheCamera.m_pRwCamera)->x, RwCameraGetViewWindow(TheCamera.m_pRwCamera)->y, 1.0f) *
		RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) / (RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) * *(CVector2D*)RwCameraGetViewWindow(TheCamera.m_pRwCamera)).Magnitude();
	splash_points[1] = CVector(RwCameraGetViewWindow(TheCamera.m_pRwCamera)->x, RwCameraGetViewWindow(TheCamera.m_pRwCamera)->y, 1.0f) *
		RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) / (RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) * *(CVector2D*)RwCameraGetViewWindow(TheCamera.m_pRwCamera)).Magnitude();
	splash_points[2] = 4.0f * CVector(-RwCameraGetViewWindow(TheCamera.m_pRwCamera)->x, RwCameraGetViewWindow(TheCamera.m_pRwCamera)->y, 1.0f) *
		RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) / (RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) * *(CVector2D*)RwCameraGetViewWindow(TheCamera.m_pRwCamera)).Magnitude();
	splash_points[3] = 4.0f * CVector(RwCameraGetViewWindow(TheCamera.m_pRwCamera)->x, RwCameraGetViewWindow(TheCamera.m_pRwCamera)->y, 1.0f) *
		RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) / (RwCameraGetFarClipPlane(TheCamera.m_pRwCamera) * *(CVector2D*)RwCameraGetViewWindow(TheCamera.m_pRwCamera)).Magnitude();
	RwV3dTransformPoints(splash_points, splash_points, 4, RwFrameGetMatrix(RwCameraGetFrame(TheCamera.m_pRwCamera)));
	CVector fp = (splash_points[0] + splash_points[1] + splash_points[2] + splash_points[3]) / 4;
	for (int i = 0; i < num_splash_attempts; i++) {
		CColPoint point;
		CEntity* entity;
		CVector np = fp + CVector(CGeneral::GetRandomNumberInRange(-SPLASH_CHECK_RADIUS, SPLASH_CHECK_RADIUS), CGeneral::GetRandomNumberInRange(-SPLASH_CHECK_RADIUS, SPLASH_CHECK_RADIUS), 0.0f);
		if (CWorld::ProcessVerticalLine(np + CVector(0.0f, 0.0f, 40.0f), -40.0f, point, entity, true, false, false, false, true, false, nil)) {
			for (int j = 0; j < num_splashes; j++)
				CParticle::AddParticle((CGeneral::GetRandomTrueFalse() ? PARTICLE_RAIN_SPLASH : PARTICLE_RAIN_SPLASHUP),
					CVector(
						np.x + CGeneral::GetRandomNumberInRange(-SPLASH_OFFSET_RADIUS, SPLASH_OFFSET_RADIUS),
						np.y + CGeneral::GetRandomNumberInRange(-SPLASH_OFFSET_RADIUS, SPLASH_OFFSET_RADIUS),
						point.point.z + 0.1f),
					CVector(0.0f, 0.0f, 0.0f), nil, 0.0f, colour);
		}
	}
}


void RenderOneRainStreak(CVector pos, CVector unused, int intensity, bool scale, float distance)
{
	static float RandomTex;
	static float RandomTexX;
	static float RandomTexY;
	TempBufferRenderIndexList[TempBufferIndicesStored + 0] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[TempBufferIndicesStored + 1] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[TempBufferIndicesStored + 2] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[TempBufferIndicesStored + 3] = TempBufferVerticesStored + 0;
	TempBufferRenderIndexList[TempBufferIndicesStored + 4] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[TempBufferIndicesStored + 5] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[TempBufferIndicesStored + 6] = TempBufferVerticesStored + 1;
	TempBufferRenderIndexList[TempBufferIndicesStored + 7] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[TempBufferIndicesStored + 8] = TempBufferVerticesStored + 4;
	TempBufferRenderIndexList[TempBufferIndicesStored + 9] = TempBufferVerticesStored + 2;
	TempBufferRenderIndexList[TempBufferIndicesStored + 10] = TempBufferVerticesStored + 3;
	TempBufferRenderIndexList[TempBufferIndicesStored + 11] = TempBufferVerticesStored + 4;
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 0], 0, 0, 0, 0);
	RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 0], pos.x + 11.0f * TheCamera.GetUp().x, pos.y + 11.0f * TheCamera.GetUp().y, pos.z + 11.0f * TheCamera.GetUp().z);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 1], 0, 0, 0, 0);
	RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 1], pos.x - 9.0f * TheCamera.GetRight().x, pos.y - 9.0f * TheCamera.GetRight().y, pos.z - 9.0f * TheCamera.GetRight().z);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 2], RAIN_COLOUR_R * intensity / 256, RAIN_COLOUR_G * intensity / 256, RAIN_COLOUR_B * intensity / 256, RAIN_ALPHA);
	RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 2], pos.x, pos.y, pos.z);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 3], 0, 0, 0, 0);
	RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 3], pos.x + 9.0f * TheCamera.GetRight().x, pos.y + 9.0f * TheCamera.GetRight().y, pos.z + 9.0f * TheCamera.GetRight().z);
	RwIm3DVertexSetRGBA(&TempBufferRenderVertices[TempBufferVerticesStored + 4], 0, 0, 0, 0); 
	RwIm3DVertexSetPos(&TempBufferRenderVertices[TempBufferVerticesStored + 4], pos.x - 11.0f * TheCamera.GetUp().x, pos.y - 11.0f * TheCamera.GetUp().y, pos.z - 11.0f * TheCamera.GetUp().z);
	float u = STREAK_U;
	float v = STREAK_V;
	if (scale) {
		u *= LARGE_STREAK_COEFFICIENT;
		v *= LARGE_STREAK_COEFFICIENT;
	}
	float distance_coefficient;
	if (distance < STREAK_MIN_DISTANCE)
		distance_coefficient = 1.0f;
	else if (distance > STREAK_MAX_DISTANCE)
		distance_coefficient = 0.5f;
	else
		distance_coefficient = 1.0f - 0.5f * (distance - STREAK_MIN_DISTANCE) / (STREAK_MAX_DISTANCE - STREAK_MIN_DISTANCE);
	u *= distance_coefficient;
	v *= distance_coefficient;
	if (!CTimer::GetIsPaused()) {
		RandomTex = ((CGeneral::GetRandomNumber() & 255) - 128) * 0.01f;
		RandomTexX = (CGeneral::GetRandomNumber() & 127) * 0.01f;
		RandomTexY = (CGeneral::GetRandomNumber() & 127) * 0.01f;
	}
	RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 0], 0.5f * u - RandomTex + RandomTexX);
	RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 0], -v * 0.5f + RandomTexY);
	RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 1], RandomTexX);
	RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 1], RandomTexY);
	RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 2], 0.5f * u + RandomTexX);
	RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 2], RandomTexY);
	RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 3], u + RandomTexX);
	RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 3], RandomTexY);
	RwIm3DVertexSetU(&TempBufferRenderVertices[TempBufferVerticesStored + 4], 0.5f * u + RandomTex + RandomTexX);
	RwIm3DVertexSetV(&TempBufferRenderVertices[TempBufferVerticesStored + 5], 0.5f * v + RandomTexY);
	TempBufferIndicesStored += 12;
	TempBufferVerticesStored += 5;
}


void CWeather::RenderRainStreaks(void)
{
	if (CTimer::GetIsCodePaused())
		return;
	int base_intensity = (64.0f - CTimeCycle::GetFogReduction()) / 64.0f * int(255 * Rain);
	if (base_intensity == 0)
		return;
	if (TheCamera.m_CameraAverageSpeed > 1.75f)
		return;
	TempBufferIndicesStored = 0;
	TempBufferVerticesStored = 0;
	for (int i = 0; i < NUM_RAIN_STREAKS; i++) {
		if (Streaks[i].timer) {
			float secondsElapsed = (CTimer::GetTimeInMilliseconds() - Streaks[i].timer) / 1024.0f;
			if (secondsElapsed > STREAK_LIFETIME)
				Streaks[i].timer = 0;
			else{
				int intensity;
				if (secondsElapsed < STREAK_INTEROLATION_TIME)
					intensity = base_intensity * 0.25f * secondsElapsed / STREAK_INTEROLATION_TIME;
				else if (secondsElapsed > (STREAK_LIFETIME - STREAK_INTEROLATION_TIME))
					intensity = (STREAK_LIFETIME - secondsElapsed) * 0.25f * base_intensity / STREAK_INTEROLATION_TIME;
				else
					intensity = base_intensity * 0.25f;
				CVector dir = Streaks[i].direction;
				dir.Normalise();
				CVector pos = Streaks[i].position + secondsElapsed * Streaks[i].direction;
				RenderOneRainStreak(pos, dir, intensity, false, (pos - TheCamera.GetPosition()).Magnitude());
#ifndef FIX_BUGS // remove useless code
				if (secondsElapsed > 1.0f && secondsElapsed < STREAK_LIFETIME - 1.0f) {
					CGeneral::GetRandomNumber(), CGeneral::GetRandomNumber();
				}
#endif
			}
		}
		else if ((CGeneral::GetRandomNumber() & 0xF00) == 0){
			// 1/16 probability
			Streaks[i].direction = CVector(0.0f, 0.0f, -12.0f);
			Streaks[i].position = 6.0f * TheCamera.GetForward() + TheCamera.GetPosition() + CVector(-1.8f * Streaks[i].direction.x, -1.8f * Streaks[i].direction.y, 8.0f);
			if (!CCutsceneMgr::IsRunning()) {
				Streaks[i].position.x += 2.0f * FindPlayerSpeed().x * 60.0f;
				Streaks[i].position.y += 2.0f * FindPlayerSpeed().y * 60.0f;
			}
			else
				Streaks[i].position += (TheCamera.GetPosition() - TheCamera.m_RealPreviousCameraPosition) * 20.0f;
			Streaks[i].position.x += ((CGeneral::GetRandomNumber() & 255) - 128) * 0.04f;
			Streaks[i].position.y += ((CGeneral::GetRandomNumber() & 255) - 128) * 0.04f;
			Streaks[i].timer = CTimer::GetTimeInMilliseconds();
		}
	}
	if (TempBufferIndicesStored){
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void*)rwFOGTYPELINEAR);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(gpRainDropTex));
		if (RwIm3DTransform(TempBufferRenderVertices, TempBufferVerticesStored, nil, 1))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, TempBufferRenderIndexList, TempBufferIndicesStored);
			RwIm3DEnd();
		}
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	}
	TempBufferVerticesStored = 0;
	TempBufferIndicesStored = 0;
}


void CWeather::StoreWeatherState()
{
	Stored_StateStored = true;
	Stored_InterpolationValue = InterpolationValue;
	Stored_Rain = Rain;
	Stored_NewWeatherType = NewWeatherType;
	Stored_OldWeatherType = OldWeatherType;
}


void CWeather::RestoreWeatherState()
{
#ifdef FIX_BUGS // it's not used anyway though
	Stored_StateStored = false;
#endif
	InterpolationValue = Stored_InterpolationValue;
	Rain = Stored_Rain;
	NewWeatherType = Stored_NewWeatherType;
	OldWeatherType = Stored_OldWeatherType;
}


#ifdef SECUROM

void CWeather::ForceHurricaneWeather()
{
	for (int i = 0; i < ARRAY_SIZE(WeatherTypesList_WithHurricanes); i++)
	{
		WeatherTypesList[i] = WEATHER_HURRICANE;
		WeatherTypesList_WithHurricanes[i] = WEATHER_HURRICANE;
	}

	CWeather::OldWeatherType = WEATHER_HURRICANE;
	CWeather::NewWeatherType = WEATHER_HURRICANE;
	CWeather::ForcedWeatherType = WEATHER_HURRICANE;
}

#endif


const int16 WeatherTypesList[] = {
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_CLOUDY, WEATHER_CLOUDY, WEATHER_RAINY, WEATHER_RAINY,
	WEATHER_CLOUDY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_CLOUDY, WEATHER_FOGGY, WEATHER_FOGGY, WEATHER_CLOUDY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_CLOUDY, WEATHER_CLOUDY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_CLOUDY, WEATHER_CLOUDY, WEATHER_RAINY, WEATHER_RAINY,
	WEATHER_CLOUDY, WEATHER_RAINY, WEATHER_CLOUDY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_FOGGY, WEATHER_FOGGY, WEATHER_SUNNY,
	WEATHER_SUNNY, WEATHER_SUNNY, WEATHER_RAINY, WEATHER_CLOUDY,
};


const float Windyness[] = {
	0.0f, // WEATHER_SUNNY
	0.7f, // WEATHER_CLOUDY
	1.0f, // WEATHER_RAINY
	0.5f  // WEATHER_FOGGY
};

	InterpolationValue = fNewInterpolation;

	if (CPad::GetPad(1)->GetRightShockJustDown()) {
		NewWeatherType = (NewWeatherType + 1) % WEATHER_TOTAL;
		OldWeatherType = NewWeatherType;
	}


	// Lightning
	if (NewWeatherType != WEATHER_RAINY || OldWeatherType != WEATHER_RAINY) {
		LightningFlash = false;
		LightningBurst = false;
	}

	else{
		if (LightningBurst) {
			if ((CGeneral::GetRandomNumber() & 255) >= 32) {
				// 0.875 probability
				if (CTimer::GetTimeInMilliseconds() - LightningFlashLastChange > MIN_TIME_BETWEEN_LIGHTNING_FLASH_CHANGES) {
					bool bOldLightningFlash = LightningFlash;
					LightningFlash = CGeneral::GetRandomTrueFalse();
					if (LightningFlash != bOldLightningFlash)
						LightningFlashLastChange = CTimer::GetTimeInMilliseconds();
				}
			}
			else {
				// 0.125 probability
				LightningBurst = false;
				LightningDuration = Min(CTimer::GetFrameCounter() - LightningStart, 20);
				LightningFlash = false;
				WhenToPlayLightningSound = CTimer::GetTimeInMilliseconds() + 150 * (20 - LightningDuration);
			}
		}
		else {
			if (CGeneral::GetRandomNumber() >= 200) {
				// lower probability on PC due to randomness bug
				LightningFlash = false;
			}
			else {
				LightningBurst = true;
				LightningStart = CTimer::GetFrameCounter();
				LightningFlashLastChange = CTimer::GetTimeInMilliseconds();
				LightningFlash = true;
			}
		}
	}

	if (WhenToPlayLightningSound && CTimer::GetTimeInMilliseconds() > WhenToPlayLightningSound) {
		DMAudio.PlayOneShot(SoundHandle, SOUND_LIGHTNING, LightningDuration);
		CPad::GetPad(0)->StartShake(40 * LightningDuration + 100, 2 * LightningDuration + 80);
		WhenToPlayLightningSound = 0;
	}


	// Wet roads
	if (OldWeatherType == WEATHER_RAINY) {
		if (NewWeatherType == WEATHER_RAINY)
			WetRoads = 1.0f;
		else
			WetRoads = 1.0f - InterpolationValue;
	}

	else {
		if (NewWeatherType == WEATHER_RAINY)
			WetRoads = InterpolationValue;
		else
			WetRoads = 0.0f;
	}


	// Rain
#ifndef VC_RAIN_NERF

	float fNewRain;

	if (NewWeatherType == WEATHER_RAINY) {
		// if raining for >1 hour, values: 0, 0.33, switching every ~16.5s
		fNewRain = (((uint16)CTimer::GetTimeInMilliseconds() >> 14) & 1) * 0.33f;
		if (OldWeatherType != WEATHER_RAINY) {
			if (InterpolationValue < 0.4f)
				// if rain has just started (<24 minutes), always 0.5
				fNewRain = 0.5f;
			else
				// if rain is ongoing for >24 minutes, values: 0.25, 0.5, switching every ~16.5s
				fNewRain = 0.25f + (((uint16)CTimer::GetTimeInMilliseconds() >> 14) & 1) * 0.25f;
		}
		fNewRain = Max(fNewRain, 0.5f);
	}

	else
		fNewRain = 0.0f;

	if (Rain != fNewRain) { // ok to use comparasion
		if (Rain < fNewRain)
			Rain = Min(fNewRain, Rain + RAIN_CHANGE_SPEED * CTimer::GetTimeStep());
		else
			Rain = Max(fNewRain, Rain - RAIN_CHANGE_SPEED * CTimer::GetTimeStep());
	}

#else

	Rain = fNewRain;


	// Clouds
	if (OldWeatherType != WEATHER_SUNNY)
		CloudCoverage = 1.0f - InterpolationValue;

	else
		CloudCoverage = 0.0f;

	if (NewWeatherType != WEATHER_SUNNY)
		CloudCoverage += InterpolationValue;

	
	// Fog
	if (OldWeatherType == WEATHER_FOGGY)
		Foggyness = 1.0f - InterpolationValue;

	else
		Foggyness = 0.0f;

	if (NewWeatherType == WEATHER_FOGGY)
		Foggyness += InterpolationValue;

	if (OldWeatherType == WEATHER_RAINY && NewWeatherType == WEATHER_SUNNY && InterpolationValue < 0.5f && CClock::GetHours() > 6 && CClock::GetHours() < 21)
		Rainbow = 1.0f - 4.0f * Abs(InterpolationValue - 0.25f) / 4.0f;

	else
		Rainbow = 0.0f;

	Wind = InterpolationValue * Windyness[NewWeatherType] + (1.0f - InterpolationValue) * Windyness[OldWeatherType];

	AddRain();
