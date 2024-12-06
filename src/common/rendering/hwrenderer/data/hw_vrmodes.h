#pragma once

#include "gl_renderer.h"
#include "matrix.h"

class DFrameBuffer;
using namespace OpenGLRenderer;

enum
{
	VR_MONO = 0,
	VR_GREENMAGENTA = 1,
	VR_REDCYAN = 2,
	VR_SIDEBYSIDEFULL = 3,
	VR_SIDEBYSIDESQUISHED = 4,
	VR_LEFTEYEVIEW = 5,
	VR_RIGHTEYEVIEW = 6,
	VR_QUADSTEREO = 7,
	VR_SIDEBYSIDELETTERBOX = 8,
	VR_AMBERBLUE = 9,
	VR_OPENVR = 10,
	VR_TOPBOTTOM = 11,
	VR_ROWINTERLEAVED = 12,
	VR_COLUMNINTERLEAVED = 13,
	VR_CHECKERINTERLEAVED = 14
};

struct VREyeInfo
{
	float mShiftFactor;
	float mScaleFactor;


	VREyeInfo() {}
	VREyeInfo(float shiftFactor, float scaleFactor);
	virtual ~VREyeInfo() {}

	virtual VSMatrix GetProjection(float fov, float aspectRatio, float fovRatio, bool iso_ortho) const;
	virtual DVector3 GetViewShift(float yaw) const;
	virtual void SetUp() const { m_isActive = true; }
	virtual void TearDown() const { m_isActive = false; }
	virtual void AdjustHud() const {}
	virtual void AdjustBlend(HWDrawInfo* di) const {}
	bool isActive() const { return m_isActive; }

private:
	mutable bool m_isActive;
	float getShift() const;

};

struct VRMode
{
	int mEyeCount;
	float mHorizontalViewportScale;
	float mVerticalViewportScale;
	float mWeaponProjectionScale;
	VREyeInfo* mEyes[2];

	VRMode(int eyeCount, float horizontalViewportScale, 
		float verticalViewportScalem, float weaponProjectionScale, VREyeInfo eyes[2]);
	virtual ~VRMode() {}
	
	static const VRMode *GetVRMode(bool toscreen = true);
	virtual void AdjustViewport(DFrameBuffer *fb) const;
	VSMatrix GetHUDSpriteProjection() const;

	/* hooks for setup and cleanup operations for each stereo mode */
	virtual void SetUp() const {};
	virtual void TearDown() const {};

	virtual bool IsVR() const { return false; }
	virtual void AdjustPlayerSprites(HWDrawInfo *di) const {};
	virtual void UnAdjustPlayerSprites() const {};
	virtual void AdjustCrossHair() const {}
	virtual void UnAdjustCrossHair() const {}
	virtual void DrawControllerModels(HWDrawInfo* di, FRenderState& state) const {}
	
	virtual void Present() const { GLRenderer->PresentStereo(); };

	virtual bool GetHandTransform(int hand, VSMatrix* out) const { return false; }
	virtual bool GetWeaponTransform(VSMatrix* out) const { return false; }
	virtual bool RenderPlayerSpritesCrossed() const { return false; }
	virtual bool RenderPlayerSpritesInScene() const { return false; }
	virtual bool IsInitialized() const { return true; }
	virtual void Vibrate(float duration, int channel, float intensity) const { }
};
