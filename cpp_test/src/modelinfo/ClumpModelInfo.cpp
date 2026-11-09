#include "common.h"


#include "main.h"

#include "RwHelper.h"

#include "General.h"

#include "NodeName.h"

#include "VisibilityPlugins.h"

#include "ModelInfo.h"

#include "AnimManager.h"

#include "Streaming.h"

#include "Leeds.h"


base::cRelocatableChunkClassInfo CClumpModelInfo::msClassInfo("CElementGroupModelInfo", VTABLE_ADDR(&msClassInstance), sizeof(msClassInstance)); // the real name

CClumpModelInfo CClumpModelInfo::msClassInstance;


void
CClumpModelInfo::DeleteRwObject(void)
{
	if(m_clump){
		if(!gUseChunkFiles)
			RpClumpDestroy(m_clump);
		else{
			CStreaming::UnregisterClump(m_clump);
			CStreaming::UnregisterPointer(&m_clump, 2);
			DeleteChunk();
		}

		m_clump = nil;
		RemoveTexDictionaryRef();
		if(GetAnimFileIndex() != -1)
			CAnimManager::RemoveAnimBlockRef(GetAnimFileIndex());
	}
}


static RpAtomic*
SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
	RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy*)data);
	return nil;
}


RwObject*
CClumpModelInfo::CreateInstance(void)
{
	if(m_clump == nil)
		return nil;
	RpClump *clone = RpClumpClone(m_clump);
#ifdef PED_SKIN
	if(IsClumpSkinned(clone)){
		RpHAnimHierarchy *hier;
		RpHAnimAnimation *anim;

		hier = GetAnimHierarchyFromClump(clone);
		assert(hier);
		// This seems dangerous as only the first atomic will get a hierarchy
		// can we guarantee this if hands and head are also in the clump?
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);
		anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, (RpHAnimHierarchyFlag)(rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS));
		// the rest is xbox only:
		// RpSkinGetNumBones(RpSkinGeometryGetSkin(RpAtomicGetGeometry(IsClumpSkinned(clone))));
		RpHAnimHierarchyUpdateMatrices(hier);
	}
#endif
	return (RwObject*)clone;
}


RwObject*
CClumpModelInfo::CreateInstance(RwMatrix *m)
{
	if(m_clump){
		RpClump *clump = (RpClump*)CreateInstance();
		*RwFrameGetMatrix(RpClumpGetFrame(clump)) = *m;
		CStreaming::RegisterInstance(clump);
		return (RwObject*)clump;
	}
	return nil;
}


RpAtomic*
CClumpModelInfo::SetAtomicRendererCB(RpAtomic *atomic, void *data)
{
	CVisibilityPlugins::SetAtomicRenderCallback(atomic, (RpAtomicCallBackRender)data);
	return atomic;
}


void
CClumpModelInfo::SetClump(RpClump *clump)
{
	m_clump = clump;
	CVisibilityPlugins::SetClumpModelInfo(m_clump, this);
	AddTexDictionaryRef();
	RpClumpForAllAtomics(clump, SetAtomicRendererCB, nil);

#ifdef PED_SKIN
	if(IsClumpSkinned(clump)){
		int i;
		RpHAnimHierarchy *hier;
		RpAtomic *skinAtomic;
		RpSkin *skin;

		// mobile:
//		hier = nil;
//		RwFrameForAllChildren(RpClumpGetFrame(clump), GetHierarchyFromChildNodesCB, &hier);
//		assert(hier);
//		RpClumpForAllAtomics(clump, SetHierarchyForSkinAtomic, hier);
//		skinAtomic = GetFirstAtomic(clump);

		// xbox:
		hier = GetAnimHierarchyFromClump(clump);
		assert(hier);
		RpSkinAtomicSetHAnimHierarchy(IsClumpSkinned(clump), hier);
		skinAtomic = IsClumpSkinned(clump);

		assert(skinAtomic);
		skin = RpSkinGeometryGetSkin(RpAtomicGetGeometry(skinAtomic));
		// ignore const
		for(i = 0; i < RpGeometryGetNumVertices(RpAtomicGetGeometry(skinAtomic)); i++){
			RwMatrixWeights *weights = (RwMatrixWeights*)&RpSkinGetVertexBoneWeights(skin)[i];
			float sum = weights->w0 + weights->w1 + weights->w2 + weights->w3;
			weights->w0 /= sum;
			weights->w1 /= sum;
			weights->w2 /= sum;
			weights->w3 /= sum;
		}
		RpHAnimHierarchySetFlags(hier, (RpHAnimHierarchyFlag)(rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS));
	}
	if(strcmp(GetModelName(), "playerh") == 0){
		// playerh is incompatible with the xbox player skin
		// so check if player model is skinned and only apply skin to head if it isn't
		CPedModelInfo *body = (CPedModelInfo*)CModelInfo::GetModelInfo(MI_PLAYER);
		if(!(body->m_clump && IsClumpSkinned(body->m_clump)))
			RpClumpForAllAtomics(clump, SetAtomicRendererCB, (void*)CVisibilityPlugins::RenderPlayerCB);
	}
#else
	if(strcmp(GetModelName(), "playerh") == 0)
		RpClumpForAllAtomics(clump, SetAtomicRendererCB, (void*)CVisibilityPlugins::RenderPlayerCB);
#endif
}


void
CClumpModelInfo::SetAnimFile(const char *file)
{
	if(strcasecmp(file, "null") == 0)
		return;

	m_animFileName = new char[strlen(file)+1];
	strcpy(m_animFileName, file);
}


void
CClumpModelInfo::ConvertAnimFileIndex(void)
{
	if(m_animFileIndex != -1){
		// we have a string pointer in that union
		int32 index = CAnimManager::GetAnimationBlockIndex(m_animFileName);
		delete[] m_animFileName;
		m_animFileIndex = index;
	}
}


void
CClumpModelInfo::SetFrameIds(RwObjectNameIdAssocation *assocs)
{
	int32 i;
	RwObjectNameAssociation objname;

	for(i = 0; assocs[i].name; i++)
		if((assocs[i].flags & CLUMP_FLAG_NO_HIERID) == 0){
			objname.frame = nil;
			objname.name = assocs[i].name;
			RwFrameForAllChildren(RpClumpGetFrame(m_clump), FindFrameFromNameWithoutIdCB, &objname);
			if(objname.frame)
				CVisibilityPlugins::SetFrameHierarchyId(objname.frame, assocs[i].hierId);
		}
}


RwFrame*
CClumpModelInfo::FindFrameFromIdCB(RwFrame *frame, void *data)
{
	RwObjectIdAssociation *assoc = (RwObjectIdAssociation*)data;

	if(CVisibilityPlugins::GetFrameHierarchyId(frame) == assoc->id){
		assoc->frame = frame;
		return nil;
	}
	RwFrameForAllChildren(frame, FindFrameFromIdCB, assoc);
	return assoc->frame ? nil : frame;
}


// unused
RwFrame*
CClumpModelInfo::FindFrameFromNameCB(RwFrame *frame, void *data)
{
	RwObjectNameAssociation *assoc = (RwObjectNameAssociation*)data;

	if(!CGeneral::faststricmp(GetFrameNodeName(frame), assoc->name)){
		assoc->frame = frame;
		return nil;
	}
	RwFrameForAllChildren(frame, FindFrameFromNameCB, assoc);
	return assoc->frame ? nil : frame;
}


RwFrame*
CClumpModelInfo::FindFrameFromNameWithoutIdCB(RwFrame *frame, void *data)
{
	RwObjectNameAssociation *assoc = (RwObjectNameAssociation*)data;

	if(CVisibilityPlugins::GetFrameHierarchyId(frame) == 0 &&
	   !CGeneral::faststricmp(GetFrameNodeName(frame), assoc->name)){
		assoc->frame = frame;
		return nil;
	}
	RwFrameForAllChildren(frame, FindFrameFromNameWithoutIdCB, assoc);
	return assoc->frame ? nil : frame;
}


RwFrame*
CClumpModelInfo::FillFrameArrayCB(RwFrame *frame, void *data)
{
	int32 id;
	RwFrame **frames = (RwFrame**)data;
	id = CVisibilityPlugins::GetFrameHierarchyId(frame);
	if(id > 0)
		frames[id] = frame;
	RwFrameForAllChildren(frame, FillFrameArrayCB, data);
	return frame;
}


void
CClumpModelInfo::FillFrameArray(RpClump *clump, RwFrame **frames)
{
	RwFrameForAllChildren(RpClumpGetFrame(clump), FillFrameArrayCB, frames);
}


RwFrame*
CClumpModelInfo::GetFrameFromId(RpClump *clump, int32 id)
{
	RwObjectIdAssociation assoc;
	assoc.id = id;
	assoc.frame = nil;
	RwFrameForAllChildren(RpClumpGetFrame(clump), FindFrameFromIdCB, &assoc);
	return assoc.frame;
}



void
CClumpModelInfo::LoadModel(void *clump, const void *chunk)
{
	m_chunk = (void*)chunk;
	m_clump = (RpClump*)clump;
	LoadResource(m_clump);
	CStreaming::RegisterPointer(&m_chunk, 2, true);
	CStreaming::RegisterClump(m_clump);
	CStreaming::RegisterPointer(&m_clump, 2, true);
}


void
CClumpModelInfo::Write(base::cRelocatableChunkWriter &writer)
{
	CBaseModelInfo::Write(writer);
	if(m_clump){
		writer.AddPatch(&m_clump);
		SaveResource(m_clump, writer);
	}
}


void*
CClumpModelInfo::WriteModel(base::cRelocatableChunkWriter &writer)
{
	if(m_clump)
		SaveResource(m_clump, writer);
	return m_clump;
}


void
CClumpModelInfo::RcWriteThis(base::cRelocatableChunkWriter &writer)
{
	writer.AllocateRaw(this, sizeof(*this), sizeof(void*), false, true);
	writer.Class(VTABLE_ADDR(this), msClassInfo);
}


void
CClumpModelInfo::RcWriteEmpty(base::cRelocatableChunkWriter &writer)
{
	writer.AllocateRaw(this, sizeof(*this), sizeof(void*), false, true);
	writer.Class(VTABLE_ADDR(this), msClassInfo);
}

#include "ModelIndices.h"


#ifdef PED_SKIN

#endif
