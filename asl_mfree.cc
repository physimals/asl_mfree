/*   asl_mfree.cc 'Model-free' deconvolution for multi-TI ASL data

      Michael Chappell - IBME & FMIRB Image Analysis Group

      Copyright (C) 2011 University of Oxford */

/*   CCOPYRIGHT   */

#
include<iostream> #include<math.h> #include<string> #include
    "newmatap.h" #include "newmatio.h" #include
    "newimage/newimageall.h" #include "miscmaths/miscmaths.h" #include
    "utils/tracer_plus.h" #include "stdlib.h"

#
    include "readoptions.h" #include "asl_mfree_functions.h"

    using namespace Utilities;
using namespace NEWMAT;
using namespace NEWIMAGE;
using namespace MISCMATHS;

using namespace OXASL;

int main(int argc, char* argv[]) {
  try {
    // cout << "ASL_MFREE (1.0)" << endl;
    cout << "ASL_MFREE (1.1)" << endl;

    // parse command line (puts these into the log file)
    ReadOptions& opts = ReadOptions::getInstance();
    opts.parse_command_line(argc, argv);

    cout << "Loading data" << endl;

    // load data
    volume4D<float> data;
    read_volume4D(data, opts.datafile.value());

    // load mask
    volume<float> mask(data.xsize(), data.ysize(), data.zsize());
    mask.setdims(data.xdim(), data.ydim(), data.zdim());
    read_volume(mask, opts.maskfile.value());

    Matrix asldata;
    asldata = data.matrix(mask);
    // data.setmatrix(asldata,mask);
    int nvox = asldata.Ncols();

    // load aif
    volume4D<float> aif;
    read_volume4D(aif, opts.aif.value());

    // select AIF based on metric
    // load metric image if it exists
    volume<float> metric;
    if (opts.metric.set()) {
      cout << "Preparing AIFs" << endl;
      read_volume(metric, opts.metric.value());

      Prepare_AIF(aif, metric, mask, opts.mthresh.value());

      // volume4D<float> aifout;
      // aifout.setmatrix(aif,mask);
      save_volume4D(aif, opts.outname.value() + "_aifs");
    }

    Matrix aifmtx;
    aifmtx = aif.matrix(mask);

    // Here we need to specifiy the exact TIs in the sequence
    // By default, it is the dt value in the input
    float ti_actual = opts.dt.value();
    // For Turbo QUASAR, we need to consider the slice shifting effects
    if (opts.turbo_quasar.set()) {
      cout << "This is Turbo QUASAR Sequence!!!" << endl;
      cout << "The current ASL_model_free function only supports consecutive "
              "bolus, i.e. no bolus skipping!!!"
           << endl;
      int shift_factor = opts.shift_factor.value();
      ti_actual = ti_actual / shift_factor;
    }

    // do deconvolution
    cout << "Performing deconvolution" << endl;
    ColumnVector mag;
    Matrix resid;
    Deconv(asldata, aifmtx, ti_actual, mag, resid);

    // estimate BAT (of tissue)
    ColumnVector batt;
    if (opts.batout.set() | (opts.tcorrect.set() & !opts.batt.set())) {
      cout << "Estimating BAT" << endl;
      Estimate_onset(asldata, batt, ti_actual);

      if (opts.batout.set()) {
        // output the BAT image (from the tissue)
        volume4D<float> batout;
        batout.setmatrix(batt.AsMatrix(1, nvox), mask);
        save_volume4D(batout, opts.outname.value() + "_bat");
      }
    }

    // correct aif magntiude for differences in arrival time between aif and
    // tissue
    ColumnVector batd;
    if (opts.tcorrect.set()) {
      cout << "Performing timing correction" << endl;

      if (opts.bata.set()) {
        // calculate difference between tissue and AIF curves using suppled BAT
        // images
        volume4D<float> bat_art;
        read_volume4D(bat_art, opts.bata.value());

        if (opts.batt.set()) {
          // load supplied tissue BAT
          volume4D<float> bat_tiss;
          read_volume4D(bat_tiss, opts.batt.value());

          batt = (bat_tiss.matrix(mask)).AsColumn();
        }

        if (opts.metric.set()) {
          // correct the AIF bat image to match the AIFs where a metric image
          // has been supplied
          Prepare_AIF(bat_art, metric, mask, opts.mthresh.value());
        }

        ColumnVector bata;
        bata = (bat_art.matrix(mask)).AsColumn();

        batd = batt - bata;
      } else {
        // otherwise estimate BAT difference using the peak in the residue
        // function
        // Estimate_BAT_difference(resid,batd,ti_actual);

        // Estiamte BAT difference using edge detection
        ColumnVector bata;
        Estimate_onset(aifmtx, bata, ti_actual);

        batd = batt - bata;
      }

      for (int i = 1; i <= batd.Nrows(); i++) {
        if (batd(i) < 0.0) batd(i) = 0.0;
      }
      Correct_magnitude(mag, batd, opts.T1.value(), ti_actual, opts.fa.value());
    }

    if (opts.std.set()) {
      // do wild boostrapping std dev estimation for cbf
      cout << "Performing wild bootstrapping for precision estimation" << endl;
      ColumnVector magstd;
      BootStrap(aifmtx, asldata, ti_actual, mag, resid, opts.nwb.value(),
                magstd);

      if (opts.tcorrect.set()) {
        // if needed we should correct the std dev for timing discrpancies
        // between aif and ctc
        Correct_magnitude(magstd, batd, opts.T1.value(), ti_actual,
                          opts.fa.value());
      }

      // save it
      volume4D<float> stdout;
      stdout.setmatrix(magstd.AsMatrix(1, nvox), mask);
      save_volume4D(stdout, opts.outname.value() + "_stddev");
    }

    cout << "Saving results" << endl;
    // output
    volume4D<float> residout;
    residout.setmatrix(resid, mask);
    save_volume4D(residout, opts.outname.value() + "_residuals");

    volume4D<float> magout;
    magout.setmatrix(mag.AsMatrix(1, nvox), mask);
    save_volume4D(magout, opts.outname.value() + "_magntiude");

    cout << "ASL_MFREE - Done!" << endl;
  } catch (Exception& e) {
    cerr << endl << e.what() << endl;
    return 1;
  } catch (X_OptionError& e) {
    cerr << endl << e.what() << endl;
    return 1;
  }

  cout << "Done." << endl;

  return 0;
}