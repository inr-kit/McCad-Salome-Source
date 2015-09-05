#ifndef MCCADPRINTUSAGE_H
#define MCCADPRINTUSAGE_H
#include <McCadConfig.h>

// Print usage message
//////////////////////////////
void printUsage(const TCollection_AsciiString&  theProg = "McCad") {
    cout << "=============================================\n" <<
            "		" << theProg.ToCString() <<  " " << MCCAD_VERSION_MAJOR << "." <<
                                                            MCCAD_VERSION_MINOR << "." <<
                                                            MCCAD_VERSION_PATCH << endl <<
            "=============================================\n\n" <<
            theProg.ToCString() << " is a CAD interface for Monte Carlo Transport Codes.\n\n" <<
            "Usage: " << theProg.ToCString() <<" [Options] [File | Path]\n\n" <<
            "Options:\n" <<
            "\t -d, --decompose :\n" <<
            "\t\t\t Decomposes the CAD input file, requires a STEP-FILE\n" <<
            "\t\t\t If no output file is defined, the output will be saved as\n" <<
            "\t\t\t converted'FILE'. \n" <<
            "\t -e, --explode: \n" <<
            "\t\t\t Calls the Exploder, requires a STEP-FILE\n" <<
            "\t\t\t Explodes all solids in a STEP file to distinct files.\n" <<
            "\t\t\t Does not accept an Output File.\n" <<
            "\t -f, --fuse : \n" <<
            "\t\t\t Calls the Fusioner, requires a DIRECTORY\n" <<
            "\t\t\t Fuses all STEP-FILES in the passed Directory into one single file.\n" <<
            "\t -m, --mcnp: \n" <<
            "\t\t\t Calls the VoidGenerator to generate void volumes,\n" <<
            "\t\t\t and prints the mcnp geometry into a file; requires an McCad_InputFile.\n" <<
            "\t\t\t See manual for more details on the InputFile.\n" <<
            "\t -s, --surface-check: \n" <<
            "\t\t\t Prints a list of all surface types of a given model into the output file.\n" <<
            "\t -t, --tripoli: \n" <<
            "\t\t\t Calls the VoidGenerator to generate void volumes,\n" <<
            "\t\t\t and prints the tripoli geometry into a file; requires an McCad_InputFile.\n" <<
            "\t\t\t See manual for more details on the InputFile.\n\n";
}

#endif // MCCADPRINTUSAGE_H
