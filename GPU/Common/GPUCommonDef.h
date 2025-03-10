//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUCommonDef.h
/// \author David Rohr

#ifndef GPUCOMMONDEF_H
#define GPUCOMMONDEF_H

// clang-format off

//Some GPU configuration settings, must be included first
#include "GPUCommonDefSettings.h"

#if (!defined(__OPENCL__) || defined(__OPENCLCPP__)) && (!(defined(__CINT__) || defined(__ROOTCINT__)) || defined(__CLING__)) && defined(__cplusplus) && __cplusplus >= 201103L
  #define GPUCA_NOCOMPAT // C++11 + No old ROOT5 + No old OpenCL
  #ifndef __OPENCL__
    #define GPUCA_NOCOMPAT_ALLOPENCL // + No OpenCL at all
  #endif
  #ifndef __CINT__
    #define GPUCA_NOCOMPAT_ALLCINT // + No ROOT CINT at all
  #endif
#endif

#if !(defined(__CINT__) || defined(__ROOTCINT__) || defined(__CLING__) || defined(__ROOTCLING__) || defined(G__ROOT)) //No GPU code for ROOT
  #if defined(__CUDACC__) || defined(__OPENCL__) || defined(__HIPCC__)
    #define GPUCA_GPUCODE //Compiled by GPU compiler
  #endif

  #if defined(__CUDA_ARCH__) || defined(__OPENCL__) || defined(__HIP_DEVICE_COMPILE__)
    #define GPUCA_GPUCODE_DEVICE //Executed on device
  #endif
#endif

//Definitions for C++11 features not supported by CINT / OpenCL
#ifdef GPUCA_NOCOMPAT
  #define CON_DELETE = delete
  #define CON_DEFAULT = default
  #define CONSTEXPR constexpr
#else
  #define CON_DELETE
  #define CON_DEFAULT
  #define CONSTEXPR const
#endif

//Set AliRoot / O2 namespace
#if defined(GPUCA_STANDALONE) || defined(GPUCA_O2_LIB) || defined(GPUCA_GPULIBRARY)
  #define GPUCA_ALIGPUCODE
#endif
#ifdef GPUCA_ALIROOT_LIB
  #define GPUCA_NAMESPACE AliGPU
#else
  #define GPUCA_NAMESPACE o2
#endif

//API Definitions for GPU Compilation
#include "GPUCommonDefAPI.h"

// clang-format on

#endif
