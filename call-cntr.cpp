#define DEBUG_TYPE "call-cntr"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
using namespace llvm;
using namespace std;

namespace {
  struct Call_Cntr : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Call_Cntr() : FunctionPass(ID) {}

    int cntr = 0;

    virtual bool runOnFunction(Function &F) {
      outs() << "Function: " << F.getName() << '\n';
      for(BasicBlock &BB : F)
      {
        for(Instruction &I : BB)
        {
        if(CallInst *CI = dyn_cast<CallInst>(&I))
          {
            outs() << "call counter: " << cntr << '\n';
            string cntr_name = "call_cntr_" + to_string(cntr);
            cntr++;
            auto i32t = Type::getInt32Ty(F.getContext());
            // auto *newInst = new AllocaInst(i32t, 0, cntr_name, &I);
            auto *newInst = new AllocaInst(i32t, 0, ConstantInt::get(i32t, 1), MaybeAlign(4), cntr_name, &I);
          }
        }
        outs() << BB << '\n';
      }

      return false;
    }
  };
}

char Call_Cntr::ID = 0;
static RegisterPass<Call_Cntr> X("call-cntr", "call counter Pass");
