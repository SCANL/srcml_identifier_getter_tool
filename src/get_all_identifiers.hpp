#ifndef WORDSFROMARCHIVEPOLICY
#define WORDSFROMARCHIVEPOLICY

#include <ClassPolicy.hpp>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <map>

enum Context {ATTRIBUTE, CLASS, DECLARATION, FUNCTION, PARAMETER};
std::map<std::string, Context> stringToContextMap = {
    {"ATTRIBUTE", Context::ATTRIBUTE},
    {"CLASS", Context::CLASS},
    {"DECLARATION", Context::DECLARATION},
    {"FUNCTION", Context::FUNCTION},
    {"PARAMETER", Context::PARAMETER}
};

struct IdentifierData{
    IdentifierData(std::string type, std::string name, std::string context, std::string fileName, std::string programmingLanguageName, std::string lineNumber) 
    : type{type}, name{name}, context{context}, fileName{fileName}, programmingLanguageName{programmingLanguageName}, lineNumber{lineNumber} {}
    
    std::string type;
    std::string name;
    std::string context;
    std::string lineNumber;
    std::string fileName;
    std::string programmingLanguageName;

    friend std::ostream& operator<<(std::ostream& outputStream, const IdentifierData& identifier){
        outputStream<<identifier.type<<" "<<identifier.name<<" "<<identifier.context<<" "
                    <<identifier.lineNumber<<" "<<identifier.fileName<<" "<<identifier.programmingLanguageName;
        return outputStream;
    }
};

class WordsFromArchivePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        WordsFromArchivePolicy(unsigned int sampleSize, unsigned int randomSeed, bool isSeedSet, std::vector<Context> requiredContexts, 
                               std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}):
                               srcSAXEventDispatch::PolicyDispatcher(listeners), sampleSize{sampleSize}, randomSeed{randomSeed}, 
                               isSeedSet{isSeedSet}, requiredContexts{requiredContexts}{
            
            InitializeEventHandlers();
            declPolicy.AddListener(this);
            paramPolicy.AddListener(this);
            functionPolicy.AddListener(this);
        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
            if(typeid(DeclTypePolicy) == typeid(*policy)){
                declarationData = *policy->Data<DeclData>();
                if(!(declarationData.nameOfIdentifier.empty()||declarationData.nameOfType.empty())){
                    if(ctx.IsOpen(ParserState::function)){

                        CollectIdentifierTypeNameAndContext(declarationData.nameOfType, declarationData.nameOfIdentifier, "DECLARATION", ctx.currentLineNumber, 
                                                            ctx.currentFilePath, ctx.currentFileLanguage);

                    }else if(ctx.IsOpen(ParserState::classn) && !declarationData.nameOfContainingClass.empty() && !declarationData.nameOfType.empty() 
                             && !declarationData.nameOfIdentifier.empty()){
                        
                        CollectIdentifierTypeNameAndContext(declarationData.nameOfType, declarationData.nameOfIdentifier, "ATTRIBUTE", ctx.currentLineNumber, 
                                                            ctx.currentFilePath, ctx.currentFileLanguage);

                    }
                }
            }else if(typeid(ParamTypePolicy) == typeid(*policy)){
                parameterData = *policy->Data<DeclData>();
                if(!(parameterData.nameOfIdentifier.empty() || parameterData.nameOfType.empty())){

                    CollectIdentifierTypeNameAndContext(parameterData.nameOfType, parameterData.nameOfIdentifier, "PARAMETER", ctx.currentLineNumber, 
                                                        ctx.currentFilePath, ctx.currentFileLanguage);

                }
            }else if(typeid(FunctionSignaturePolicy) == typeid(*policy)){
                functionData = *policy->Data<SignatureData>();
                if(!(functionData.name.empty() || functionData.returnType.empty())){
                    if(functionData.hasAliasedReturn){
                        functionData.returnType+="*";
                    }

                    CollectIdentifierTypeNameAndContext(functionData.returnType, functionData.name, "FUNCTION", ctx.currentLineNumber, 
                                                        ctx.currentFilePath, ctx.currentFileLanguage);

                }
            }
        }
        
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx){}
        
        void CollectIdentifierTypeNameAndContext(std::string identifierType, std::string identifierName, std::string codeContext,
                                                 unsigned int lineNumber, std::string fileName, std::string programmingLanguageName){
            if(!sampleSize){
                std::cout<<identifierType<<" "<<identifierName<<" "<<codeContext<<" "<<lineNumber<<" "<<fileName<<" "<<programmingLanguageName<<std::endl;
            }else{
                allSystemIdentifiers.push_back(IdentifierData(identifierType, identifierName, codeContext, std::to_string(lineNumber), fileName, programmingLanguageName));        
            }
        }

        void GenerateSampleOfIdentifiers(int sampleSize){
            std::vector<IdentifierData> sampleOfIdentifiers;
            unsigned int random_seed = isSeedSet ? randomSeed : std::random_device{}();
            if(requiredContexts.size()){
                std::map<Context, std::vector<IdentifierData>> buckets;

                buckets[Context::PARAMETER] = std::vector<IdentifierData>();
                buckets[Context::ATTRIBUTE] = std::vector<IdentifierData>();
                buckets[Context::FUNCTION] = std::vector<IdentifierData>();
                buckets[Context::DECLARATION] = std::vector<IdentifierData>();
                buckets[Context::CLASS] = std::vector<IdentifierData>();
                
                for(auto identifier : allSystemIdentifiers){
                    for(auto contextId : requiredContexts){
                        if(stringToContextMap[identifier.context] == contextId){
                            buckets[contextId].push_back(identifier);
                        }
                    }
                }
                for(auto bucket : buckets){
                    std::cout<<bucket.second.size()<<std::endl;
                }

            }else{
                std::sample(allSystemIdentifiers.begin(), allSystemIdentifiers.end(), std::back_inserter(sampleOfIdentifiers), 
                            sampleSize, std::mt19937{random_seed});
                for(auto identifier : sampleOfIdentifiers){
                    std::cout<<identifier<<std::endl;
                }
            }

            std::cout<<"YOUR RANDOM SEED IS: "<<random_seed<<". Please SAVE IT."<<std::endl;
        }
    protected:
        void *DataInner() const override {
            return (void*)0; // export profile to listeners
        }
        
    private:
        unsigned int sampleSize;
        unsigned int randomSeed;
        bool isSeedSet;
        std::vector<Context> requiredContexts;
        std::vector<IdentifierData> allSystemIdentifiers;
        DeclTypePolicy declPolicy;
        DeclData declarationData;

        ParamTypePolicy paramPolicy;
        DeclData parameterData;

        FunctionSignaturePolicy functionPolicy;
        SignatureData functionData;

        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            
            //Open events
            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&declPolicy);
            };
            openEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&paramPolicy);
            };
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&functionPolicy);
            };

            //Close events
            closeEventMap[ParserState::classn] = [this](srcSAXEventContext& ctx){
                if(!ctx.currentClassName.empty()){

                    CollectIdentifierTypeNameAndContext("class", ctx.currentClassName, "CLASS", ctx.currentLineNumber, 
                                                        ctx.currentFilePath, ctx.currentFileLanguage);
                }
            };
            closeEventMap[ParserState::functionblock] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&declPolicy);
            };
            closeEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->RemoveListenerDispatch(&paramPolicy);
            };
            closeEventMap[ParserState::archive] = [this](srcSAXEventContext& ctx) {
                GenerateSampleOfIdentifiers(sampleSize);
            };
        }
};
#endif