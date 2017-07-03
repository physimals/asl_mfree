/*   asl_functions.h various functions for the manipulation of ASL data

      Michael Chappell - IBME & FMIRB Image Analysis Group

      Copyright (C) 2010 University of Oxford */

/*   CCOPYRIGHT   */

#if !defined(asl_mfree_functions_h)
#define asl_mfree_functions_h

#include <string>
#include "newimage/newimageall.h"
#include "miscmaths/miscmaths.h"

using namespace MISCMATHS;
using namespace NEWIMAGE;

namespace OXASL {

  //output the result of deconvolution with an AIF
  void Deconv(const Matrix& data, const Matrix& aif, float dt,ColumnVector& mag, Matrix& resid);

  // prepare the AIFs
  void Prepare_AIF(volume4D<float>& aif, const volume<float>& metric, const volume<float>& mask, const float mthresh);
  // do SVD convoloution
  ReturnMatrix SVDdeconv(const Matrix& data, const Matrix& aif, float dt);
  // create a (simple) convolution matrix
  ReturnMatrix convmtx(const ColumnVector& invec);
  // create a cicular deconvolution
  ReturnMatrix SVDdeconv_circular(const Matrix& data, const Matrix& aif, float dt);
  ReturnMatrix convmtx_circular(const ColumnVector& invec);
  ReturnMatrix SVDdeconv_wu(const Matrix& data, const Matrix& aif, float dt);

  void Estimate_BAT_difference(const Matrix& resid, ColumnVector& batd, const float dt);
  void Correct_magnitude(ColumnVector& mag, const ColumnVector& batd, const float T1, const float dt, const float fa);

  void Estimate_onset(const Matrix& curves, ColumnVector& bate, const float dt);

  // estimation of magntiude precision
  void BootStrap(const Matrix& aif, const Matrix& data, float dt, const ColumnVector& mag, const Matrix& resid, int Nwb, ColumnVector& magsd);
}

#endif
