//===- DigitalInstrInfo.td - Target Description for Digital Target -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Digital implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Digital profiles and nodes
//===----------------------------------------------------------------------===//

def SDT_DigitalRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;

// Return
def DigitalRet : SDNode<"DigitalISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "DigitalInstrFormats.td"

//===----------------------------------------------------------------------===//
// Digital Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//
// Instruction operand types

// Signed Operand
def simm16      : Operand<i32> {
  let DecoderMethod= "DecodeSimm16";
}

// Address operand
def mem : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops CPURegs, simm16);
  let EncoderMethod = "getMemEncoding";
}

// Node immediate fits as 16-bit sign extended on target immediate.
// e.g. addi, andi
def immSExt16  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

// Digital Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addr : 
  ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], [SDNPWantParent]>;

//===----------------------------------------------------------------------===//
// Pattern fragment for load/store
//===----------------------------------------------------------------------===//

// class AlignedLoad<PatFrag Node> :
//   PatFrag<(ops node:$ptr), (Node node:$ptr), [{
//   LoadSDNode *LD = cast<LoadSDNode>(N);
//   return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
// }]>;

// class AlignedStore<PatFrag Node> :
//   PatFrag<(ops node:$val, node:$ptr), (Node node:$val, node:$ptr), [{
//   StoreSDNode *SD = cast<StoreSDNode>(N);
//   return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlignment();
// }]>;

// // Load/Store PatFrags.
// def load_a          : AlignedLoad<load>;
// def store_a         : AlignedStore<store>;

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

// Arithmetic and logical instructions with 2 register operands.
// class ArithLogicI<bits<6> op, string instr_asm, SDNode OpNode,
//                   Operand Od, PatLeaf imm_type, RegisterClass RC> :
//   FL<op, (outs GPROut:$ra), (ins RC:$rb, Od:$imm16),
//      !strconcat(instr_asm, "\t$ra, $rb, $imm16"),
//      [(set GPROut:$ra, (OpNode RC:$rb, imm_type:$imm16))], IIAlu> {
//   let isReMaterializable = 1;
// }

class ArithLogic<bits<6> op, string instr_asm, SDNode OpNode,
                 InstrItinClass itin, RegisterClass RC, bit isComm = 0> :
  TwoRegs<op, (outs RC:$rd), (ins RC:$rf, RC:$rs),
     !strconcat(instr_asm, "\t$rd, $rs"),
     [(set RC:$rd, (OpNode RC:$rf, RC:$rs))], itin> {
  let isCommutable = isComm;	// e.g. add rb rc =  add rc rb
  let isReMaterializable = 1;
}

// class FMem<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern,
//           InstrItinClass itin>: FL<op, outs, ins, asmstr, pattern, itin> {
//   bits<20> addr;
//   let Inst{19-16} = addr{19-16};
//   let Inst{15-0}  = addr{15-0};
//   let DecoderMethod = "DecodeMem";
// }

// Memory Load/Store
// let canFoldAsLoad = 1 in
// class LoadM<bits<6> op, string instr_asm, PatFrag OpNode, RegisterClass RC,
//             Operand MemOpnd, bit Pseudo>:
//   FMem<op, (outs RC:$ra), (ins MemOpnd:$addr),
//      !strconcat(instr_asm, "\t$ra, $addr"),
//      [(set RC:$ra, (OpNode addr:$addr))], IILoad> {
//   let isPseudo = Pseudo;
// }

// class StoreM<bits<6> op, string instr_asm, PatFrag OpNode, RegisterClass RC,
//              Operand MemOpnd, bit Pseudo>:
//   FMem<op, (outs), (ins RC:$ra, MemOpnd:$addr),
//      !strconcat(instr_asm, "\t$ra, $addr"),
//      [(OpNode RC:$ra, addr:$addr)], IIStore> {
//   let isPseudo = Pseudo;
// }

// //@ 32-bit load.
// multiclass LoadM32<bits<6> op, string instr_asm, PatFrag OpNode,
//                    bit Pseudo = 0> {
//   def #NAME# : LoadM<op, instr_asm, OpNode, GPROut, mem, Pseudo>;
// }

// // 32-bit store.
// multiclass StoreM32<bits<6> op, string instr_asm, PatFrag OpNode,
//                     bit Pseudo = 0> {
//   def #NAME# : StoreM<op, instr_asm, OpNode, CPURegs, mem, Pseudo>;
// }

//@JumpFR {
let isBranch=1, isTerminator=1, isBarrier=1, hasDelaySlot = 1,
    isIndirectBranch = 1 in
class JumpFR<bits<6> op, string instr_asm, RegisterClass RC>:
  Return<op, (outs), (ins RC:$rs),
     !strconcat(instr_asm, "\t$rs"), [(brind RC:$rs)], IIBranch> {
}
//@JumpFR }

// Return instruction
class RetBase<RegisterClass RC>: JumpFR<0x35, "rret", RC> {
  let isReturn = 1;
  let isCodeGenOnly = 1;
  let hasCtrlDep = 1;
  let hasExtraSrcRegAllocReq = 1;
}

  
//===----------------------------------------------------------------------===//
// Instruction definition
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Digital Instructions
//===----------------------------------------------------------------------===//

// No operation
def NOP     : DigitalInst<0x0, (outs), (ins), "NOP", [], IIAlu>;

def MOV     : TwoRegs<0x1, (outs CPURegs:$rd), (ins CPURegs:$rs), "MOV $rd,$rs",
                [(set CPURegs:$rd, (load CPURegs:$rs))], IIAlu>;

def ADD     : ArithLogic<0x2, "ADD", add, IIAlu, CPURegs, 1>;
def SUB     : ArithLogic<0x4, "SUB", sub, IIAlu, CPURegs>;
def AND     : ArithLogic<0x6, "AND", and, IIAlu, CPURegs, 1>;
def OR      : ArithLogic<0x7, "OR",  or,  IIAlu, CPURegs, 1>;
def EOR     : ArithLogic<0x8, "EOR", xor, IIAlu, CPURegs, 1>;

/// Load and Store Instructions
///  aligned
// defm LD     : LoadM32<0x27,  "ld",  load_a>;
// defm ST     : StoreM32<0x26, "st",  store_a>;

/// Arithmetic Instructions (ALU Immediate)
// IR "add" defined in include/llvm/Target/TargetSelectionDAG.td, line 315 (def add).
// def ADDi   : ArithLogicI<0xb, "addi", add, simm16, immSExt16, CPURegs>;

/// Arithmetic Instructions (3-Operand, R-Type)

/// Shift Instructions

def JMP      : JumpFR<0x36, "jmp", CPURegs>;

def RRET     : RetBase<CPURegs>;

//===----------------------------------------------------------------------===//
//  Arbitrary patterns that map to one or more instructions
//===----------------------------------------------------------------------===//

// Small immediates
// def : Pat<(i32 immSExt16:$in),
//           (ADDiu ZERO, imm:$in)>;