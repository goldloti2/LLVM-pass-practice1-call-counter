#define DEBUG_TYPE "call-cntr"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;
using namespace std;

namespace {
    struct Call_Cntr : public ModulePass {
        static char ID; // Pass identification, replacement for typeid
        Call_Cntr() : ModulePass(ID) {}

        int cntr = 0;

        GlobalVariable *createGlob(Module *M, string Name, int init) {
            Type *i32t = Type::getInt32Ty(M->getContext());
            M->getOrInsertGlobal(Name, i32t);
            GlobalVariable *gVar = M->getNamedGlobal(Name);
            gVar->setLinkage(GlobalValue::CommonLinkage);
            gVar->setAlignment(MaybeAlign(4));
            gVar->setInitializer(ConstantInt::get(i32t, init));
            return gVar;
        }

        virtual bool runOnModule(Module &M) {
            Type *i32t = Type::getInt32Ty(M.getContext());
            for(Function &F : M) {
                outs() << "Function: " << F.getName() << '\n';
                for(BasicBlock &BB : F) {
                    for(Instruction &I : BB) {
                        if(CallInst *CI = dyn_cast<CallInst>(&I))
                        {
                            outs() << "call counter: " << cntr << '\n';
                            string cntr_name = "call_cntr_" + to_string(cntr);
                            Value *cntr_glob = createGlob(&M, cntr_name, 0);
                            cntr++;
                            // auto *newInst = new AllocaInst(i32t, 0, Twine(), &I);
                            // newInst->setAlignment(MaybeAlign(4));
                            // auto *testInst = new AllocaInst(i32t, 0, ConstantInt::get(i32t, 1), Twine(), &I);
                            Value *loadcntr = new LoadInst(i32t, cntr_glob, Twine(), false, MaybeAlign(4), &I);
                            Value *addcntr = new BinaryOperator::Create(1, loadcntr, ConstantInt::get(i32t, 1), Twine(), &I); // Can't find Create!!
                            Value *storecntr = new StoreInst(loadcntr, cntr_glob, false, MaybeAlign(4), &I);
                        }
                    }
                }
            }
            outs() <<"----------\n" << M << "\n----------\n";

            return false;
        }
    };
}

char Call_Cntr::ID = 0;
static RegisterPass<Call_Cntr> X("call-cntr", "call counter Pass");
