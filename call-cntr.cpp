#define DEBUG_TYPE "call-cntr"
#include <vector>
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
            gVar->setAlignment(MaybeAlign(type->getScalarSizeInBits()/8));
            // errs() << gVar << '\n';
            return gVar;
        }

        CallInst *call_print(Module *M, IRBuilder<> *builder, string name) {
            FunctionCallee callee = M->getOrInsertFunction("printf",
                                                           FunctionType::get(builder->getInt32Ty(), builder->getInt8PtrTy(), true));
            GlobalVariable *g_cntr = M->getNamedGlobal(name);
            // errs() << name << ' ' << gVar << '\n';
            Value *loadcntr = builder->CreateLoad(builder->getInt32Ty(), g_cntr);
            string str = name + ": %u\n";
            Value *p_str = builder->CreateGlobalStringPtr(str);
            vector<Value*> args = {p_str, loadcntr};
            return builder->CreateCall(callee, args);
        }

        virtual bool runOnModule(Module &M) {
            IRBuilder<> builder(M.getContext());
            Type *i32t = builder.getInt32Ty();
            vector<string> cntr_name_list;
            for(Function &F : M) {
                int cntr = 0;
                errs() << "Function: " << F.getName() << '\n';
                for(BasicBlock &BB : F) {
                    for(Instruction &I : BB) {
                        if(CallInst *CI = dyn_cast<CallInst>(&I))
                        {
                            string cntr_name = "call_" + CI->getCalledFunction()->getName().str() + "_in_" + F.getName().str() + '_' + to_string(cntr++);
                            cntr_name_list.push_back(cntr_name);
                            errs() << "call counter: " << cntr_name << '\n';
                            builder.SetInsertPoint(&I);
                            GlobalVariable *g_cntr = createGlob(&M, i32t, cntr_name);
                            g_cntr->setInitializer(builder.getInt32(0));
                            Value *loadcntr = builder.CreateLoad(i32t, g_cntr);
                            Value *temp_sum = builder.CreateAdd(loadcntr, builder.getInt32(1));
                            builder.CreateStore(temp_sum, g_cntr);
                        }
                    }
                }
                // errs() << cntr_name_list.size() << '/' << cntr_name_list.capacity() << '\n';
            }
            // for(string x : cntr_name_list)
            //     errs() << x << '\n';
            Instruction &end = M.getFunction("main")->back().back();
            builder.SetInsertPoint(&end);
            for(string name : cntr_name_list)
                call_print(&M, &builder, name);
            errs() << "----------\n" << M << "\n----------\n";

            return false;
        }
    };
}

char Call_Cntr::ID = 0;
static RegisterPass<Call_Cntr> X("call-cntr", "call counter Pass");
