#include <get_all_identifiers.hpp>
#include <iostream>
#include <popl.hpp>

int main(int argc, char** argv){
    using namespace popl;
    
    unsigned int sampleSize = 0;
    unsigned int randomSeed = 0;
    bool seedSet = false;

    OptionParser commandLineOptions("allowed options");
    auto helpOption = commandLineOptions.add<Switch>("h", "help", "show a help message");
    auto sampleOption = commandLineOptions.add<Value<unsigned int>>("s", "sample", "Specify identifier sample size to be returned by the tool.");
    auto seedOption = commandLineOptions.add<Value<unsigned int>>("r", "seed", "Specify the random seed for collecting a sample.");

    commandLineOptions.parse(argc, argv);
    
    if (helpOption->is_set() || argc < 2){
	    std::cout << commandLineOptions << std::endl;
        std::cout << "Typical usage:"<<std::endl<<"  ./grabidentifiers <options> <filename>"<<std::endl;
        return EXIT_FAILURE;
    }
    
    if(sampleOption->is_set()){
        sampleSize = sampleOption->value();   
    }

    if(seedOption->is_set()){
        randomSeed = seedOption->value();
        seedSet = true;
        if(!sampleOption->is_set()){
            std::cerr<<"You set a seed without specifying a sample. Please set a sample with -s."<<std::endl;
            return EXIT_FAILURE;
        }
    }

    if(commandLineOptions.non_option_args().size() == 1){
        std::string fileName = commandLineOptions.non_option_args().back();
        WordsFromArchivePolicy* cat = new WordsFromArchivePolicy(sampleSize, randomSeed, seedSet);
        srcSAXController control(fileName.c_str());
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat}, false);
        control.parse(&handler); //Start parsing
    }

    return EXIT_SUCCESS;
}