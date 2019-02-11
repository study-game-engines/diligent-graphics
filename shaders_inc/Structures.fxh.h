"// This file is derived from the open source project provided by Intel Corportaion that\n"
"// requires the following notice to be kept:\n"
"//--------------------------------------------------------------------------------------\n"
"// Copyright 2013 Intel Corporation\n"
"// All Rights Reserved\n"
"//\n"
"// Permission is granted to use, copy, distribute and prepare derivative works of this\n"
"// software for any purpose and without fee, provided, that the above copyright notice\n"
"// and this statement appear in all copies.  Intel makes no representations about the\n"
"// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED \"AS IS.\"\n"
"// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,\n"
"// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,\n"
"// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE\n"
"// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not\n"
"// assume any responsibility for any errors which may appear in this software nor any\n"
"// responsibility to update it.\n"
"//--------------------------------------------------------------------------------------\n"
"\n"
"#ifndef _STRCUTURES_FXH_\n"
"#define _STRCUTURES_FXH_\n"
"\n"
"#define PI 3.1415928f\n"
"\n"
"#ifdef __cplusplus\n"
"\n"
"#   ifndef BOOL\n"
"#      define BOOL int32_t // Do not use bool, because sizeof(bool)==1 !\n"
"#   endif\n"
"\n"
"#   ifndef TRUE\n"
"#      define TRUE 1\n"
"#   endif\n"
"\n"
"#   ifndef FALSE\n"
"#      define FALSE 0\n"
"#   endif\n"
"\n"
"#   ifndef CHECK_STRUCT_ALIGNMENT\n"
"#       define CHECK_STRUCT_ALIGNMENT(s) static_assert( sizeof(s) % 16 == 0, \"sizeof(\" #s \") is not multiple of 16\" );\n"
"#   endif\n"
"\n"
"#   ifndef DEFAULT_VALUE\n"
"#       define DEFAULT_VALUE(x) =x\n"
"#   endif\n"
"\n"
"#else\n"
"\n"
"#   ifndef BOOL\n"
"#       define BOOL bool\n"
"#   endif\n"
"\n"
"#   ifndef CHECK_STRUCT_ALIGNMENT\n"
"#       define CHECK_STRUCT_ALIGNMENT(s)\n"
"#   endif\n"
"\n"
"#   ifndef DEFAULT_VALUE\n"
"#       define DEFAULT_VALUE(x)\n"
"#   endif\n"
"\n"
"#endif\n"
"\n"
"\n"
"#define MAX_CASCADES 8\n"
"struct CascadeAttribs\n"
"{\n"
"	float4 f4LightSpaceScale;\n"
"	float4 f4LightSpaceScaledBias;\n"
"    float4 f4StartEndZ;\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(CascadeAttribs)\n"
"\n"
"\n"
"struct ShadowMapAttribs\n"
"{\n"
"    // 0\n"
"#ifdef __cplusplus\n"
"    float4x4 mWorldToLightViewT; // Matrices in HLSL are COLUMN-major while float4x4 is ROW major\n"
"#else\n"
"    matrix mWorldToLightView;  // Transform from view space to light projection space\n"
"#endif\n"
"    // 16\n"
"    CascadeAttribs Cascades[MAX_CASCADES];\n"
"\n"
"#ifdef __cplusplus\n"
"    float fCascadeCamSpaceZEnd[MAX_CASCADES];\n"
"    float4x4 mWorldToShadowMapUVDepthT[MAX_CASCADES];\n"
"#else\n"
"	float4 f4CascadeCamSpaceZEnd[MAX_CASCADES/4];\n"
"    matrix mWorldToShadowMapUVDepth[MAX_CASCADES];\n"
"#endif\n"
"\n"
"    // Do not use bool, because sizeof(bool)==1 !\n"
"	BOOL bVisualizeCascades;\n"
"\n"
"    // float3 f3Padding;\n"
"    // Stupid OpenGL compiler does not handle 3-component vectors properly\n"
"    // and screws up the structure layout.\n"
"    // Opengl.org suggests not using vec3 at all\n"
"    int Padding0, Padding1, Padding2;\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(ShadowMapAttribs)\n"
"\n"
"\n"
"struct LightAttribs\n"
"{\n"
"    float4 f4DirOnLight;\n"
"    float4 f4AmbientLight;\n"
"    float4 f4LightScreenPos;\n"
"    float4 f4ExtraterrestrialSunColor;\n"
"\n"
"    BOOL bIsLightOnScreen;\n"
"    // Stupid OpenGL compiler does not handle 3-component vectors properly\n"
"    // and screws up the structure layout.\n"
"    // Opengl.org suggests not using vec3 at all\n"
"    int Padding0, Padding1, Padding2;\n"
"\n"
"    ShadowMapAttribs ShadowAttribs;\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(LightAttribs)\n"
"\n"
"\n"
"struct CameraAttribs\n"
"{\n"
"    float4 f4CameraPos;            ///< Camera world position\n"
"    float fNearPlaneZ; \n"
"    float fFarPlaneZ; // fNearPlaneZ < fFarPlaneZ\n"
"    float2 f2Dummy;\n"
"\n"
"#ifdef __cplusplus\n"
"    float4x4 mViewProjT;\n"
"    //float4x4 mViewT;\n"
"    float4x4 mProjT;\n"
"    float4x4 mViewProjInvT;\n"
"#else\n"
"    matrix mViewProj;\n"
"    //matrix mView;\n"
"    matrix mProj;\n"
"    matrix mViewProjInv;\n"
"#endif\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(CameraAttribs)\n"
"\n"
"\n"
"#define LIGHT_SCTR_TECHNIQUE_EPIPOLAR_SAMPLING 0\n"
"#define LIGHT_SCTR_TECHNIQUE_BRUTE_FORCE 1\n"
"\n"
"#define CASCADE_PROCESSING_MODE_SINGLE_PASS 0\n"
"#define CASCADE_PROCESSING_MODE_MULTI_PASS 1\n"
"#define CASCADE_PROCESSING_MODE_MULTI_PASS_INST 2\n"
"\n"
"#define REFINEMENT_CRITERION_DEPTH_DIFF 0\n"
"#define REFINEMENT_CRITERION_INSCTR_DIFF 1\n"
"\n"
"// Extinction evaluation mode used when attenuating background\n"
"#define EXTINCTION_EVAL_MODE_PER_PIXEL 0// Evaluate extinction for each pixel using analytic formula \n"
"                                        // by Eric Bruneton\n"
"#define EXTINCTION_EVAL_MODE_EPIPOLAR 1 // Render extinction in epipolar space and perform\n"
"                                        // bilateral filtering in the same manner as for\n"
"                                        // inscattering\n"
"\n"
"#define SINGLE_SCTR_MODE_NONE 0\n"
"#define SINGLE_SCTR_MODE_INTEGRATION 1\n"
"#define SINGLE_SCTR_MODE_LUT 2\n"
"\n"
"#define MULTIPLE_SCTR_MODE_NONE 0\n"
"#define MULTIPLE_SCTR_MODE_UNOCCLUDED 1\n"
"#define MULTIPLE_SCTR_MODE_OCCLUDED 2\n"
"\n"
"#define TONE_MAPPING_MODE_EXP 0\n"
"#define TONE_MAPPING_MODE_REINHARD 1\n"
"#define TONE_MAPPING_MODE_REINHARD_MOD 2\n"
"#define TONE_MAPPING_MODE_UNCHARTED2 3\n"
"#define TONE_MAPPING_FILMIC_ALU 4\n"
"#define TONE_MAPPING_LOGARITHMIC 5\n"
"#define TONE_MAPPING_ADAPTIVE_LOG 6\n"
"\n"
"\n"
"struct PostProcessingAttribs\n"
"{\n"
"    uint m_uiNumEpipolarSlices              DEFAULT_VALUE(512);\n"
"    uint m_uiMaxSamplesInSlice              DEFAULT_VALUE(256);\n"
"    uint m_uiInitialSampleStepInSlice       DEFAULT_VALUE(16);\n"
"    // Note that sampling near the epipole is very cheap since only a few steps\n"
"    // required to perform ray marching\n"
"    uint m_uiEpipoleSamplingDensityFactor   DEFAULT_VALUE(2);\n"
"\n"
"    float m_fRefinementThreshold            DEFAULT_VALUE(0.03f);\n"
"    // do not use bool, because sizeof(bool)==1 and as a result bool variables\n"
"    // will be incorrectly mapped on GPU constant buffer\n"
"    BOOL m_bShowSampling                    DEFAULT_VALUE(FALSE); \n"
"    BOOL m_bCorrectScatteringAtDepthBreaks  DEFAULT_VALUE(FALSE); \n"
"    BOOL m_bShowDepthBreaks                 DEFAULT_VALUE(FALSE); \n"
"\n"
"    BOOL m_bShowLightingOnly                DEFAULT_VALUE(FALSE);\n"
"    BOOL m_bOptimizeSampleLocations         DEFAULT_VALUE(TRUE);\n"
"    BOOL m_bEnableLightShafts               DEFAULT_VALUE(TRUE);\n"
"    uint m_uiInstrIntegralSteps             DEFAULT_VALUE(30);\n"
"    \n"
"    float2 m_f2ShadowMapTexelSize           DEFAULT_VALUE(float2(0,0));\n"
"    uint m_uiShadowMapResolution            DEFAULT_VALUE(0);\n"
"    uint m_uiMinMaxShadowMapResolution      DEFAULT_VALUE(0);\n"
"\n"
"    BOOL m_bUse1DMinMaxTree                 DEFAULT_VALUE(TRUE);\n"
"    float m_fMaxShadowMapStep               DEFAULT_VALUE(16.f);\n"
"    float m_fMiddleGray                     DEFAULT_VALUE(0.18f);\n"
"    uint m_uiLightSctrTechnique             DEFAULT_VALUE(LIGHT_SCTR_TECHNIQUE_EPIPOLAR_SAMPLING);\n"
"\n"
"    int m_iNumCascades                      DEFAULT_VALUE(0);\n"
"    int m_iFirstCascade                     DEFAULT_VALUE(2);\n"
"    float m_fNumCascades                    DEFAULT_VALUE(0);\n"
"    float m_fFirstCascade                   DEFAULT_VALUE(1);\n"
"\n"
"    uint m_uiCascadeProcessingMode          DEFAULT_VALUE(CASCADE_PROCESSING_MODE_SINGLE_PASS);\n"
"    uint m_uiRefinementCriterion            DEFAULT_VALUE(REFINEMENT_CRITERION_INSCTR_DIFF);\n"
"    BOOL m_bIs32BitMinMaxMipMap             DEFAULT_VALUE(FALSE);\n"
"    uint m_uiMultipleScatteringMode         DEFAULT_VALUE(MULTIPLE_SCTR_MODE_UNOCCLUDED);\n"
"\n"
"    uint m_uiSingleScatteringMode           DEFAULT_VALUE(SINGLE_SCTR_MODE_INTEGRATION);\n"
"    BOOL m_bAutoExposure                    DEFAULT_VALUE(TRUE);\n"
"    uint m_uiToneMappingMode                DEFAULT_VALUE(TONE_MAPPING_MODE_UNCHARTED2);\n"
"    BOOL m_bLightAdaptation                 DEFAULT_VALUE(TRUE);\n"
"\n"
"    float m_fWhitePoint                     DEFAULT_VALUE(3.f);\n"
"    float m_fLuminanceSaturation            DEFAULT_VALUE(1.f);\n"
"    float2 f2Dummy;\n"
"    \n"
"    uint m_uiExtinctionEvalMode             DEFAULT_VALUE(EXTINCTION_EVAL_MODE_EPIPOLAR);\n"
"    BOOL m_bUseCustomSctrCoeffs             DEFAULT_VALUE(FALSE);\n"
"    float m_fAerosolDensityScale            DEFAULT_VALUE(1.f);\n"
"    float m_fAerosolAbsorbtionScale         DEFAULT_VALUE(0.1f);\n"
"\n"
"    float4 m_f4CustomRlghBeta               DEFAULT_VALUE(float4(5.8e-6f, 13.5e-6f, 33.1e-6f, 0.f));\n"
"    float4 m_f4CustomMieBeta                DEFAULT_VALUE(float4(2.e-5f, 2.e-5f, 2.e-5f, 0.f));\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(PostProcessingAttribs)\n"
"\n"
"\n"
"struct AirScatteringAttribs\n"
"{\n"
"    // Angular Rayleigh scattering coefficient contains all the terms exepting 1 + cos^2(Theta):\n"
"    // Pi^2 * (n^2-1)^2 / (2*N) * (6+3*Pn)/(6-7*Pn)\n"
"    float4 f4AngularRayleighSctrCoeff;\n"
"    // Total Rayleigh scattering coefficient is the integral of angular scattering coefficient in all directions\n"
"    // and is the following:\n"
"    // 8 * Pi^3 * (n^2-1)^2 / (3*N) * (6+3*Pn)/(6-7*Pn)\n"
"    float4 f4TotalRayleighSctrCoeff;\n"
"    float4 f4RayleighExtinctionCoeff;\n"
"\n"
"    // Note that angular scattering coefficient is essentially a phase function multiplied by the\n"
"    // total scattering coefficient\n"
"    float4 f4AngularMieSctrCoeff;\n"
"    float4 f4TotalMieSctrCoeff;\n"
"    float4 f4MieExtinctionCoeff;\n"
"\n"
"    float4 f4TotalExtinctionCoeff;\n"
"    // Cornette-Shanks phase function (see Nishita et al. 93) normalized to unity has the following form:\n"
"    // F(theta) = 1/(4*PI) * 3*(1-g^2) / (2*(2+g^2)) * (1+cos^2(theta)) / (1 + g^2 - 2g*cos(theta))^(3/2)\n"
"    float4 f4CS_g; // x == 3*(1-g^2) / (2*(2+g^2))\n"
"                   // y == 1 + g^2\n"
"                   // z == -2*g\n"
"\n"
"    float fEarthRadius              DEFAULT_VALUE(6360000.f);\n"
"    float fAtmTopHeight             DEFAULT_VALUE(80000.f);\n"
"    float2 f2ParticleScaleHeight    DEFAULT_VALUE(float2(7994.f, 1200.f));\n"
"    \n"
"    float fTurbidity                DEFAULT_VALUE(1.02f);\n"
"    float fAtmTopRadius             DEFAULT_VALUE(fEarthRadius + fAtmTopHeight);\n"
"    float m_fAerosolPhaseFuncG      DEFAULT_VALUE(0.76f);\n"
"    float m_fDummy;\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(AirScatteringAttribs)\n"
"\n"
"\n"
"struct MiscDynamicParams\n"
"{\n"
"    float fMaxStepsAlongRay;   // Maximum number of steps during ray tracing\n"
"    float fCascadeInd;\n"
"    float fElapsedTime;\n"
"    float fDummy;\n"
"\n"
"#ifdef __cplusplus\n"
"    uint ui4SrcMinMaxLevelXOffset;\n"
"    uint ui4SrcMinMaxLevelYOffset;\n"
"    uint ui4DstMinMaxLevelXOffset;\n"
"    uint ui4DstMinMaxLevelYOffset;\n"
"#else\n"
"    uint4 ui4SrcDstMinMaxLevelOffset;\n"
"#endif\n"
"};\n"
"CHECK_STRUCT_ALIGNMENT(MiscDynamicParams)\n"
"\n"
"#endif //_STRCUTURES_FXH_\n"
