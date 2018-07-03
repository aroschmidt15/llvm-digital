//===-- DigitalTargetInfo.cpp - Digital Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Digital.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheDigitalTarget;

extern "C" void LLVMInitializeDigitalTargetInfo() {
  RegisterTarget<Triple::digital,
        /*HasJIT=*/false> X(TheDigitalTarget, "digital", "Digital simulated CPU", "Digital");
}