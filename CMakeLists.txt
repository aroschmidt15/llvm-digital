
set(LLVM_TARGET_DEFINITIONS Digital.td)

# Generate DigitalGenRegisterInfo.inc and DigitalGenInstrInfo.inc which included by 
#  your hand code C++ files. 
# DigitalGenRegisterInfo.inc came from DigitalRegisterInfo.td, DigitalGenInstrInfo.inc 
#  came from DigitalInstrInfo.td.
tablegen(LLVM DigitalGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM DigitalGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM DigitalGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM DigitalGenMCPseudoLowering.inc -gen-pseudo-lowering)

# DigitalCommonTableGen must be defined
add_public_tablegen_target(DigitalCommonTableGen)

# DigitalCodeGen should match with LLVMBuild.txt DigitalCodeGen
add_llvm_target(DigitalCodeGen
  DigitalTargetMachine.cpp
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)