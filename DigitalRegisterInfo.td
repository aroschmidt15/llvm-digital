//===-- DigitalRegisterInfo.td - Digital Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Digital register file
//===----------------------------------------------------------------------===//

// Digital CPU Registers
class DigitalReg<bits<16> Enc, string n> : Register<n> {
  // For tablegen(... -gen-emitter)  in CMakeLists.txt
  let HWEncoding = Enc;
  
  let Namespace = "Digital";
}


//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "Digital" in {
  //@ General Purpose Registers
  def R0   : DigitalReg<0,  "0">,    DwarfRegNum<[0]>;
  def R1   : DigitalReg<1,  "1">,    DwarfRegNum<[1]>;
  def R2   : DigitalReg<2,  "2">,    DwarfRegNum<[2]>;
  def R3   : DigitalReg<3,  "3">,    DwarfRegNum<[3]>;
  def R4   : DigitalReg<4,  "4">,    DwarfRegNum<[4]>;
  def R5   : DigitalReg<5,  "5">,    DwarfRegNum<[5]>;
  def R6   : DigitalReg<6,  "6">,    DwarfRegNum<[6]>;
  def R7   : DigitalReg<7,  "7">,    DwarfRegNum<[7]>;
  def R8   : DigitalReg<8,  "8">,    DwarfRegNum<[8]>;
  def R9   : DigitalReg<9,  "9">,    DwarfRegNum<[9]>;
  def R10  : DigitalReg<10, "10">,   DwarfRegNum<[10]>;
  def R11  : DigitalReg<11, "11">,   DwarfRegNum<[11]>;
  def R12  : DigitalReg<12, "12">,   DwarfRegNum<[12]>;
  def BP   : DigitalReg<13, "bp">,   DwarfRegNum<[13]>;
  def SP   : DigitalReg<14, "sp">,   DwarfRegNum<[14]>;
  def RA   : DigitalReg<15, "ra">,   DwarfRegNum<[15]>;
}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"Digital", [i16], 16, (
  // Registers
  add R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12,
  // Reserved
  BP, SP, RA)>;