#include "common.h"


#include "General.h"

#include "CutsceneMgr.h"

#include "Directory.h"

#include "Camera.h"

#include "Streaming.h"

#include "FileMgr.h"

#include "main.h"

#include "AnimManager.h"

#include "AnimBlendAssociation.h"

#include "AnimBlendAssocGroup.h"

#include "AnimBlendClumpData.h"

#include "Pad.h"

#include "DMAudio.h"

#include "World.h"

#include "PlayerPed.h"

#include "Wanted.h"

#include "RpAnimBlend.h"

#include "ModelIndices.h"

#include "TempColModels.h"

#include "ColStore.h"

#include "Radar.h"

#include "Pools.h"


const struct {
	const char *szTrackName;
	int iTrackId;
} musicNameIdAssoc[] = {
	{ "ASS_1", STREAMED_SOUND_CUTSCENE_ASS_1 },
	{ "ASS_2", STREAMED_SOUND_CUTSCENE_ASS_2 },
	{ "BANK_1", STREAMED_SOUND_CUTSCENE_BANK_1 },
	{ "BANK_2A", STREAMED_SOUND_CUTSCENE_BANK_2A },
	{ "BANK_2B", STREAMED_SOUND_CUTSCENE_BANK_2B },
	{ "BANK_3A", STREAMED_SOUND_CUTSCENE_BANK_3A },
	{ "BANK_3B", STREAMED_SOUND_CUTSCENE_BANK_3B },
	{ "BANK_4", STREAMED_SOUND_CUTSCENE_BANK_4 },
	{ "BIKE_1", STREAMED_SOUND_CUTSCENE_BIKE_1 },
	{ "BIKE_2", STREAMED_SOUND_CUTSCENE_BIKE_2 },
	{ "BIKE_3", STREAMED_SOUND_CUTSCENE_BIKE_3 },
	{ "BUD_1", STREAMED_SOUND_CUTSCENE_BUD_1 },
	{ "BUD_2", STREAMED_SOUND_CUTSCENE_BUD_2 },
	{ "BUD_3", STREAMED_SOUND_CUTSCENE_BUD_3 },
	{ "CAP_1", STREAMED_SOUND_CUTSCENE_CAP_1 },
	{ "CAR_1", STREAMED_SOUND_CUTSCENE_CAR_1 },
	{ "CNT_1A", STREAMED_SOUND_CUTSCENE_CNT_1A },
	{ "CNT_1B", STREAMED_SOUND_CUTSCENE_CNT_1B },
	{ "CNT_2", STREAMED_SOUND_CUTSCENE_CNT_2 },
	{ "COK_1", STREAMED_SOUND_CUTSCENE_COK_1 },
	{ "COK_2A", STREAMED_SOUND_CUTSCENE_COK_2A },
	{ "COK_2B", STREAMED_SOUND_CUTSCENE_COK_2B },
	{ "COK_3", STREAMED_SOUND_CUTSCENE_COK_3 },
	{ "COK_4A", STREAMED_SOUND_CUTSCENE_COK_4A },
	{ "COK_4A2", STREAMED_SOUND_CUTSCENE_COK_4A2 },
	{ "COK_4B", STREAMED_SOUND_CUTSCENE_COK_4B },
	{ "COL_1", STREAMED_SOUND_CUTSCENE_COL_1 },
	{ "COL_2", STREAMED_SOUND_CUTSCENE_COL_2 },
	{ "COL_3A", STREAMED_SOUND_CUTSCENE_COL_3A },
	{ "COL_4A", STREAMED_SOUND_CUTSCENE_COL_4A },
	{ "COL_5A", STREAMED_SOUND_CUTSCENE_COL_5A },
	{ "COL_5B", STREAMED_SOUND_CUTSCENE_COL_5B },
	{ "CUB_1", STREAMED_SOUND_CUTSCENE_CUB_1 },
	{ "CUB_2", STREAMED_SOUND_CUTSCENE_CUB_2 },
	{ "CUB_3", STREAMED_SOUND_CUTSCENE_CUB_3 },
	{ "CUB_4", STREAMED_SOUND_CUTSCENE_CUB_4 },
	{ "DRUG_1", STREAMED_SOUND_CUTSCENE_DRUG_1 },
	{ "FIN", STREAMED_SOUND_CUTSCENE_FIN },
	{ "FIN_2", STREAMED_SOUND_CUTSCENE_FIN2 },
	{ "FINALE", STREAMED_SOUND_CUTSCENE_FINALE },
	{ "HAT_1", STREAMED_SOUND_CUTSCENE_HAT_1 },
	{ "HAT_2", STREAMED_SOUND_CUTSCENE_HAT_2 },
	{ "HAT_3", STREAMED_SOUND_CUTSCENE_HAT_3 },
	{ "ICE_1", STREAMED_SOUND_CUTSCENE_ICE_1 },
	{ "INT_A", STREAMED_SOUND_CUTSCENE_INT_A },
	{ "INT_B", STREAMED_SOUND_CUTSCENE_INT_B },
	{ "INT_D", STREAMED_SOUND_CUTSCENE_INT_D },
	{ "INT_M", STREAMED_SOUND_CUTSCENE_INT_M },
	{ "LAW_1A", STREAMED_SOUND_CUTSCENE_LAW_1A },
	{ "LAW_1B", STREAMED_SOUND_CUTSCENE_LAW_1B },
	{ "LAW_2A", STREAMED_SOUND_CUTSCENE_LAW_2A },
	{ "LAW_2B", STREAMED_SOUND_CUTSCENE_LAW_2B },
	{ "LAW_2C", STREAMED_SOUND_CUTSCENE_LAW_2C },
	{ "LAW_3", STREAMED_SOUND_CUTSCENE_LAW_3 },
	{ "LAW_4", STREAMED_SOUND_CUTSCENE_LAW_4 },
	{ "PHIL_1", STREAMED_SOUND_CUTSCENE_PHIL_1 },
	{ "PHIL_2", STREAMED_SOUND_CUTSCENE_PHIL_2 },
	{ "PORN_1", STREAMED_SOUND_CUTSCENE_PORN_1 },
	{ "PORN_2", STREAMED_SOUND_CUTSCENE_PORN_2 },
	{ "PORN_3", STREAMED_SOUND_CUTSCENE_PORN_3 },
	{ "PORN_4", STREAMED_SOUND_CUTSCENE_PORN_4 },
	{ "RESC_1A", STREAMED_SOUND_CUTSCENE_RESC_1A },
	{ "ROK_1", STREAMED_SOUND_CUTSCENE_ROK_1 },
	{ "ROK_2", STREAMED_SOUND_CUTSCENE_ROK_2 },
	{ "ROK_3A", STREAMED_SOUND_CUTSCENE_ROK_3A },
	{ "STRIPA", STREAMED_SOUND_CUTSCENE_STRIPA },
	{ "TAX_1", STREAMED_SOUND_CUTSCENE_TAX_1 },
	{ "TEX_1", STREAMED_SOUND_CUTSCENE_TEX_1 },
	{ "TEX_2", STREAMED_SOUND_CUTSCENE_TEX_2 },
	{ "TEX_3", STREAMED_SOUND_CUTSCENE_TEX_3 },
	{ "GSPOT", STREAMED_SOUND_CUTSCENE_GLIGHT },
	{ "FIST", STREAMED_SOUND_CUTSCENE_FIST },
	{ "EL_PH1", STREAMED_SOUND_CUTSCENE_ELBURRO1_PH1 },
	{ "EL_PH2", STREAMED_SOUND_CUTSCENE_ELBURRO2_PH2 },
	{ NULL, 0 }
};


int
FindCutsceneAudioTrackId(const char *szCutsceneName)
{
	for (int i = 0; musicNameIdAssoc[i].szTrackName; i++) {
		if (!CGeneral::faststricmp(musicNameIdAssoc[i].szTrackName, szCutsceneName))
			return musicNameIdAssoc[i].iTrackId;
	}
	return -1;
}


bool CCutsceneMgr::ms_running;

bool CCutsceneMgr::ms_cutsceneProcessing;

CDirectory *CCutsceneMgr::ms_pCutsceneDir;

CCutsceneObject *CCutsceneMgr::ms_pCutsceneObjects[NUMCUTSCENEOBJECTS];

int32 CCutsceneMgr::ms_numCutsceneObjs;

bool CCutsceneMgr::ms_loaded;

bool CCutsceneMgr::ms_animLoaded;

bool CCutsceneMgr::ms_useLodMultiplier;

char CCutsceneMgr::ms_cutsceneName[CUTSCENENAMESIZE];

CAnimBlendAssocGroup CCutsceneMgr::ms_cutsceneAssociations;

CVector CCutsceneMgr::ms_cutsceneOffset;

float CCutsceneMgr::ms_cutsceneTimer;

bool CCutsceneMgr::ms_wasCutsceneSkipped;

uint32 CCutsceneMgr::ms_cutsceneLoadStatus;

bool CCutsceneMgr::ms_useCutsceneShadows = true;


bool bCamLoaded;

bool bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver; // pls don't shrink the name :P

int32 NumberOfSavedWeapons;

eWeaponType SavedWeaponIDs[TOTAL_WEAPON_SLOTS];

int32 SavedWeaponAmmo[TOTAL_WEAPON_SLOTS];

char uncompressedAnims[8][32];

uint32 numUncompressedAnims;



RpAtomic *
CalculateBoundingSphereRadiusCB(RpAtomic *atomic, void *data)
{
	float radius = RpAtomicGetBoundingSphere(atomic)->radius;
	RwV3d center = RpAtomicGetBoundingSphere(atomic)->center;

	for (RwFrame *frame = RpAtomicGetFrame(atomic); RwFrameGetParent(frame); frame = RwFrameGetParent(frame))
		RwV3dTransformPoints(&center, &center, 1, RwFrameGetMatrix(frame));

	float size = RwV3dLength(&center) + radius;
	if (size > *(float *)data)
		*(float *)data = size;
	return atomic;
}


void
CCutsceneMgr::Initialise(void)
{
	ms_numCutsceneObjs = 0;
	ms_loaded = false;
	ms_wasCutsceneSkipped = false;
	ms_running = false;
	ms_useLodMultiplier = false;
	ms_animLoaded = false;
	ms_cutsceneProcessing = false;

	ms_pCutsceneDir = new CDirectory(CUTSCENEDIRSIZE);
	ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.DIR");

	numUncompressedAnims = 0;
	uncompressedAnims[0][0] = '\0';
}


void
CCutsceneMgr::Shutdown(void)
{
	delete ms_pCutsceneDir;
}


void
CCutsceneMgr::LoadCutsceneData(const char *szCutsceneName)
{
	int file;
	uint32 size;
	uint32 offset;
	CPlayerPed *pPlayerPed;

	ms_cutsceneProcessing = true;
	ms_wasCutsceneSkipped = false;
	CTimer::Suspend();
	if (!bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver)
		CStreaming::RemoveCurrentZonesModels();

	ms_pCutsceneDir->numEntries = 0;
	ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.DIR");

	CStreaming::RemoveUnusedModelsInLoadedList();
	CGame::DrasticTidyUpMemory(true);

	strcpy(ms_cutsceneName, szCutsceneName);

	RwStream *stream;
	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
	assert(stream);

	// Load animations
	sprintf(gString, "%s.IFP", szCutsceneName);
	if (ms_pCutsceneDir->FindItem(gString, offset, size)) {
		CStreaming::MakeSpaceFor(size << 11);
		CStreaming::ImGonnaUseStreamingMemory();
		RwStreamSkip(stream,  offset << 11);
		CAnimManager::LoadAnimFile(stream, true, uncompressedAnims);
		ms_cutsceneAssociations.CreateAssociations(szCutsceneName);
		CStreaming::IHaveUsedStreamingMemory();
		ms_animLoaded = true;
	} else {
		ms_animLoaded = false;
	}
	RwStreamClose(stream, nil);

	// Load camera data
	file = CFileMgr::OpenFile("ANIM\\CUTS.IMG", "rb");
	sprintf(gString, "%s.DAT", szCutsceneName);
	if (ms_pCutsceneDir->FindItem(gString, offset, size)) {
		CStreaming::ImGonnaUseStreamingMemory();
		CFileMgr::Seek(file, offset << 11, SEEK_SET);
		TheCamera.LoadPathSplines(file);
		CStreaming::IHaveUsedStreamingMemory();
		bCamLoaded = true;
	} else {
		bCamLoaded = false;
	}

	CFileMgr::CloseFile(file);

	if (CGeneral::faststricmp(ms_cutsceneName, "finale")) {
		DMAudio.ChangeMusicMode(MUSICMODE_CUTSCENE);
		int trackId = FindCutsceneAudioTrackId(szCutsceneName);
		if (trackId != -1) {
			printf("Start preload audio %s\n", szCutsceneName);
			DMAudio.PreloadCutSceneMusic(trackId);
			printf("End preload audio %s\n", szCutsceneName);
		}
	}

	ms_cutsceneTimer = 0.0f;
	ms_loaded = true;
	ms_cutsceneOffset = CVector(0.0f, 0.0f, 0.0f);

	pPlayerPed = FindPlayerPed();
	pPlayerPed->m_pWanted->ClearQdCrimes();
	pPlayerPed->bIsVisible = false;
	pPlayerPed->m_fCurrentStamina = pPlayerPed->m_fMaxStamina;
	CPad::GetPad(0)->SetDisablePlayerControls(PLAYERCONTROL_CUTSCENE);
	CWorld::Players[CWorld::PlayerInFocus].MakePlayerSafe(true);

	CTimer::Resume();
}


void
CCutsceneMgr::FinishCutscene()
{
	ms_wasCutsceneSkipped = true;
	if (bCamLoaded) {
		CCutsceneMgr::ms_cutsceneTimer = TheCamera.GetCutSceneFinishTime() * 0.001f;
		TheCamera.FinishCutscene();
	}

	FindPlayerPed()->bIsVisible = true;
	CWorld::Players[CWorld::PlayerInFocus].MakePlayerSafe(false);
}


void
CCutsceneMgr::SetupCutsceneToStart(void)
{
	if (bCamLoaded) {
		TheCamera.SetCamCutSceneOffSet(ms_cutsceneOffset);
		TheCamera.TakeControlWithSpline(JUMP_CUT);
		TheCamera.SetWideScreenOn();
	}

	ms_cutsceneOffset.z++;

	for (int i = ms_numCutsceneObjs - 1; i >= 0; i--) {
		assert(RwObjectGetType(ms_pCutsceneObjects[i]->m_rwObject) == rpCLUMP);
		if (CAnimBlendAssociation *pAnimBlendAssoc = RpAnimBlendClumpGetFirstAssociation((RpClump*)ms_pCutsceneObjects[i]->m_rwObject)) {
			assert(pAnimBlendAssoc->hierarchy->sequences[0].HasTranslation());
			if (ms_pCutsceneObjects[i]->m_pAttachTo != nil) {
				pAnimBlendAssoc->flags &= (~ASSOC_HAS_TRANSLATION);
			} else {
				if (pAnimBlendAssoc->hierarchy->IsCompressed()){
					KeyFrameTransCompressed *keyFrames = ((KeyFrameTransCompressed*)pAnimBlendAssoc->hierarchy->sequences[0].GetKeyFrameCompressed(0));
					CVector trans;
					keyFrames->GetTranslation(&trans);
					ms_pCutsceneObjects[i]->SetPosition(ms_cutsceneOffset + trans);
				}else{
					KeyFrameTrans *keyFrames = ((KeyFrameTrans*)pAnimBlendAssoc->hierarchy->sequences[0].GetKeyFrame(0));
					ms_pCutsceneObjects[i]->SetPosition(ms_cutsceneOffset + keyFrames->translation);
				}
			}
			pAnimBlendAssoc->SetRun();
		} else {
			ms_pCutsceneObjects[i]->SetPosition(ms_cutsceneOffset);
		}
		CWorld::Add(ms_pCutsceneObjects[i]);
		if (RwObjectGetType(ms_pCutsceneObjects[i]->m_rwObject) == rpCLUMP) {
			ms_pCutsceneObjects[i]->UpdateRpHAnim();
		}
	}

	CTimer::Update();
	CTimer::Update();
	ms_running = true;
	ms_cutsceneTimer = 0.0f;
}


void
CCutsceneMgr::SetCutsceneAnim(const char *animName, CObject *pObject)
{
	CAnimBlendAssociation *pNewAnim;
	CAnimBlendClumpData *pAnimBlendClumpData;

	assert(RwObjectGetType(pObject->m_rwObject) == rpCLUMP);
	debug("Give cutscene anim %s\n", animName);
	RpAnimBlendClumpRemoveAllAssociations((RpClump*)pObject->m_rwObject);

	pNewAnim = ms_cutsceneAssociations.GetAnimation(animName);
	if (!pNewAnim) {
		debug("\n\nHaven't I told you I can't find the fucking animation %s\n\n\n", animName);
		return;
	}

	if (pNewAnim->hierarchy->IsCompressed())
		pNewAnim->hierarchy->keepCompressed = true;

	CStreaming::ImGonnaUseStreamingMemory();
	pNewAnim = ms_cutsceneAssociations.CopyAnimation(animName);
	CStreaming::IHaveUsedStreamingMemory();

	pNewAnim->SetCurrentTime(0.0f);
	pNewAnim->flags |= ASSOC_HAS_TRANSLATION;
	pNewAnim->flags &= ~ASSOC_RUNNING;

	pAnimBlendClumpData = *RPANIMBLENDCLUMPDATA(pObject->m_rwObject);
	pAnimBlendClumpData->link.Prepend(&pNewAnim->link);

	if (pNewAnim->hierarchy->keepCompressed)
		pAnimBlendClumpData->frames->flag |= AnimBlendFrameData::COMPRESSED;
}


void
CCutsceneMgr::SetCutsceneAnimToLoop(const char* animName)
{
	ms_cutsceneAssociations.GetAnimation(animName)->flags |= ASSOC_REPEAT;
}


CCutsceneHead *
CCutsceneMgr::AddCutsceneHead(CObject *pObject, int modelId)
{
	CCutsceneHead *pHead = new CCutsceneHead(pObject);
	pHead->SetModelIndex(modelId);
	CWorld::Add(pHead);
	ms_pCutsceneObjects[ms_numCutsceneObjs++] = pHead;
	return pHead;
}


void UpdateCutsceneObjectBoundingBox(RpClump* clump, int modelId)
{
	if (modelId >= MI_CUTOBJ01 && modelId <= MI_CUTOBJ05) {
		CColModel* pColModel = &CTempColModels::ms_colModelCutObj[modelId - MI_CUTOBJ01];
		float radius = 0.0f;
		RpClumpForAllAtomics(clump, CalculateBoundingSphereRadiusCB, &radius);
		pColModel->boundingSphere.radius = radius;
		pColModel->boundingBox.min = CVector(-radius, -radius, -radius);
		pColModel->boundingBox.max = CVector(radius, radius, radius);
	}
}


CCutsceneObject *
CCutsceneMgr::CreateCutsceneObject(int modelId)
{
	CBaseModelInfo *pModelInfo;
	CColModel *pColModel;
	CCutsceneObject *pCutsceneObject;

	CStreaming::ImGonnaUseStreamingMemory();
	debug("Created cutscene object %s\n", CModelInfo::GetModelInfo(modelId)->GetModelName());
	if (modelId >= MI_CUTOBJ01 && modelId <= MI_CUTOBJ05) {
		pModelInfo = CModelInfo::GetModelInfo(modelId);
		pColModel = &CTempColModels::ms_colModelCutObj[modelId - MI_CUTOBJ01];
		pModelInfo->SetColModel(pColModel);
		UpdateCutsceneObjectBoundingBox((RpClump*)pModelInfo->GetRwObject(), modelId);
	} else if (modelId >= MI_SPECIAL01 && modelId <= MI_SPECIAL21) {
		pModelInfo = CModelInfo::GetModelInfo(modelId);
		if (pModelInfo->GetColModel() == &CTempColModels::ms_colModelPed1) {
			CColModel *colModel = new CColModel();
			colModel->boundingSphere.radius = 2.0f;
			colModel->boundingSphere.center = CVector(0.0f, 0.0f, 0.0f);
			pModelInfo->SetColModel(colModel, true);
		}
		pColModel = pModelInfo->GetColModel();
		float radius = 2.0f;
		pColModel->boundingSphere.radius = radius;
		pColModel->boundingBox.min = CVector(-radius, -radius, -radius);
		pColModel->boundingBox.max = CVector(radius, radius, radius);
	}

	pCutsceneObject = new CCutsceneObject();
	pCutsceneObject->SetModelIndex(modelId);
	if (ms_useCutsceneShadows)
		pCutsceneObject->CreateShadow();
	ms_pCutsceneObjects[ms_numCutsceneObjs++] = pCutsceneObject;
	CStreaming::IHaveUsedStreamingMemory();
	return pCutsceneObject;
}


void
CCutsceneMgr::DeleteCutsceneData(void)
{
	if (!ms_loaded) return;
	CTimer::Suspend();

	ms_cutsceneProcessing = false;
	ms_useLodMultiplier = false;
	ms_useCutsceneShadows = true;

	for (--ms_numCutsceneObjs; ms_numCutsceneObjs >= 0; ms_numCutsceneObjs--) {
		CWorld::Remove(ms_pCutsceneObjects[ms_numCutsceneObjs]);
		ms_pCutsceneObjects[ms_numCutsceneObjs]->DeleteRwObject();
		delete ms_pCutsceneObjects[ms_numCutsceneObjs];
		ms_pCutsceneObjects[ms_numCutsceneObjs] = nil;
	}
	ms_numCutsceneObjs = 0;

	for (int i = MI_SPECIAL01; i < MI_SPECIAL21; i++) {
		CBaseModelInfo *minfo = CModelInfo::GetModelInfo(i);
		CColModel *colModel = minfo->GetColModel();
		if (colModel != &CTempColModels::ms_colModelPed1) {
			delete colModel;
			minfo->SetColModel(&CTempColModels::ms_colModelPed1);
		}
	}

	if (ms_animLoaded)
		CAnimManager::RemoveLastAnimFile();

	ms_animLoaded = false;
	numUncompressedAnims = 0;
	uncompressedAnims[0][0] = '\0';

	if (bCamLoaded) {
		TheCamera.RestoreWithJumpCut();
		TheCamera.SetWideScreenOff();
		TheCamera.DeleteCutSceneCamDataMemory();
	}
	ms_running = false;
	ms_loaded = false;

	FindPlayerPed()->bIsVisible = true;
	CPad::GetPad(0)->SetEnablePlayerControls(PLAYERCONTROL_CUTSCENE);
	CWorld::Players[CWorld::PlayerInFocus].MakePlayerSafe(false);

	if (CGeneral::faststricmp(ms_cutsceneName, "finale")) {
		DMAudio.StopCutSceneMusic();
		DMAudio.ChangeMusicMode(MUSICMODE_GAME);
	}

	CStreaming::ms_disableStreaming = false;
	CWorld::bProcessCutsceneOnly = false;

	if(bCamLoaded)
		CGame::DrasticTidyUpMemory(TheCamera.GetScreenFadeStatus() == FADE_2);
	
	CPad::GetPad(0)->Clear(false);
	if (bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver) {
		CStreaming::LoadInitialPeds();
		CStreaming::LoadInitialWeapons();
		CStreaming::LoadInitialVehicles();
		bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver = false;
		
		CPlayerPed *pPlayerPed = FindPlayerPed();
		for (int i = 0; i < NumberOfSavedWeapons; i++) {
			int32 weaponModelId = CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModelId;
			uint8 flags = CStreaming::ms_aInfoForModel[weaponModelId].m_flags;
			CStreaming::RequestModel(weaponModelId, STREAMFLAGS_DONT_REMOVE);
			CStreaming::LoadAllRequestedModels(false);
			if (CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModel2Id != -1) {
				CStreaming::RequestModel(CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModel2Id, 0);
				CStreaming::LoadAllRequestedModels(false);
			}
			if (!(flags & STREAMFLAGS_DONT_REMOVE))
				CStreaming::SetModelIsDeletable(weaponModelId);
			pPlayerPed->GiveWeapon(SavedWeaponIDs[i], SavedWeaponAmmo[i], true);
		}
		NumberOfSavedWeapons = 0;
	}

	CTimer::Resume();
}


void
CCutsceneMgr::Update(void)
{
	enum {
		CUTSCENE_LOADING_0 = 0,
		CUTSCENE_LOADING_AUDIO,
		CUTSCENE_LOADING_2,
		CUTSCENE_LOADING_3,
		CUTSCENE_LOADING_4
	};

	switch (ms_cutsceneLoadStatus) {
	case CUTSCENE_LOADING_AUDIO:
		SetupCutsceneToStart();
		if (CGeneral::faststricmp(ms_cutsceneName, "finale"))
			DMAudio.PlayPreloadedCutSceneMusic();
		ms_cutsceneLoadStatus++;
		break;
	case CUTSCENE_LOADING_2:
	case CUTSCENE_LOADING_3:
		ms_cutsceneLoadStatus++;
		break;
	case CUTSCENE_LOADING_4:
		ms_cutsceneLoadStatus = CUTSCENE_LOADING_0;
		break;
	default:
		break;
	}

	if (!ms_running) return;

	ms_cutsceneTimer += CTimer::GetTimeStepNonClippedInSeconds();

	for (int i = 0; i < ms_numCutsceneObjs; i++) {
		int modelId = ms_pCutsceneObjects[i]->GetModelIndex();
		if (modelId >= MI_CUTOBJ01 && modelId <= MI_CUTOBJ05)
			UpdateCutsceneObjectBoundingBox(ms_pCutsceneObjects[i]->GetClump(), modelId);

		if (ms_pCutsceneObjects[i]->m_pAttachTo != nil && modelId >= MI_SPECIAL01 && modelId <= MI_SPECIAL21)
			UpdateCutsceneObjectBoundingBox(ms_pCutsceneObjects[i]->GetClump(), modelId);
	}

	if (bCamLoaded)
		if (CGeneral::faststricmp(ms_cutsceneName, "finale") && TheCamera.Cams[TheCamera.ActiveCam].Mode == CCam::MODE_FLYBY && ms_cutsceneLoadStatus == CUTSCENE_LOADING_0) {
			if (CPad::GetPad(0)->GetCrossJustDown()
				|| (CGame::playingIntro && CPad::GetPad(0)->GetStartJustDown())
				|| CPad::GetPad(0)->GetLeftMouseJustDown()
				|| CPad::GetPad(0)->GetEnterJustDown()
				|| CPad::GetPad(0)->GetCharJustDown(' '))
					FinishCutscene();
		}
}


bool CCutsceneMgr::HasCutsceneFinished(void) { return !bCamLoaded || TheCamera.GetPositionAlongSpline() == 1.0f; }

void
CCutsceneMgr::LoadAnimationUncompressed(char const* name)
{
	strcpy(uncompressedAnims[numUncompressedAnims], name);
	
	// Because that's how CAnimManager knows the end of array
	++numUncompressedAnims;
	assert(numUncompressedAnims < ARRAY_SIZE(uncompressedAnims));
	uncompressedAnims[numUncompressedAnims][0] = '\0';
}


void
CCutsceneMgr::AttachObjectToParent(CObject *pObject, CEntity *pAttachTo)
{
	((CCutsceneObject*)pObject)->m_pAttachmentObject = nil;
	((CCutsceneObject*)pObject)->m_pAttachTo = RpClumpGetFrame(pAttachTo->GetClump());

	debug("Attach %s to %s\n", CModelInfo::GetModelInfo(pObject->GetModelIndex())->GetModelName(), CModelInfo::GetModelInfo(pAttachTo->GetModelIndex())->GetModelName());
}


void
CCutsceneMgr::AttachObjectToFrame(CObject *pObject, CEntity *pAttachTo, const char *frame)
{
	((CCutsceneObject*)pObject)->m_pAttachmentObject = nil;
	((CCutsceneObject*)pObject)->m_pAttachTo = RpAnimBlendClumpFindFrame(pAttachTo->GetClump(), frame)->frame;
	debug("Attach %s to component %s of %s\n",
		CModelInfo::GetModelInfo(pObject->GetModelIndex())->GetModelName(),
		frame,
		CModelInfo::GetModelInfo(pAttachTo->GetModelIndex())->GetModelName());
	if (RwObjectGetType(pObject->m_rwObject) == rpCLUMP) {
		RpClump *clump = (RpClump*)pObject->m_rwObject;
		if (IsClumpSkinned(clump))
			RpAtomicGetBoundingSphere(GetFirstAtomic(clump))->radius *= 1.1f;
	}
}


void
CCutsceneMgr::AttachObjectToBone(CObject *pObject, CObject *pAttachTo, int bone)
{
	RpHAnimHierarchy *hanim = GetAnimHierarchyFromSkinClump(pAttachTo->GetClump());
	RwInt32 id = RpHAnimIDGetIndex(hanim, bone);
	RwMatrix *matrixArray = RpHAnimHierarchyGetMatrixArray(hanim);
	((CCutsceneObject*)pObject)->m_pAttachmentObject = pAttachTo;
	((CCutsceneObject*)pObject)->m_pAttachTo = &matrixArray[id];
	debug("Attach %s to %s\n",
		CModelInfo::GetModelInfo(pObject->GetModelIndex())->GetModelName(),
		CModelInfo::GetModelInfo(pAttachTo->GetModelIndex())->GetModelName());
}


void
CCutsceneMgr::RemoveEverythingFromTheWorldForTheBiggestFuckoffCutsceneEver()
{
	CStreaming::ms_disableStreaming = true;
	CColStore::RemoveAllCollision();
	CWorld::bProcessCutsceneOnly = true;
	ms_cutsceneProcessing = true;

	for (int i = CPools::GetPedPool()->GetSize() - 1; i >= 0; i--) {
		CPed *pPed = CPools::GetPedPool()->GetSlot(i);
		if (pPed) {
			if (!pPed->IsPlayer() && pPed->CanBeDeleted()) {
				CWorld::Remove(pPed);
				delete pPed;
			}
		}
	}

	for (int i = CPools::GetVehiclePool()->GetSize() - 1; i >= 0; i--) {
		CVehicle *pVehicle = CPools::GetVehiclePool()->GetSlot(i);
		if (pVehicle) {
			if (pVehicle->CanBeDeleted()) {
				CWorld::Remove(pVehicle);
				delete pVehicle;
			}
		}
	}

	bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver = true;
	CStreaming::RemoveCurrentZonesModels();
	CStreaming::SetModelIsDeletable(MI_MALE01);
	CStreaming::SetModelTxdIsDeletable(MI_MALE01);
	CStreaming::SetModelIsDeletable(MI_TAXI_D);
	CStreaming::SetModelTxdIsDeletable(MI_TAXI_D);
	CStreaming::SetModelIsDeletable(MI_NIGHTSTICK);
	CStreaming::SetModelTxdIsDeletable(MI_NIGHTSTICK);
	CStreaming::SetModelIsDeletable(MI_MISSILE);
	CStreaming::SetModelTxdIsDeletable(MI_MISSILE);
	CStreaming::SetModelIsDeletable(MI_POLICE);
	CStreaming::SetModelTxdIsDeletable(MI_POLICE);

	while (CStreaming::RemoveLoadedVehicle()) ;

	CRadar::RemoveRadarSections();

	for (int i = CPools::GetDummyPool()->GetSize() - 1; i >= 0; i--) {
		CDummy* pDummy = CPools::GetDummyPool()->GetSlot(i);
		if (pDummy)
			pDummy->DeleteRwObject();
	}

	for (int i = CPools::GetObjectPool()->GetSize() - 1; i >= 0; i--) {
		CObject* pObject = CPools::GetObjectPool()->GetSlot(i);
		if (pObject)
			pObject->DeleteRwObject();
	}

	for (int i = CPools::GetBuildingPool()->GetSize() - 1; i >= 0; i--) {
		CBuilding* pBuilding = CPools::GetBuildingPool()->GetSlot(i);
		if (pBuilding && pBuilding->m_rwObject != nil && pBuilding->bIsBIGBuilding && pBuilding->bStreamBIGBuilding) {
			if (pBuilding->bIsBIGBuilding)
				CStreaming::RequestModel(pBuilding->GetModelIndex(), 0);
			if (!pBuilding->bImBeingRendered)
				pBuilding->DeleteRwObject();
		}
	}

	CPlayerPed *pPlayerPed = FindPlayerPed();
	pPlayerPed->RemoveWeaponAnims(0, -1000.0f);
	NumberOfSavedWeapons = 0;

	for (int i = 0; i < TOTAL_WEAPON_SLOTS; i++) {
		if (pPlayerPed->m_weapons[i].m_eWeaponType != WEAPONTYPE_UNARMED) {
			SavedWeaponIDs[NumberOfSavedWeapons] = pPlayerPed->m_weapons[i].m_eWeaponType;
			SavedWeaponAmmo[NumberOfSavedWeapons] = pPlayerPed->m_weapons[i].m_nAmmoTotal;
			NumberOfSavedWeapons++;
		}
	}

	pPlayerPed->ClearWeapons();
	CGame::DrasticTidyUpMemory(true);
}

#include "CutsceneHead.h"


const struct {
	const char *szTrackName;
	int iTrackId;
} musicNameIdAssoc[] = {
	{ "JB",			STREAMED_SOUND_NEWS_INTRO },
	{ "BET",		STREAMED_SOUND_BANK_INTRO },
	{ "L1_LG",		STREAMED_SOUND_CUTSCENE_LUIGI1_LG },
	{ "L2_DSB",		STREAMED_SOUND_CUTSCENE_LUIGI2_DSB },
	{ "L3_DM",		STREAMED_SOUND_CUTSCENE_LUIGI3_DM },
	{ "L4_PAP",		STREAMED_SOUND_CUTSCENE_LUIGI4_PAP },
	{ "L5_TFB",		STREAMED_SOUND_CUTSCENE_LUIGI5_TFB },
	{ "J0_DM2",		STREAMED_SOUND_CUTSCENE_JOEY0_DM2 },
	{ "J1_LFL",		STREAMED_SOUND_CUTSCENE_JOEY1_LFL },
	{ "J2_KCL",		STREAMED_SOUND_CUTSCENE_JOEY2_KCL },
	{ "J3_VH",		STREAMED_SOUND_CUTSCENE_JOEY3_VH },
	{ "J4_ETH",		STREAMED_SOUND_CUTSCENE_JOEY4_ETH },
	{ "J5_DST",		STREAMED_SOUND_CUTSCENE_JOEY5_DST },
	{ "J6_TBJ",		STREAMED_SOUND_CUTSCENE_JOEY6_TBJ },
	{ "T1_TOL",		STREAMED_SOUND_CUTSCENE_TONI1_TOL },
	{ "T2_TPU",		STREAMED_SOUND_CUTSCENE_TONI2_TPU },
	{ "T3_MAS",		STREAMED_SOUND_CUTSCENE_TONI3_MAS },
	{ "T4_TAT",		STREAMED_SOUND_CUTSCENE_TONI4_TAT },
	{ "T5_BF",		STREAMED_SOUND_CUTSCENE_TONI5_BF },
	{ "S0_MAS",		STREAMED_SOUND_CUTSCENE_SAL0_MAS },
	{ "S1_PF",		STREAMED_SOUND_CUTSCENE_SAL1_PF },
	{ "S2_CTG",		STREAMED_SOUND_CUTSCENE_SAL2_CTG },
	{ "S3_RTC",		STREAMED_SOUND_CUTSCENE_SAL3_RTC },
	{ "S5_LRQ",		STREAMED_SOUND_CUTSCENE_SAL5_LRQ },
	{ "S4_BDBA",	STREAMED_SOUND_CUTSCENE_SAL4_BDBA },
	{ "S4_BDBB",	STREAMED_SOUND_CUTSCENE_SAL4_BDBB },
	{ "S2_CTG2",	STREAMED_SOUND_CUTSCENE_SAL2_CTG2 },
	{ "S4_BDBD",	STREAMED_SOUND_CUTSCENE_SAL4_BDBD },
	{ "S5_LRQB",	STREAMED_SOUND_CUTSCENE_SAL5_LRQB },
	{ "S5_LRQC",	STREAMED_SOUND_CUTSCENE_SAL5_LRQC },
	{ "A1_SS0",		STREAMED_SOUND_CUTSCENE_ASUKA_1_SSO },
	{ "A2_PP",		STREAMED_SOUND_CUTSCENE_ASUKA_2_PP },
	{ "A3_SS",		STREAMED_SOUND_CUTSCENE_ASUKA_3_SS },
	{ "A4_PDR",		STREAMED_SOUND_CUTSCENE_ASUKA_4_PDR },
	{ "A5_K2FT",	STREAMED_SOUND_CUTSCENE_ASUKA_5_K2FT},
	{ "K1_KBO",		STREAMED_SOUND_CUTSCENE_KENJI1_KBO },
	{ "K2_GIS",		STREAMED_SOUND_CUTSCENE_KENJI2_GIS },
	{ "K3_DS",		STREAMED_SOUND_CUTSCENE_KENJI3_DS },
	{ "K4_SHI",		STREAMED_SOUND_CUTSCENE_KENJI4_SHI },
	{ "K5_SD",		STREAMED_SOUND_CUTSCENE_KENJI5_SD },
	{ "R0_PDR2",	STREAMED_SOUND_CUTSCENE_RAY0_PDR2 },
	{ "R1_SW",		STREAMED_SOUND_CUTSCENE_RAY1_SW },
	{ "R2_AP",		STREAMED_SOUND_CUTSCENE_RAY2_AP },
	{ "R3_ED",		STREAMED_SOUND_CUTSCENE_RAY3_ED },
	{ "R4_GF",		STREAMED_SOUND_CUTSCENE_RAY4_GF },
	{ "R5_PB",		STREAMED_SOUND_CUTSCENE_RAY5_PB },
	{ "R6_MM",		STREAMED_SOUND_CUTSCENE_RAY6_MM },
	{ "D1_STOG",	STREAMED_SOUND_CUTSCENE_DONALD1_STOG },
	{ "D2_KK",		STREAMED_SOUND_CUTSCENE_DONALD2_KK },
	{ "D3_ADO",		STREAMED_SOUND_CUTSCENE_DONALD3_ADO },
	{ "D5_ES",		STREAMED_SOUND_CUTSCENE_DONALD5_ES },
	{ "D7_MLD",		STREAMED_SOUND_CUTSCENE_DONALD7_MLD },
	{ "D4_GTA",		STREAMED_SOUND_CUTSCENE_DONALD4_GTA },
	{ "D4_GTA2",	STREAMED_SOUND_CUTSCENE_DONALD4_GTA2 },
	{ "D6_STS",		STREAMED_SOUND_CUTSCENE_DONALD6_STS },
	{ "A6_BAIT",	STREAMED_SOUND_CUTSCENE_ASUKA6_BAIT },
	{ "A7_ETG",		STREAMED_SOUND_CUTSCENE_ASUKA7_ETG },
	{ "A8_PS",		STREAMED_SOUND_CUTSCENE_ASUKA8_PS },
	{ "A9_ASD",		STREAMED_SOUND_CUTSCENE_ASUKA9_ASD },
	{ "K4_SHI2",	STREAMED_SOUND_CUTSCENE_KENJI4_SHI2 },
	{ "C1_TEX",		STREAMED_SOUND_CUTSCENE_CATALINA1_TEX },
	{ "EL_PH1",		STREAMED_SOUND_CUTSCENE_ELBURRO1_PH1 },
	{ "EL_PH2",		STREAMED_SOUND_CUTSCENE_ELBURRO2_PH2 },
	{ "EL_PH3",		STREAMED_SOUND_CUTSCENE_ELBURRO3_PH3 },
	{ "EL_PH4",		STREAMED_SOUND_CUTSCENE_ELBURRO4_PH4 },
	{ "YD_PH1",		STREAMED_SOUND_CUTSCENE_YARDIE_PH1 },
	{ "YD_PH2",		STREAMED_SOUND_CUTSCENE_YARDIE_PH2 },
	{ "YD_PH3",		STREAMED_SOUND_CUTSCENE_YARDIE_PH3 },
	{ "YD_PH4",		STREAMED_SOUND_CUTSCENE_YARDIE_PH4 },
	{ "HD_PH1",		STREAMED_SOUND_CUTSCENE_HOODS_PH1 },
	{ "HD_PH2",		STREAMED_SOUND_CUTSCENE_HOODS_PH2 },
	{ "HD_PH3",		STREAMED_SOUND_CUTSCENE_HOODS_PH3 },
	{ "HD_PH4",		STREAMED_SOUND_CUTSCENE_HOODS_PH4 },
	{ "HD_PH5",		STREAMED_SOUND_CUTSCENE_HOODS_PH5 },
	{ "MT_PH1",		STREAMED_SOUND_CUTSCENE_MARTY_PH1 },
	{ "MT_PH2",		STREAMED_SOUND_CUTSCENE_MARTY_PH2 },
	{ "MT_PH3",		STREAMED_SOUND_CUTSCENE_MARTY_PH3 },
	{ "MT_PH4",		STREAMED_SOUND_CUTSCENE_MARTY_PH4 },
	{ NULL,			0 }
};


void
CCutsceneMgr::SetHeadAnim(const char *animName, CObject *pObject)
{
	CCutsceneHead *pCutsceneHead = (CCutsceneHead*)pObject;
	char szAnim[CUTSCENENAMESIZE * 2];

	sprintf(szAnim, "%s_%s", ms_cutsceneName, animName);
	pCutsceneHead->PlayAnimation(szAnim);
}

#include "crossplatform.h"


static bool bModelsRemovedForCutscene;

static int32 NumberOfSavedWeapons;

static eWeaponType SavedWeaponIDs[TOTAL_WEAPON_SLOTS];

static int32 SavedWeaponAmmo[TOTAL_WEAPON_SLOTS];


char CCutsceneMgr::ms_cAppendAnimName[NUMCUTSCENEOBJECTS][NAMELENGTH];

char CCutsceneMgr::ms_cAppendObjectName[NUMCUTSCENEOBJECTS][NAMELENGTH];

int CCutsceneMgr::ms_numAppendObjectNames;

bool CCutsceneMgr::ms_hasFileInfo;


void
CCutsceneMgr::Initialise(void *dir)
{
	ms_cutsceneLoadStatus = CUTSCENE_NOT_LOADED;
	ms_running = false;
	ms_animLoaded = false;
	ms_cutsceneProcessing = false;
	ms_useLodMultiplier = false;
	ms_wasCutsceneSkipped = false;
	ms_hasFileInfo = false;
	//ms_numCutsceneObjs = 0;
	//ms_loaded = false;
	if (gMakeResources) {
		ms_pCutsceneDir = new CDirectory(CUTSCENEDIRSIZE);
		ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.DIR");
	}
	else
		ms_pCutsceneDir = (CDirectory*)dir;

	//numUncompressedAnims = 0;
	//uncompressedAnims[0][0] = '\0';
}


void CCutsceneMgr::Write(base::cRelocatableChunkWriter& writer)
{
	writer.AllocateRaw(ms_pCutsceneDir, sizeof(*ms_pCutsceneDir), 4, false, true);
	writer.AllocateRaw(ms_pCutsceneDir->entries, sizeof(CDirectory::DirectoryInfo) * ms_pCutsceneDir->numEntries, 4, false, true);
	writer.AddPatch(ms_pCutsceneDir);
}


void
CCutsceneMgr::RemoveEverythingBecauseCutsceneDoesntFitInMemory()
{
	//CStreaming::ms_disableStreaming = true;
	CWorld::ClearExcitingStuffFromArea(FindPlayerCoors(), 120.0f, true);
	CColStore::RemoveAllCollision();
	CWorld::bProcessCutsceneOnly = true;
	/*ms_cutsceneProcessing = true;

	for (int i = CPools::GetPedPool()->GetSize() - 1; i >= 0; i--) {
		CPed *pPed = CPools::GetPedPool()->GetSlot(i);
		if (pPed) {
			if (!pPed->IsPlayer() && pPed->CanBeDeleted()) {
				CWorld::Remove(pPed);
				delete pPed;
			}
		}
	}

	for (int i = CPools::GetVehiclePool()->GetSize() - 1; i >= 0; i--) {
		CVehicle *pVehicle = CPools::GetVehiclePool()->GetSlot(i);
		if (pVehicle) {
			if (pVehicle->CanBeDeleted()) {
				CWorld::Remove(pVehicle);
				delete pVehicle;
			}
		}
	}
	CWorld::bProcessCutsceneOnly = true;
	//bIsEverythingRemovedFromTheWorldForTheBiggestFuckoffCutsceneEver = true;*/
	CStreaming::RemoveCurrentZonesModels();
	while (CStreaming::RemoveLoadedVehicle());
	CRadar::RemoveRadarSections();
	CStreaming::SetModelIsDeletable(MI_MALE01);
	CStreaming::SetModelTxdIsDeletable(MI_MALE01);
	CStreaming::SetModelIsDeletable(MI_COLT45);
	CStreaming::SetModelTxdIsDeletable(MI_COLT45);
	CStreaming::SetModelIsDeletable(MI_NIGHTSTICK);
	CStreaming::SetModelTxdIsDeletable(MI_NIGHTSTICK);
	CStreaming::SetModelIsDeletable(MI_MISSILE);
	CStreaming::SetModelTxdIsDeletable(MI_MISSILE);

	/*for (int i = CPools::GetDummyPool()->GetSize() - 1; i >= 0; i--) {
		CDummy* pDummy = CPools::GetDummyPool()->GetSlot(i);
		if (pDummy)
			pDummy->DeleteRwObject();
	}

	for (int i = CPools::GetObjectPool()->GetSize() - 1; i >= 0; i--) {
		CObject* pObject = CPools::GetObjectPool()->GetSlot(i);
		if (pObject)
			pObject->DeleteRwObject();
	}

	for (int i = CPools::GetBuildingPool()->GetSize() - 1; i >= 0; i--) {
		CBuilding* pBuilding = CPools::GetBuildingPool()->GetSlot(i);
		if (pBuilding && pBuilding->m_rwObject != nil && pBuilding->bIsBIGBuilding && pBuilding->bStreamBIGBuilding) {
			if (pBuilding->bIsBIGBuilding)
				CStreaming::RequestModel(pBuilding->GetModelIndex(), 0);
			if (!pBuilding->bImBeingRendered)
				pBuilding->DeleteRwObject();
		}
	}*/

	CPlayerPed *pPlayerPed = FindPlayerPed();
	pPlayerPed->RemoveWeaponAnims(0, -1000.0f);
	NumberOfSavedWeapons = 0;

	for (int i = 0; i < TOTAL_WEAPON_SLOTS; i++) {
		if (pPlayerPed->m_weapons[i].m_eWeaponType != WEAPONTYPE_UNARMED) {
			SavedWeaponIDs[NumberOfSavedWeapons] = pPlayerPed->m_weapons[i].m_eWeaponType;
			SavedWeaponAmmo[NumberOfSavedWeapons] = pPlayerPed->m_weapons[i].m_nAmmoTotal;
			NumberOfSavedWeapons++;
		}
	}

	pPlayerPed->ClearWeapons();
	bModelsRemovedForCutscene = true;
	//CGame::DrasticTidyUpMemory(true);
}


void
CCutsceneMgr::LoadEverythingBecauseCutsceneDeletedAllOfIt()
{
	bModelsRemovedForCutscene = false;
	CStreaming::LoadInitialPeds();
	CStreaming::LoadInitialWeapons();
	//CStreaming::LoadInitialVehicles();
		
	CPlayerPed *pPlayerPed = FindPlayerPed();
	for (int i = 0; i < NumberOfSavedWeapons; i++) {
		int32 weaponModelId = CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModelId;
		uint8 flags = CStreaming::ms_aInfoForModel[weaponModelId].m_flags;
		CStreaming::RequestModel(weaponModelId, STREAMFLAGS_DONT_REMOVE);
		CStreaming::LoadAllRequestedModels(false);
		if (CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModel2Id != -1) {
			CStreaming::RequestModel(CWeaponInfo::GetWeaponInfo(SavedWeaponIDs[i])->m_nModel2Id, 0);
			CStreaming::LoadAllRequestedModels(false);
		}
		if (!(flags & STREAMFLAGS_DONT_REMOVE))
			CStreaming::SetModelIsDeletable(weaponModelId);
		pPlayerPed->GiveWeapon(SavedWeaponIDs[i], SavedWeaponAmmo[i], true);
	}
	NumberOfSavedWeapons = 0;
}


void
CCutsceneMgr::LoadAnimationUncompressed(char const* name)
{
	strcpy(ms_aUncompressedCutsceneAnims[ms_numUncompressedCutsceneAnims], name);
	
	// Because that's how CAnimManager knows the end of array
	++ms_numUncompressedCutsceneAnims;
	assert(ms_numUncompressedCutsceneAnims < ARRAY_SIZE(ms_aUncompressedCutsceneAnims));
	ms_aUncompressedCutsceneAnims[ms_numUncompressedCutsceneAnims][0] = '\0';
}



bool
CCutsceneMgr::IsCutsceneSkipButtonBeingPressed()
{
	return (CPad::GetPad(0)->GetCrossJustDown()
		|| CPad::GetPad(0)->GetLeftMouseJustDown()
		|| CPad::GetPad(0)->GetEnterJustDown()
		|| CPad::GetPad(0)->GetCharJustDown(' '));
}


void
CCutsceneMgr::AppendToNextCutscene(const char *object, const char *anim)
{
	strcpy(ms_cAppendObjectName[ms_numAppendObjectNames], object);
	strlwr(ms_cAppendObjectName[ms_numAppendObjectNames]);
	strcpy(ms_cAppendAnimName[ms_numAppendObjectNames], anim);
	strlwr(ms_cAppendAnimName[ms_numAppendObjectNames]);
	ms_numAppendObjectNames++;
}
