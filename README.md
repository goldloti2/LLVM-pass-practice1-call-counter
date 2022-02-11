# LLVM Pass Practice 1 - Call Counter, 2022.2.11

This is my first LLVM pass practice. This pass inserts a counter immediately before each call instruction, and prints out every counter, which indicates the number of times this call has been executed in run time. 

## Instrumentation
  
We use the original pass style. LLVM evolves a lot over time, but still supports the old style, which can even mix with the new style.

## Environment

Ubuntu 20.04  
LLVM 10.0.0

## How to Run

1. After downloading the repo, run `make` in the repo directory.
2. Run `opt` with option `-load /path/to/call-cntr.so -call-cntr`.

## Practice Note

### Type of Pass

In this practice, I use the `ModulePass` type. The corresponding `run()` function is `runOnModule()`, which takes the module as input. Finally, `printf` is inserted at the end of the `main()` function after all counters are properly inserted. `printf` is executed at run time.
  
There are other type of passes. Their `run()` functions iterate corresponding units in the input file. For example, a `FunctionPass` iterates over all functions with `runOnFunction()`.

### IR Builder

`IRBuilder` is a very convenient tool class. With IRBuilder, most instructions can be added with a simple function call.
  
Instantiate an `IRbuilder`:
```cpp
42. IRBuilder<> builder(M.getContext());
```

Set the insertion point:
```cpp
55. builder.SetInsertPoint(&I);
```

Add `load`, `add`, and `store` instructions:
```cpp
58. Value *loadcntr = builder.CreateLoad(i32t, g_cntr);
59. Value *temp_sum = builder.CreateAdd(loadcntr, builder.getInt32(1));
60. builder.CreateStore(temp_sum, g_cntr);
```

### Create a new / get an existing function and insert it into the file
In `CallInst *call_print(...)`, I want to insert `printf` to print out the counter.
1. Get or create a function in line 30: `FunctionCallee callee = M->getOrInsertFunction(...)`.
2. Load the global variable in lines 32 and 34. First get `GlobalVariable *g_cntr`, then load it into `Value *loadcntr`.
3. Create a constant string in lines 35 and 36. Add `string str` into the file and get the pointer `Value *p_str`.
4. Pack them into `vector<Value*> args`.
5. Insert the call instruction: `builder->CreateCall(callee, args)`.


## Reference
[llvm.org - Writing An LLVM Pass](https://llvm.org/docs/WritingAnLLVMPass.html "llvm.org - Writing An LLVM Pass")  
[llvm document](https://llvm.org/doxygen/index.html "llvm document")
