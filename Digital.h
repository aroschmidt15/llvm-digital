//===-- Digital.h - Top-level interface for Digital representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Digital back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_CPU0_CPU0_H
#define LLVM_LIB_TARGET_CPU0_CPU0_H

#include "MCTargetDesc/DigitalMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class DigitalTargetMachine;
  class FunctionPass;

} // end namespace llvm;

#endif