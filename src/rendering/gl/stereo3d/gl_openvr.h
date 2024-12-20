//
//---------------------------------------------------------------------------
//
// Copyright(C) 2016-2017 Christopher Bruns
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//
/*
** gl_openvr.h
** Stereoscopic virtual reality mode for the HTC Vive headset
*/

#ifndef GL_OPENVR_H_
#define GL_OPENVR_H_

#include "r_utility.h" // viewpitch
#include "hwrenderer/data/hw_vrmodes.h"

namespace openvr {
	// forward declarations
	struct TrackedDevicePose_t;
	struct Texture_t;
	struct VR_IVRSystem_FnTable;
	struct VR_IVRCompositor_FnTable;
	struct VR_IVRRenderModels_FnTable;
}

#define PITCH 0
#define YAW 1
#define ROLL 2

typedef float vec_t;
typedef vec_t vec3_t[3];

/* stereoscopic 3D API */
namespace s3d {

class OpenVREyePose : public VREyeInfo
{
public:
	friend class OpenVRMode;

	OpenVREyePose(int eye, float shiftFactor, float scaleFactor);
	virtual ~OpenVREyePose() override;
	virtual VSMatrix GetProjection(FLOATTYPE fov, FLOATTYPE aspectRatio, FLOATTYPE fovRatio, bool iso_ortho) const override;
	DVector3 GetViewShift(FLOATTYPE yaw) const override;
	virtual void AdjustHud() const override;
	virtual void AdjustBlend(HWDrawInfo* di) const override;

	void initialize(openvr::VR_IVRSystem_FnTable * vrsystem);
	void dispose();
	void setCurrentHmdPose(const openvr::TrackedDevicePose_t * pose) const {currentPose = pose;}
	bool submitFrame(openvr::VR_IVRCompositor_FnTable * vrCompositor) const;

protected:
	VSMatrix projectionMatrix;
	VSMatrix eyeToHeadTransform;
	VSMatrix otherEyeToHeadTransform;
	openvr::Texture_t* eyeTexture;
	mutable uint32_t framebuffer;
	int eye;

	mutable const openvr::TrackedDevicePose_t * currentPose;
	VSMatrix getHUDProjection() const;
};

class OpenVRHaptics
{

public:

	OpenVRHaptics(openvr::VR_IVRSystem_FnTable* vrSystem);

	void Vibrate(float duration, int channel, float intensity);
	void ProcessHaptics();

private:
	openvr::VR_IVRSystem_FnTable* vrSystem;
	uint32_t controllerIDs[2];
	//0 = left, 1 = right
	float vibration_channel_duration[2] = { 0.0f, 0.0f };
	float vibration_channel_intensity[2] = { 0.0f, 0.0f };

};

class OpenVRMode : public VRMode
{
public:
	friend class OpenVREyePose;
	//static const VRMode& getInstance(); // Might return Mono mode, if no HMD available

	OpenVRMode(OpenVREyePose eyes[2]);
	virtual ~OpenVRMode() override;
	virtual void SetUp() const override; // called immediately before rendering a scene frame
	virtual void TearDown() const override; // called immediately after rendering a scene frame
	virtual bool IsVR() const override { return true; }
	virtual void Present() const override;
	virtual void AdjustViewport(DFrameBuffer* screen) const override;
	virtual void AdjustPlayerSprites(HWDrawInfo* di) const override;
	virtual void UnAdjustPlayerSprites() const override;
	virtual void AdjustCrossHair() const override;
	virtual void UnAdjustCrossHair() const override;

	virtual void DrawControllerModels(HWDrawInfo* di, FRenderState& state) const override;
	
	virtual bool GetHandTransform(int hand, VSMatrix* out) const override;
	virtual bool GetWeaponTransform(VSMatrix* out) const override;
	virtual bool RenderPlayerSpritesCrossed() const { return true; }
	virtual bool RenderPlayerSpritesInScene() const { return true; }
	virtual bool IsInitialized() const { return hmdWasFound; }

	virtual void Vibrate(float duration, int channel, float intensity) const
	{ 
		haptics->Vibrate(duration, channel, intensity);
	}

protected:
	OpenVRMode();
	// void updateDoomViewDirection() const;
	void updateHmdPose(FRenderViewpoint& vp, double hmdYawRadians, double hmdPitchRadians, double hmdRollRadians) const;

	OpenVREyePose* leftEyeView;
	OpenVREyePose* rightEyeView;
	OpenVRHaptics* haptics;

	openvr::VR_IVRSystem_FnTable * vrSystem;
	openvr::VR_IVRCompositor_FnTable * vrCompositor;
	openvr::VR_IVRRenderModels_FnTable * vrRenderModels;
	uint32_t vrToken;

	mutable int cachedScreenBlocks;
	mutable double hmdYaw; // cached latest value in radians
	mutable int cachedViewwidth, cachedViewheight, cachedViewwindowx, cachedViewwindowy;
	mutable F2DDrawer * crossHairDrawer;

private:
	typedef VRMode super;
	bool hmdWasFound;
	uint32_t sceneWidth, sceneHeight;
};

} /* namespace st3d */


#endif /* GL_OPENVR_H_ */
