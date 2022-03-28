#include <get_all_identifiers.hpp>
int main(int argc, char** argv){
        if(argc < 2){
            std::cerr<<"Syntax: ./grabidentifiers [srcML file name]"<<std::endl;
            return 0;
        }
        WordsFromArchivePolicy* cat = new WordsFromArchivePolicy(true);
        srcSAXController control(argv[1]);
        srcSAXEventDispatch::srcSAXEventDispatcher<> handler({cat}, false);
        control.parse(&handler); //Start parsing
}