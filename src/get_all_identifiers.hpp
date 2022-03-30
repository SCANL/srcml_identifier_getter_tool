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

struct IdentifierData{
    IdentifierData(std::string type, std::string name, std::string context, std::string fileName, std::string programmingLanguage, std::string lineNumber) 
    : type{type}, name{name}, context{context}, fileName{fileName}, programmingLanguage{programmingLanguage}, lineNumber{lineNumber} {}
    
    std::string type;
    std::string name;
    std::string context;
    std::string lineNumber;
    std::string fileName;
    std::string programmingLanguage;

    void PrintIdentifier(){
        std::cout<<type<<" "<<name<<" "<<context<<" "<<lineNumber<<" "<<fileName<<" "<<programmingLanguage<<std::endl;
    }
};

class WordsFromArchivePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    public:
        WordsFromArchivePolicy(unsigned int sampleSize, std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) 
        : srcSAXEventDispatch::PolicyDispatcher(listeners), sampleSize{sampleSize}{
            
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

                    CollectIdentifierTypeNameAndContext(functionData.returnType, functionData.name, "FUNCTION NAME", ctx.currentLineNumber, 
                                                        ctx.currentFilePath, ctx.currentFileLanguage);

                    for(auto param : functionData.parameters){

                        CollectIdentifierTypeNameAndContext(functionData.returnType, param.nameOfIdentifier, "FUNCTION PARAMETER", ctx.currentLineNumber, 
                                                            ctx.currentFilePath, ctx.currentFileLanguage);

                    }
                }
            }
        }
        
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx){}
        
        void CollectIdentifierTypeNameAndContext(std::string identifierType, std::string identifierName, std::string codeContext,
                                                 unsigned int lineNumber, std::string fileName, std::string programmingLanguage){
            if(!sampleSize){
                std::cout<<identifierType<<" "<<identifierName<<" "<<codeContext<<" "<<lineNumber<<" "<<fileName<<" "<<programmingLanguage<<std::endl;
            }else{
                allSystemIdentifiers.push_back(IdentifierData(identifierType, identifierName, codeContext, std::to_string(lineNumber), fileName, programmingLanguage));        
            }
        }

        void GenerateSampleOfIdentifiers(int sampleSize){
            std::vector<IdentifierData> sampleOfIdentifiers;
            unsigned int random_seed = std::random_device{}();
            std::sample(allSystemIdentifiers.begin(), allSystemIdentifiers.end(), std::back_inserter(sampleOfIdentifiers), 
                        sampleSize, std::mt19937{random_seed});
            for(auto identifier : sampleOfIdentifiers){
               identifier.PrintIdentifier();
            }
            std::cout<<"YOUR RANDOM SEED IS: "<<random_seed<<" Please SAVE IT."<<std::endl;
        }
    protected:
        void *DataInner() const override {
            return (void*)0; // export profile to listeners
        }
        
    private:
        unsigned int sampleSize;
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