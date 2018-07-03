//===-- llvm/Target/DigitalTargetObjectFile.h - Digital Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETOBJECTFILE_H

#include "DigitalConfig.h"
#if CH >= CH3_1

#include "DigitalTargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class DigitalTargetMachine;
  class DigitalTargetObjectFile : public TargetLoweringObjectFileELF {
    MCSection *SmallDataSection;
    MCSection *SmallBSSSection;
    const DigitalTargetMachine *TM;
  public:

    void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  };
} // end namespace llvm

#endif // #if CH >= CH3_1

#endif
