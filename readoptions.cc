/*  readoptions.cc

    Michael Chappell - FMRIB Image Analysis Group

    Copyright (C) 2009 University of Oxford  */

/*  CCOPYRIGHT  */


#include "readoptions.h"
#include "utils/log.h"
#include "utils/tracer_plus.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace Utilities;

namespace OXASL
{
ReadOptions *ReadOptions::ropt = NULL;

void ReadOptions::parse_command_line(int argc, char **argv)
{
    Tracer_Plus("ReadOptions::parse_command_line");

    // do once to establish log directory name
    for (int a = options.parse_command_line(argc, argv); a < argc; a++)
        ;

    // setup logger directory
    //logger.makeDir(logdir.value());

    // cout << "Log directory is: " << logger.getDir() << endl;

    // do again so that options are logged
    //for(int a = 0; a < argc; a++)
    //  logger.str() << argv[a] << " ";
    //logger.str() << endl << "---------------------------------------------" << endl << endl;

    if (help.value() || !options.check_compulsory_arguments())
    {
        options.usage();
        throw std::runtime_error("Not all of the compulsory arguments have been provided");
    }
}
}
