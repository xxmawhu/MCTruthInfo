/* ====================================================
#   Copyright (C)2020 All rights reserved.
#
#   Author        : Xin-Xin MA
#   Email         : xxmawhu@163.com
#   File Name     : IMCTruthInfo.h
#   Create Time   : 2020-02-08 15:07
#   Last Modified : 2020-02-08 15:07
#   Describe      :
#
# ====================================================*/

#ifndef MCTruthInfo__IMCTruthInfo_H
#define MCTruthInfo__IMCTruthInfo_H

#include "GaudiKernel/IService.h"
#include "McTruth/McParticle.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "CLHEP/Vector/LorentzVector.h"

/* Decaration of the interface ID */
static const InterfaceID IID_IMCTruthInfo("IMCTruthInfo", 1, 0);

class IMCTruthInfo : virtual public IService {
   public:
    virtual ~IMCTruthInfo() {}
    static const InterfaceID& interfaceID() { return IID_IMCTruthInfo; }
};

#endif
