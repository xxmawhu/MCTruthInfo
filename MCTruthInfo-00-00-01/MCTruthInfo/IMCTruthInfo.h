#ifndef MCTruthInfo__IMCTruthInfo_H
#define MCTruthInfo__IMCTruthInfo_H

#include "GaudiKernel/IService.h"
#include "McTruth/McParticle.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "CLHEP/Vector/LorentzVector.h"

/* Decaration of the interface ID */
static const InterfaceID IID_IMCTruthInfo("IMCTruthInfo", 1, 0);

using CLHEP::HepLorentzVector;
class EvtRecMC;
namespace Event {
class McParticle;
}

class IMCTruthInfo : virtual public IService {
   public:
    virtual ~IMCTruthInfo() {}
    static const InterfaceID& interfaceID() { return IID_IMCTruthInfo; }
    virtual bool matchTrack(const EvtRecTrack* recTrk,
                            Event::McParticle* mcTrk) = 0;
    virtual bool matchTrack(const EvtRecTrack* recTrk, int pid,
                            Event::McParticle* mcTrk) = 0;
    virtual bool match(vector<const EvtRecTrack*> recTracks, vector<int> Pids,
                       vector<int> pdgid, vector<int> dechain) = 0;
};

#endif
