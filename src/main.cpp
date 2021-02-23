#include <easylogging++.h>
#include <iostream>

#include "incrementalscramble.h"
#include "cubestate.h"
#include "cube_moves.h"
#include "commutatorfinder.h"

INITIALIZE_EASYLOGGINGPP

static int showUsage(char* name) {
    std::cerr << "Usage: " << name << " output_path max_moves\n"
        << "\toutput_path: either /path/to/all_results.txt or /path/to/dir/\n"
        << "\tmax_moves: maximum number of partB moves in [A, B] commutators"
        << std::endl;
    return -1;
}

int main(int argc, char** argv) {
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %level %msg");
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    if (argc < 3 || std::string(argv[1]) == "-h")
        return showUsage(argv[0]);

    std::string outputPath(argv[1]);
    unsigned int maxMovesPartB = std::stoi(argv[2]);

    SearchCriteria criteriaAll(true, CenterSafety::SolvedCenterSafe);
    // we don't look for algs that solve a solved cube
    criteriaAll.set(CaseType::allSolved, false);
    CommutatorFinder cf(maxMovesPartB, criteriaAll, outputPath);
    cf.find();

    return 0;
}
