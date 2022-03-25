#ifndef WORDSFROMARCHIVEPOLICY
#define WORDSFROMARCHIVEPOLICY

#include <vector>
#include <ctype.h>
#include <exception>
#include <unordered_map>
#include <unordered_set>
#include <ClassPolicy.hpp>
#include <srcSAXHandler.hpp>
#include <DeclTypePolicy.hpp>
#include <ParamTypePolicy.hpp>
#include <srcSAXEventDispatcher.hpp>
#include <FunctionSignaturePolicy.hpp>

class WordsFromArchivePolicy : public srcSAXEventDispatch::EventListener, public srcSAXEventDispatch::PolicyDispatcher, public srcSAXEventDispatch::PolicyListener 
{
    private:
        std::string AnnotateIdentifier(std::string identifierType, std::string identifierName, std::string codeContext){
            std::cout<<identifierType<<" "<<identifierName<<" "<<codeContext<<std::endl;
            return "";
        }
    public:
        ~WordsFromArchivePolicy(){};
        WordsFromArchivePolicy(std::initializer_list<srcSAXEventDispatch::PolicyListener*> listeners = {}) : srcSAXEventDispatch::PolicyDispatcher(listeners){
            InitializeEventHandlers();
        
            declPolicy.AddListener(this);
            paramPolicy.AddListener(this);
            functionPolicy.AddListener(this);
        }
        void Notify(const PolicyDispatcher *policy, const srcSAXEventDispatch::srcSAXEventContext &ctx) override {
            using namespace srcSAXEventDispatch;
            if(typeid(DeclTypePolicy) == typeid(*policy)){
                decldata = *policy->Data<DeclData>();
                if(!(decldata.nameOfIdentifier.empty()||decldata.nameOfType.empty())){
                    if(ctx.IsOpen(ParserState::function)){
                        AnnotateIdentifier(decldata.nameOfType, decldata.nameOfIdentifier, "DECLARATION");
                    }else if(ctx.IsOpen(ParserState::classn) && !decldata.nameOfContainingClass.empty() && !decldata.nameOfType.empty() && !decldata.nameOfIdentifier.empty()){
                        AnnotateIdentifier(decldata.nameOfType, decldata.nameOfIdentifier, "ATTRIBUTE");
                    }
                }
            }else if(typeid(ParamTypePolicy) == typeid(*policy)){
                paramdata = *policy->Data<DeclData>();
                if(!(paramdata.nameOfIdentifier.empty() || paramdata.nameOfType.empty())){
                    AnnotateIdentifier(paramdata.nameOfType, paramdata.nameOfIdentifier, "PARAMETER");
                }
            }else if(typeid(FunctionSignaturePolicy) == typeid(*policy)){
                functiondata = *policy->Data<SignatureData>();
                if(!(functiondata.name.empty() || functiondata.returnType.empty())){
                    if(functiondata.hasAliasedReturn) functiondata.returnType+="*";
                    AnnotateIdentifier(functiondata.returnType, functiondata.name, "FUNCTION NAME");
                    for(auto param : functiondata.parameters){
                        AnnotateIdentifier(functiondata.returnType, param.nameOfIdentifier, "FUNCTION PARAMETER");
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
        DeclTypePolicy declPolicy;
        DeclData decldata;

        ParamTypePolicy paramPolicy;
        DeclData paramdata;

        FunctionSignaturePolicy functionPolicy;
        SignatureData functiondata;

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
                    AnnotateIdentifier("class", ctx.currentClassName, "CLASS");
            };
            closeEventMap[ParserState::functionblock] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            };
            closeEventMap[ParserState::declstmt] = [this](srcSAXEventContext& ctx){
                ctx.dispatcher->RemoveListenerDispatch(&declPolicy);
            };
            closeEventMap[ParserState::parameterlist] = [this](srcSAXEventContext& ctx) {
                ctx.dispatcher->RemoveListenerDispatch(&paramPolicy);
                //ctx.dispatcher->RemoveListenerDispatch(&functionPolicy);
            };
        }
};
#endif