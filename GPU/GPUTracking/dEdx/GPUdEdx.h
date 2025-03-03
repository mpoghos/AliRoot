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

/// \file GPUdEdx.h
/// \author David Rohr

#ifndef GPUDEDX_H
#define GPUDEDX_H

#include "GPUDef.h"
#include "GPUTPCGeometry.h"
#include "GPUCommonMath.h"
#include "GPUParam.h"
#include "GPUdEdxInfo.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
#ifndef HAVE_O2HEADERS

class GPUdEdx
{
 public:
  GPUd() void clear() {}
  GPUd() void fillCluster(float qtot, float qmax, int padRow, float trackSnp, float trackTgl, const GPUParam& param) {}
  GPUd() void fillSubThreshold(int padRow, const GPUParam& param) {}
  GPUd() void computedEdx(GPUdEdxInfo& output, const GPUParam& param) {}
};

#else

class GPUdEdx
{
 public:
  // The driver must call clear(), fill clusters row by row outside-in, then run computedEdx() to get the result
  GPUd() void clear();
  GPUd() void fillCluster(float qtot, float qmax, int padRow, float trackSnp, float trackTgl, const GPUParam& param);
  GPUd() void fillSubThreshold(int padRow, const GPUParam& param);
  GPUd() void computedEdx(GPUdEdxInfo& output, const GPUParam& param);

 private:
  GPUd() float GetSortTruncMean(float* array, int count, int trunclow, int trunchigh);
  GPUd() void checkSubThresh(int roc);

  static constexpr int MAX_NCL = GPUCA_ROW_COUNT; // Must fit in mNClsROC (unsigned char)!

  float mChargeTot[MAX_NCL]; // No need for default, just some memory
  float mChargeMax[MAX_NCL]; // No need for default, just some memory
  float mSubThreshMinTot = 0.f;
  float mSubThreshMinMax = 0.f;
  unsigned char mNClsROC[4] = {0};
  unsigned char mNClsROCSubThresh[4] = {0};
  unsigned char mCount = 0;
  unsigned char mLastROC = 255;
  char mNSubThresh = 0;
};

GPUdi() void GPUdEdx::checkSubThresh(int roc)
{
  if (roc != mLastROC) {
    if (mNSubThresh && mCount + mNSubThresh <= MAX_NCL) {
      for (int i = 0; i < mNSubThresh; i++) {
        mChargeTot[mCount] = mSubThreshMinTot;
        mChargeMax[mCount++] = mSubThreshMinMax;
      }
      mNClsROC[mLastROC] += mNSubThresh;
      mNClsROCSubThresh[mLastROC] += mNSubThresh;
    }
    mNSubThresh = 0;
    mSubThreshMinTot = 1e10f;
    mSubThreshMinMax = 1e10f;
  }

  mLastROC = roc;
}

GPUdi() void GPUdEdx::fillCluster(float qtot, float qmax, int padRow, float trackSnp, float trackTgl, const GPUParam& param)
{
  if (mCount >= MAX_NCL) {
    return;
  }
  const int roc = param.tpcGeometry.GetROC(padRow);
  checkSubThresh(roc);
  float snp2 = trackSnp * trackSnp;
  if (snp2 > GPUCA_MAX_SIN_PHI_LOW) {
    snp2 = GPUCA_MAX_SIN_PHI_LOW;
  }
  float factor = CAMath::Sqrt((1 - snp2) / (1 + trackTgl * trackTgl));
  factor /= param.tpcGeometry.PadHeight(padRow);
  qtot *= factor;
  qmax *= factor / param.tpcGeometry.PadWidth(padRow);

  mChargeTot[mCount] = qtot;
  mChargeMax[mCount++] = qmax;
  mNClsROC[roc]++;
  if (qtot < mSubThreshMinTot) {
    mSubThreshMinTot = qtot;
  }
  if (qmax < mSubThreshMinMax) {
    mSubThreshMinMax = qmax;
  }
}

GPUdi() void GPUdEdx::fillSubThreshold(int padRow, const GPUParam& param)
{
  const int roc = param.tpcGeometry.GetROC(padRow);
  checkSubThresh(roc);
  mNSubThresh++;
}

#endif // !HAVE_O2HEADERS
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
