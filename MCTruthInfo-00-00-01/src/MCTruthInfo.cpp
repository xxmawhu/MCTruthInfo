#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "EventModel/EventHeader.h"
#include "MCTruthInfo/MCTruthInfo.h"
#include <algorithm>

using CLHEP::HepLorentzVector;
using namespace Event;
using std::cout;
using std::endl;
MCTruthInfo::MCTruthInfo(const std::string& name, ISvcLocator* svcLoc)
    : Service(name, svcLoc), m_mother(0), m_mode(0) {
    declareProperty("Mother", m_motherPID);
    UpdateAvialInfo();
}

bool MCTruthInfo::irrational(HepLorentzVector p4) {
    for (int i = 0; i < 4; i++) {
        if (isnan(p4[i])) return true;
    }
    // if(p4.rho()>10) return true;
    // if(p4.rho()<0.0001) return true;
    // if(p4[3]<0.0001) return true;
    // if(p4[3]>10) return true;
    return false;
}

MCTruthInfo::~MCTruthInfo() {}

void MCTruthInfo::UpdateAvialInfo() {
    AvailableInfo::Clear();
    AvailableInfo::Add("mcindex", "int");
    AvailableInfo::Add("motherindex", "int", "mcindex");
    AvailableInfo::Add("pdgid", "int", "mcindex");
}
void MCTruthInfo::GetInfoI(const std::string& info_name, int& targe) {
    if (info_name == "mcindex") {
        this->AnaDecayMode();
        targe = m_motherindex.size();
    }
}
void MCTruthInfo::GetInfoVi(const std::string& info_name,
                            std::vector<int>& targe) {
    this->AnaDecayMode();
    if (info_name == "motherindex") {
        targe = m_motherindex;
    } else if (info_name == "pdgid") {
        targe = m_pdgid;
    }
}

void MCTruthInfo::AnaDecayMode() {
    // get m_mother
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc_,
                                                     "/Event/MC/McParticleCol");
    if (!mcParticleCol) {
        cout << "Could not retireve McparticleCol" << endl;
        return;
    }
    Event::McParticleCol::iterator itr;
    int pid;
    for (itr = mcParticleCol->begin(); itr != mcParticleCol->end(); itr++) {
        if (m_motherPID == (*itr)->particleProperty()) {
            m_mother = const_cast<McParticle*>(*itr);
        }
    }
    if (!m_mother) {
        m_mode = 0;
        m_motherindex.clear();
        m_pdgid.clear();
        return;
    }
    m_mode = m_MCDecayModeSvc->extract(m_mother, m_pdgid, m_motherindex);
}
void MCTruthInfo::SetDecayTree(const DecayTree& decayTree) {
    m_decayTree = decayTree;
    UpdateAvialInfo();
}
