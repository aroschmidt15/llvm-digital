//===-- DigitalMCTargetDesc.h - Digital Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Digital specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_DIGITAL_MCTARGETDESC_DIGITALMCTARGETDESC_H
#define LLVM_LIB_TARGET_DIGITAL_MCTARGETDESC_DIGITALMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

namespace llvm {
class Target;
class Triple;

extern Target TheDigitalTarget;

} // End llvm namespace

// Defines symbolic names for Digital registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "DigitalGenRegisterInfo.inc"

// Defines symbolic names for the Digital instructions.
#define GET_INSTRINFO_ENUM
#include "DigitalGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "DigitalGenSubtargetInfo.inc"

#endif