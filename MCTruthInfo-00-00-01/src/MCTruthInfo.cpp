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
    : Service(name, svcLoc), m_mother(0), m_mode(0), m_run(0), m_event(0) {
    declareProperty("Mother", m_motherPID = 443);
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
    AvailableInfo::Add("mcmode", "int");
    AvailableInfo::Add("mcindex", "int");
    AvailableInfo::Add("motherindex", "int", "mcindex");
    AvailableInfo::Add("pdgid", "int", "mcindex");
}
void MCTruthInfo::GetInfoI(const std::string& info_name, int& targe) {
   //cout << "Info in MCTruthInfo::GetInfoI: "
   //    << "info_name = " << info_name << endl;
    this->AnaDecayMode();
    if (info_name == "mcindex") {
        targe = m_motherindex.size();
    } else if (info_name == "mcmode") {
        targe = m_mode;
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
    // get m_mother, m_mode, m_pdgid, and m_motherindex
    SmartDataPtr<Event::EventHeader> eventHeader(eventSvc_,
                                                 "/Event/EventHeader");
    if (m_run == eventHeader->runNumber() &&
        m_event == eventHeader->eventNumber()) {
     //  cout << "Info in MCTruthInfo::AnaDecayMode: "
     //      << "#run = " << m_run << ", #id = " << m_event << endl;
        return;
    } else {
        m_run = eventHeader->runNumber();
        m_event = eventHeader->eventNumber();
     //  cout << "Info in MCTruthInfo::AnaDecayMode: "
     //      << "#run = " << m_run << ", #id = " << m_event << endl;
    }
    if (m_run > 0) return;
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc_,
                                                     "/Event/MC/McParticleCol");
    if (!mcParticleCol) {
        return;
    }
    Event::McParticleCol::iterator itr;
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
