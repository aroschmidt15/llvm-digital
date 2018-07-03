//===-- DigitalTargetMachine.h - Define TargetMachine for Digital ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Digital specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETMACHINE_H
#define LLVM_LIB_TARGET_DIGITAL_DIGITALTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {

class DigitalTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  // Selected ABI
  DigitalABIInfo ABI;
  DigitalSubtarget DefaultSubtarget;

  mutable StringMap<std::unique_ptr<DigitalSubtarget>> SubtargetMap;
public:
  DigitalTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    Optional<Reloc::Model> RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL);
  ~DigitalTargetMachine() override;

  const DigitalSubtarget *getSubtargetImpl() const {
    return &DefaultSubtarget;
  }

  const DigitalSubtarget *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  const DigitalABIInfo &getABI() const { return ABI; }
};

} // End llvm namespace

#endif

