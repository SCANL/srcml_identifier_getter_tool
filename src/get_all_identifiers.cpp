#include <get_all_identifiers.hpp>
#include <iostream>
#include <popl.hpp>
#include <cstring>

int main(int argc, char** argv){
    using namespace popl;
    
    unsigned int sampleSize = 0;
    unsigned int randomSeed = 0;
    bool isSeedSet = false;

    OptionParser commandLineOptions("allowed options");
    auto helpOption = commandLineOptions.add<Switch>("h", "help", "show a help message");
    auto sampleOption = commandLineOptions.add<Value<unsigned int>>("s", "sample", "Specify identifier sample size to be returned by the tool.");
    auto seedOption = commandLineOptions.add<Value<unsigned int>>("r", "seed", "Specify the random seed for collecting a sample.");
    auto contextOptions = commandLineOptions.add<Value<std::string>>("c", "contexts", "Specify which contexts you want identifiers from." 
                                                                          "This can be PARAMTER, FUNCTION, DECLARATION, PARAMETER, or CLASS."
                                                                          "You can specify these in any combination.");

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
        isSeedSet = true;
        if(!sampleOption->is_set()){
            std::cerr<<"You set a seed without specifying a sample. Please set a sample with -s."<<std::endl;
            return EXIT_FAILURE;
        }
    }

    std::vector<Context> requiredContexts;
    if(contextOptions->is_set()){
        //Need to split the list. First make a copy since c_str() is const.
        char delimityContextOptions[contextOptions->value().size()];
        std::strcpy(delimityContextOptions, contextOptions->value().c_str());
        
        //string tok to split
        char* contextOption = std::strtok(delimityContextOptions, ",");

        bool isAllContextInputValid = true;
        while(contextOption){
            std::string contextOptionString = std::string(contextOption);
            
            //Validate input
            auto isContextValid = stringToContextMap.find(contextOptionString);
            if(isContextValid == stringToContextMap.end()){
                std::cerr<<"Invalid identifier type: "<<contextOptionString<<std::endl;
                std::cerr<<"Valid types are: "<<std::endl
                         <<"ATTRIBUTE"<<std::endl
                         <<"CLASS"<<std::endl
                         <<"DECLARATION"<<std::endl
                         <<"FUNCTION"<<std::endl
                         <<"PARAMETER"<<std::endl;
                isAllContextInputValid = false;
            }else{
                requiredContexts.push_back(isContextValid->second);
            }
            //Get the next token
            contextOption = strtok(nullptr, ",");
        }

        //invalid input, exit failure
        if(!isAllContextInputValid) return EXIT_FAILURE;
    }

    if(commandLineOptions.non_option_args().size() == 1){
        std::string fileName = commandLineOptions.non_option_args().back();
        WordsFromArchivePolicy* cat = new WordsFromArchivePolicy(sampleSize, randomSeed, isSeedSet, requiredContexts);
        srcSAXController control(fileName.c_str());
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat}, false);
        control.parse(&handler); //Start parsing
    }

    return EXIT_SUCCESS;
}