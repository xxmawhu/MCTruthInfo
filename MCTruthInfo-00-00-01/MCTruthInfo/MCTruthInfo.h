/*-*- c++ -*-*/

#ifndef MC_TRUTH_MATCH_SVC_H
#define MC_TRUTH_MATCH_SVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include <map>
#include <vector>
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "MCTruthInfo/IMCTruthInfo.h"
#include "EventNavigator/EventNavigator.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "McDecayModeSvc/McDecayModeSvc.h"

#include "HadronInfo/AvailableInfo.h"
#include "TupleSvc/DecayTree.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
using std::vector;
using Event::McParticle;
using Event::McParticleCol;
using Event::MdcMcHitCol;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

// template <class TYPE> class CnvFactory;

class MCTruthInfo : public Service,
                    virtual public IMCTruthInfo,
                    virtual AvailableInfo {

    // friend class CnvFactory<MCTruthInfo>;

   public:
    MCTruthInfo(const std::string& name, ISvcLocator* svcLoc);
    virtual ~MCTruthInfo();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);

    virtual void GetInfoI(const std::string& info_name, int& targe);
    virtual void GetInfoVi(const std::string& info_name,
                           std::vector<int>& targe);
    void SetDecayTree(const DecayTree& decayTree);

   private:
    DecayTree m_decayTree;
    McParticle* m_mother;
    int m_mode, m_motherPID;
    vector<int> m_motherindex, m_pdgid;

    IDataProviderSvc* eventSvc_;
    McDecayModeSvc* m_MCDecayModeSvc;

    mutable EventNavigator* m_navigator;
    void AnaDecayMode();
    bool irrational(HepLorentzVector p4);
    void UpdateAvialInfo();
};

#endif
