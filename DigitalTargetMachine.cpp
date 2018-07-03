//===-- DigitalTargetMachine.cpp - Define TargetMachine for Digital -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "DigitalTargetMachine.h"
#include "Digital.h"

#include "DigitalSubtarget.h"
#include "DigitalTargetObjectFile.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "digital"

extern "C" void LLVMInitializeDigitalTarget() {
  // Register the target.
  RegisterTargetMachine<DigitalebTargetMachine> X(TheDigitalTarget);
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::PIC_;
  return *RM;
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 8 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
DigitalTargetMachine::DigitalTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     Optional<Reloc::Model> RM,
                                     CodeModel::Model CM, CodeGenOpt::Level OL,
                                     bool isLittle)
  //- Default is big endian
    : LLVMTargetMachine(T, "E-m:m-p:16:16-i16:16-n16-S16", TT,
                        CPU, FS, Options, getEffectiveRelocModel(RM), CM,
                        OL),
      isLittle(isLittle), TLOF(make_unique<DigitalTargetObjectFile>()),
      ABI(DigitalABIInfo::computeTargetABI()),
      DefaultSubtarget(TT, CPU, FS, isLittle, *this) {
  // initAsmInfo will display features by llc -march=digital -mcpu=help on 3.7 but
  // not on 3.6
  initAsmInfo();
}

DigitalTargetMachine::~DigitalTargetMachine() {}

void DigitalebTargetMachine::anchor() { }

DigitalebTargetMachine::DigitalebTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : DigitalTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false) {}

void DigitalelTargetMachine::anchor() { }

DigitalelTargetMachine::DigitalelTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : DigitalTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, true) {}

const DigitalSubtarget *
DigitalTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = llvm::make_unique<DigitalSubtarget>(TargetTriple, CPU, FS, isLittle,
                                         *this);
  }
  return I.get();
}

namespace {
//@DigitalPassConfig {
/// Digital Code Generator Pass Configuration Options.
class DigitalPassConfig : public TargetPassConfig {
public:
  DigitalPassConfig(DigitalTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  DigitalTargetMachine &getDigitalTargetMachine() const {
    return getTM<DigitalTargetMachine>();
  }

  const DigitalSubtarget &getDigitalSubtarget() const {
    return *getDigitalTargetMachine().getSubtargetImpl();
  }
};
} // namespace

TargetPassConfig *DigitalTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new DigitalPassConfig(this, PM);
}
