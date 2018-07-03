//===-- DigitalInstrFormats.td - Digital Instruction Formats -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the GNU General Public License Version 3.
// See LICENSE for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe DIGITAL instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  rd      - destination register
//  rs      - source register
//  rf      - fake register for lowering 3 to 2 operand instructions
//  imm     - immediate bit
//  cs      - constant
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Basic Digital Format instruction class : <|opcode|>
//===----------------------------------------------------------------------===//
class DigitalInst<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern,
          InstrItinClass itin>: Instruction
{
  // Inst and Size: for tablegen(... -gen-emitter) and 
  // tablegen(... -gen-disassembler) in CMakeLists.txt
  field bits<16> Inst;

  let Namespace = "Digital";
  let DecoderNamespace = "Digital";

  let Size = 2;

  bits<6> Opcode = op;

  let Inst{14-9} = Opcode;

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = itin;

  field bits<16> SoftFail = 0;
}

//===----------------------------------------------------------------------===//
// Format with one Register instruction class in Digital : <|opcode|rd|>
//===----------------------------------------------------------------------===//

class OneReg<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      DigitalInst<op, outs, ins, asmstr, pattern, itin>
{
  bits<4>  rd;

  let Inst{7-4} = rd;
}

//===----------------------------------------------------------------------===//
// Format for return instruction in Digital : <|opcode|rs|>
//===----------------------------------------------------------------------===//

class Return<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      DigitalInst<op, outs, ins, asmstr, pattern, itin>
{
  bits<4>  rs;

  let Inst{3-0} = rs;
}

//===----------------------------------------------------------------------===//
// Format with two Registers instruction class in Digital : <|opcode|rd|rs|>
//===----------------------------------------------------------------------===//

class TwoRegs<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      DigitalInst<op, outs, ins, asmstr, pattern, itin>
{
  bits<4>  rd;
  bits<4>  rs;
  bits<4>  rf = rd;

  let Inst{7-4} = rd;
  let Inst{3-0} = rs;
}

//===----------------------------------------------------------------------===//
// Format with Constant instruction class in Digital : <|opcode|rd|cs|>
//===----------------------------------------------------------------------===//

class RegAndConstant<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      DigitalInst<op, outs, ins, asmstr, pattern, itin>
{
  bits<4>  rd;
  bits<5>  cs;

  let Inst{7-4} = rd;
  let Inst{8,3-0} = cs;
}

//===----------------------------------------------------------------------===//
// Format for Pseudo instruction class in Digital : <|imm|cs|>
// This is a pseudo format for constant values bigger than 31.
//===----------------------------------------------------------------------===//

class Pseudo<bits<6> op, dag outs, dag ins, string asmstr,
         list<dag> pattern, InstrItinClass itin>:
      DigitalInst<op, outs, ins, asmstr, pattern, itin>
{
  bits<1>   imm = 1;
  bits<15>  cs;

  let Inst{15}    = imm;
  let Inst{14-0}  = cs;
}