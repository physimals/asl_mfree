/*  readoptions.h

    Michael Chappell - FMRIB Image Analysis Group

    Copyright (C) 2009 University of Oxford  */

/*  CCOPYRIGHT  */

#if !defined(ReadOptions_h)
#define ReadOptions_h

#include "utils/log.h"
#include "utils/options.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace Utilities;

namespace OXASL
{
class ReadOptions
{
public:
    static ReadOptions &getInstance();
    ~ReadOptions() { delete ropt; }
    Option<bool> help;

    Option<string> datafile;
    Option<string> maskfile;
    Option<string> outname;
    Option<string> aif;
    Option<float> dt;

    Option<string> metric;
    Option<float> mthresh;

    // timing correction
    Option<bool> tcorrect;
    Option<string> bata;
    Option<string> batt;
    Option<bool> batout;
    Option<float> bat_gradient_threshold;
    Option<float> T1;
    Option<float> fa;

    // std deviation estimation
    Option<bool> std;
    Option<int> nwb;

    // parameters specific to Turbo QUASAR
    Option<bool> turbo_quasar; // specify this is Turbo QUASAR
    Option<int> shift_factor;  // Slice shifting factor <usually a factor of 2>

    void parse_command_line(int argc, char **argv);

private:
    ReadOptions();
    const ReadOptions &operator=(ReadOptions &);
    ReadOptions(ReadOptions &);

    OptionParser options;

    static ReadOptions *ropt;
};

inline ReadOptions &ReadOptions::getInstance()
{
    if (ropt == NULL)
        ropt = new ReadOptions();

    return *ropt;
}

inline ReadOptions::ReadOptions()
    :

    help(string("-h,--help"), false,
        string("display this message"),
        false, no_argument)
    ,
    //input files
    datafile(string("--data,--datafile"), string("data"),
        string("ASL data file"),
        true, requires_argument)
    , maskfile(string("--mask"), string("maskfile"),
          string("mask"),
          true, requires_argument)
    , outname(string("--out"), string("asl_mfree"),
          string("Output directory name"),
          true, requires_argument)
    , aif(string("--aif"), string(""),
          string("Arterial input functions for deconvolution (4D volume, one aif for each voxel within mask)"),
          true, requires_argument)
    , dt(string("--dt"), 1.0,
          string("Temporal spacing in data (s)\n"),
          true, requires_argument)
    ,

    metric(string("--metric"), string(""),
        string("Metric image"),
        false, requires_argument)
    , mthresh(string("--mthresh"), 0.0,
          string("Metric threshold\n"),
          false, requires_argument)
    ,

    tcorrect(string("--tcorrect"), false,
        string("Apply correction for timing difference between AIF and tissue curve"),
        false, no_argument)
    , bata(string("--bata"), string(""),
          string("arterial BAT image"),
          false, requires_argument)
    , batt(string("--batt"), string(""),
          string("tissue BAT image"),
          false, requires_argument)
    , batout(string("--bat"), false,
          string("Estimate tissue BAT from data (and save this image)"),
          false, no_argument)
    , bat_gradient_threshold(string("--bat_grad_thr"), 0.2,
          string("Edge detection gradient threshold (default: 0.2)"),
          false, no_argument)
    , T1(string("--t1"), 1.6,
          string("T1 (of blood) value"),
          false, requires_argument)
    , fa(string("--fa"), 0.0,
          string("Flip angle (if using LL readout)"),
          false, requires_argument)
    ,

    std(string("--std"), false,
        string("Calculate standard deviations on perfusion values using wild bootstrapping"),
        false, no_argument)
    , nwb(string("--nwb"), 1000,
          string("Number of permuatations for wild bootstrapping \n"),
          false, requires_argument)
    ,

    // Turbo QUASAR parameters
    turbo_quasar(string("--turbo_quasar"), false,
        string("Specify this is a Turbo QUASAR Sequence"),
        false, no_argument)
    , shift_factor(string("--shift_factor"), 1,
          string("Slice shifting factor in Turbo QUASAR <default value: 1> \n"),
          false, requires_argument)
    ,

    options("asl_mfree", "asl_mfree --verbose\n")
{
    try
    {
        options.add(help);

        options.add(datafile);
        options.add(maskfile);
        options.add(outname);
        options.add(aif);
        options.add(dt);

        options.add(metric);
        options.add(mthresh);

        options.add(tcorrect);
        options.add(bata);
        options.add(batt);
        options.add(batout);
        options.add(bat_gradient_threshold);
        options.add(T1);
        options.add(fa);

        options.add(std);
        options.add(nwb);

        options.add(turbo_quasar);
        options.add(shift_factor);
    }
    catch (X_OptionError &e)
    {
        options.usage();
        cerr << endl
             << e.what() << endl;
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
    }
}
}
#endif
