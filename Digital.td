//===-- DigitalOther.td - Describe the Digital Target Machine ----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the Digital target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===---------------------------------------------------------------------===//
// Register File Description
//===---------------------------------------------------------------------===//

include "DigitalRegisterInfo.td"

//===---------------------------------------------------------------------===//
// Schedule Definitions
//===---------------------------------------------------------------------===//

include "DigitalSchedule.td"

//===---------------------------------------------------------------------===//
// Instruction Descriptions
//===---------------------------------------------------------------------===//

include "DigitalInstrInfo.td"

def DigitalInstrInfo : InstrInfo;

//===---------------------------------------------------------------------===//
// Assembly Printers
//===---------------------------------------------------------------------===//

def DigitalAsmWriter : AsmWriter {
 string AsmWriterClassName = "InstPrinter";
 bit isMCAsmWriter = 1;
}

//===---------------------------------------------------------------------===//
// Target Declaration
//===---------------------------------------------------------------------===//

def Digital : Target {
  let InstructionSet         = DigitalInstrInfo;
  let AssemblyWriters        = [DigitalAsmWriter];
}