#ifndef WORDSFROMARCHIVEPOLICY
#define WORDSFROMARCHIVEPOLICY

#include <ClassPolicy.hpp>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>
#include <vector>

struct IdentifierData{
    std::string type;
    std::string name;
    std::string context;
};

class WordsFromArchivePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    private:
        void CollectIdentifierTypeNameAndContext(std::string identifierType, std::string identifierName, std::string codeContext){
            if(!shouldTakeSample){
                std::cout<<identifierType<<" "<<identifierName<<" "<<codeContext<<std::endl;
            }else{
                
            }
        }
    public:
        WordsFromArchivePolicy(bool shouldTakeSample, std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){
            InitializeEventHandlers();
            this->shouldTakeSample = shouldTakeSample;
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
                        CollectIdentifierTypeNameAndContext(declarationData.nameOfType, declarationData.nameOfIdentifier, "DECLARATION");
                    }else if(ctx.IsOpen(ParserState::classn) && !declarationData.nameOfContainingClass.empty() && !declarationData.nameOfType.empty() && !declarationData.nameOfIdentifier.empty()){
                        CollectIdentifierTypeNameAndContext(declarationData.nameOfType, declarationData.nameOfIdentifier, "ATTRIBUTE");
                    }
                }
            }else if(typeid(ParamTypePolicy) == typeid(*policy)){
                parameterData = *policy->Data<DeclData>();
                if(!(parameterData.nameOfIdentifier.empty() || parameterData.nameOfType.empty())){
                    CollectIdentifierTypeNameAndContext(parameterData.nameOfType, parameterData.nameOfIdentifier, "PARAMETER");
                }
            }else if(typeid(FunctionSignaturePolicy) == typeid(*policy)){
                functionData = *policy->Data<SignatureData>();
                if(!(functionData.name.empty() || functionData.returnType.empty())){
                    if(functionData.hasAliasedReturn) functionData.returnType+="*";
                    CollectIdentifierTypeNameAndContext(functionData.returnType, functionData.name, "FUNCTION NAME");
                    for(auto param : functionData.parameters){
                        CollectIdentifierTypeNameAndContext(functionData.returnType, param.nameOfIdentifier, "FUNCTION PARAMETER");
                    }
                }
            }
        }
        void NotifyWrite(const PolicyDispatcher *policy, srcSAXEventDispatch::srcSAXEventContext &ctx){}
    protected:
        void *DataInner() const override {
            return (void*)0; // export profile to listeners
        }
        
    private:
        bool shouldTakeSample;
        std::vector<IdentifierData> allSystemIdentifiers;
        DeclTypePolicy declPolicy;
        DeclData declarationData;

        ParamTypePolicy paramPolicy;
        DeclData parameterData;

        FunctionSignaturePolicy functionPolicy;
        SignatureData functionData;

        void InitializeEventHandlers(){
            using namespace srcSAXEventDispatch;
            openEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->AddListenerDispatch(&declPolicy);
            };
            openEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&paramPolicy);
            };
            openEventMap[ParserState::function] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->AddListenerDispatch(&functionPolicy);
            };
            closeEventMap[ParserState::classn] = [this](srcSAXEventContext& ctx){
                if(!ctx.currentClassName.empty())
                    CollectIdentifierTypeNameAndContext("class", ctx.currentClassName, "CLASS");
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
        }
};
#endif