#include <get_all_identifiers.hpp>
#include <iostream>
#include <popl.hpp>

int main(int argc, char** argv){
    using namespace popl;
    
    unsigned int sampleSize = 0;

    OptionParser commandLineOptions("allowed options");
    auto helpOption = commandLineOptions.add<Switch>("h", "help", "show a help message");
    auto sampleOption = commandLineOptions.add<Value<unsigned int>>("s", "sample", "Specify identifier sample size to be returned by the tool.");

    commandLineOptions.parse(argc, argv);
    
    if (helpOption->is_set() || argc < 2){
	    std::cout << commandLineOptions << std::endl;
        std::cout << "Typical usage:"<<std::endl<<"  ./grabidentifiers <options> <filename>"<<std::endl;
        return EXIT_SUCCESS;
    }
    
    if(sampleOption->is_set()){
        sampleSize = sampleOption->value();   
    }

    if(commandLineOptions.non_option_args().size() == 1){
        std::string fileName = commandLineOptions.non_option_args().back();
        WordsFromArchivePolicy* cat = new WordsFromArchivePolicy(sampleSize);
        srcSAXController control(fileName.c_str());
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat}, false);
        control.parse(&handler); //Start parsing
    }

    return EXIT_SUCCESS;
}