//===-- DigitalTargetMachine.cpp - Define TargetMachine for Digital -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "DigitalTargetMachine.h"
#include "Digital.h"

#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "digital"

extern "C" void LLVMInitializeDigitalTarget() {
    // Register the target.
    RegisterTargetMachine<DigitalTargetMachine> X(TheDigitalTarget);
}

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
    if (!RM.hasValue())
        return Reloc::PIC_;
    return *RM;
}

// Code models. Some only make sense for 64-bit code.
//
// SunCC  Reloc   CodeModel  Constraints
// abs32  Static  Small      t^ext+data+bss linked below 2^32 bytes
// abs44  Static  Medium     text+data+bss linked below 2^44 bytes
// abs64  Static  Large      text smaller than 2^31 bytes
// pic13  PIC_    Small      GOT < 2^13 bytes
// pic32  PIC_    Medium     GOT < 2^32 bytes
//
// All code models require that the text segment is smaller than 2GB.
static CodeModel::Model getEffectiveCodeModel(Optional<CodeModel::Model> CM) {
    if (!CM)
        return CodeModel::Small;
    return *CM;
}

// /// Create an Digital 16bit architecture model
DigitalTargetMachine::DigitalTargetMachine(
        const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
        const TargetOptions &Options, Optional<Reloc::Model> RM,
        Optional<CodeModel::Model> CM, CodeGenOpt::Level OL)
        : LLVMTargetMachine(
        T, DataLayout("E-p:16:16-i:16:16-n16-S16"), TT, CPU, FS, Options,
        getEffectiveRelocModel(RM),
        getEffectiveCodeModel(CM),
        OL),
          TLOF(make_unique<DigitalTargetObjectFile>()),
          DefaultSubtarget(TT, CPU, FS, *this) {
    initAsmInfo();
}

DigitalTargetMachine::~DigitalTargetMachine() {}

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

    // FIXME: This is related to the code below to reset the target options,
    // we need to know whether or not the soft float flag is set on the
    // function, so we can enable it as a subtarget feature.
    bool softFloat =
            F.hasFnAttribute("use-soft-float") &&
            F.getFnAttribute("use-soft-float").getValueAsString() == "true";

    if (softFloat)
        FS += FS.empty() ? "+soft-float" : ",+soft-float";

    auto &I = SubtargetMap[CPU + FS];
    if (!I) {
        // This needs to be done before we create a new subtarget since any
        // creation will depend on the TM and the code generation flags on the
        // function that reside in TargetOptions.
        resetTargetOptions(F);
        I = llvm::make_unique<DigitalSubtarget>(TargetTriple, CPU, FS, *this,
                                             this->is64Bit);
    }
    return I.get();
}

namespace {
/// Digital Code Generator Pass Configuration Options.
    class DigitalPassConfig : public TargetPassConfig {
    public:
        DigitalPassConfig(DigitalTargetMachine &TM, PassManagerBase &PM)
                : TargetPassConfig(TM, PM) {}

        DigitalTargetMachine &getDigitalTargetMachine() const {
            return getTM<DigitalTargetMachine>();
        }

        void addIRPasses() override;

        bool addInstSelector() override;

        void addPreEmitPass() override;
    };
} // namespace

TargetPassConfig *DigitalTargetMachine::createPassConfig(PassManagerBase &PM) {
    return new DigitalPassConfig(*this, PM);
}

void DigitalPassConfig::addIRPasses() {
    addPass(createAtomicExpandPass());

    TargetPassConfig::addIRPasses();
}

bool DigitalPassConfig::addInstSelector() {
    addPass(createDigitalISelDag(getDigitalTargetMachine()));
    return false;
}

void DigitalPassConfig::addPreEmitPass() {
    addPass(createDigitalDelaySlotFillerPass());

    if (this->getDigitalTargetMachine().getSubtargetImpl()->insertNOPLoad()) {
        addPass(new InsertNOPLoad());
    }
    if (this->getDigitalTargetMachine().getSubtargetImpl()->detectRoundChange()) {
        addPass(new DetectRoundChange());
    }
    if (this->getDigitalTargetMachine().getSubtargetImpl()->fixAllFDIVSQRT()) {
        addPass(new FixAllFDIVSQRT());
    }
}

void DigitalV8TargetMachine::anchor() {}

DigitalV8TargetMachine::DigitalV8TargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT)
        : DigitalTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}

void DigitalV9TargetMachine::anchor() {}

DigitalV9TargetMachine::DigitalV9TargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT)
        : DigitalTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

void DigitalelTargetMachine::anchor() {}

DigitalelTargetMachine::DigitalelTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Optional<Reloc::Model> RM,
                                         Optional<CodeModel::Model> CM,
                                         CodeGenOpt::Level OL, bool JIT)
        : DigitalTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}
