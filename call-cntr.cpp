#define DEBUG_TYPE "call-cntr"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/Module.h"
// #include "llvm/IR/Function.h"
// #include "llvm/ADT/Statistic.h"
// #include "llvm/IR/Instructions.h"
// #include "llvm/IR/Constants.h"
// #include "llvm/IR/InstrTypes.h"
using namespace llvm;
using namespace std;

namespace {
    struct Call_Cntr : public ModulePass {
        static char ID; // Pass identification, replacement for typeid
        Call_Cntr() : ModulePass(ID) {}

        GlobalVariable *createGlob(Module *M, Type *type, string name) {
            M->getOrInsertGlobal(name, type);
            GlobalVariable *gVar = M->getNamedGlobal(name);
            gVar->setLinkage(GlobalValue::CommonLinkage);
            gVar->setAlignment(MaybeAlign(4));
            return gVar;
        }

        virtual bool runOnModule(Module &M) {
            IRBuilder<> builder(M.getContext());
            Type *i32t = builder.getInt32Ty();
            for(Function &F : M) {
                int cntr = 0;
                errs() << "Function: " << F.getName() << '\n';
                for(BasicBlock &BB : F) {
                    for(Instruction &I : BB) {
                        if(CallInst *CI = dyn_cast<CallInst>(&I))
                        {
                            string cntr_name = "call_" + CI->getCalledFunction()->getName().str() + "_in_" + F.getName().str() + "_" + to_string(cntr++);
                            errs() << "call counter: " << cntr_name << '\n';
                            builder.SetInsertPoint(&I);
                            GlobalVariable *cntr_glob = createGlob(&M, i32t, cntr_name);
                            cntr_glob->setInitializer(builder.getInt32(0));
                            Value *loadcntr = builder.CreateLoad(i32t, cntr_glob);
                            Value *temp_sum = builder.CreateAdd(loadcntr, builder.getInt32(1));
                            builder.CreateStore(temp_sum, cntr_glob);
                        }
                    }
                }
            }
            errs() <<"----------\n" << M << "\n----------\n";

            return false;
        }
    };
}

char Call_Cntr::ID = 0;
static RegisterPass<Call_Cntr> X("call-cntr", "call counter Pass");
