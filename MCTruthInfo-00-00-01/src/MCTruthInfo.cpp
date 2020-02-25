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
    declareProperty("NoTracingList", m_stableParticle);
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

MCTruthInfo::~MCTruthInfo() {
}

void MCTruthInfo::UpdateAvialInfo() {
    AvailableInfo::Clear();
    AvailableInfo::Add("mcmode", "int");
    AvailableInfo::Add("mcindex", "int");
    AvailableInfo::Add("motherindex", "int", "mcindex");
    AvailableInfo::Add("pdgid", "int", "mcindex");
    // add initialFourMomentum
    for (int i=0; i<m_decayTree.size(); ++i) {
        AvailableInfo::Add("initialP4" + m_decayTree.GetName(i),
                           "HepLorentzVector");
        AvailableInfo::Add("initialPos" + m_decayTree.GetName(i),
                           "HepLorentzVector");
        AvailableInfo::Add("finalPos"+m_decayTree.GetName(i),
                           "HepLorentzVector");
    }
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
void MCTruthInfo::GetInfoH(const std::string& info_name, 
                           HepLorentzVector& p4){
    // initialP4
    /// cout << "Info In MCTruthInfo::GetInfoH: ";
    /// cout << " info_name = " << info_name << endl; 
    if (info_name.substr(0, 9) == "initialP4") {
        string name = info_name.substr(9, info_name.size());
        int Index = m_decayTree.Index(name);
        p4 = m_p4AllDaughter[Index];
        return;
    }
    if (info_name.substr(0, 10) == "initialPos") {
        string name = info_name.substr(10, info_name.size());
        int Index = m_decayTree.Index(name);
        p4 = m_iniPosAllDaughter[Index];
        return;
    }
    if (info_name.substr(0, 8) == "finalPos") {
        string name = info_name.substr(8, info_name.size());
        int Index = m_decayTree.Index(name);
        p4 = m_finalPosAllDaughter[Index];
        return;
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
    GetAllGaughter();
}
void MCTruthInfo::SetDecayTree(const DecayTree& decayTree) {
    m_decayTree = decayTree;
    UpdateAvialInfo();
}

void MCTruthInfo::collectDaughters(const McParticle* mother,
                                   const vector<int>& _fid,
                                   vector<int>& found_fid,
                                   SmartRefVector<McParticle>& cc)
{
    int aPID = mother->particleProperty();
    /// cout << "Info in <MCTruthInfo::collectDaughters>: ";
    /// cout << "PID = " << aPID << endl;
    // found the pid in the target list
    if (std::find(_fid.begin(), _fid.end(), aPID) != _fid.end()) {
        if (std::count(_fid.begin(), _fid.end(), aPID) 
                > std::count(found_fid.begin(), found_fid.end(), aPID)) {
            cc.push_back(mother);
            found_fid.push_back(aPID);
            return;
        }
        // do not store this particle any more
        return;
    }
    // pass the stable particle: n, pi0, pi+, K+, Ks
    if (std::find(m_stableParticle.begin(), m_stableParticle.end(), aPID)
            != m_stableParticle.end()) {
        return;
    }

    // check the daughterList
    SmartRefVector<McParticle> tmp = mother->daughterList();

    for (SmartRefVector<McParticle>::iterator itr = tmp.begin();
         itr != tmp.end(); ++itr) {
        const McParticle * tmpmother = *itr;
        collectDaughters(*itr, _fid, found_fid, cc);
    }
}

bool MCTruthInfo::GetAllGaughter() {
    // declare temporary variables
    this->ClearAllInfo();
    const std::vector<int>& mm_fid = m_decayTree.GetFID();
    /// std::cout << "mm_fid: [ ";
    /// for (unsigned i = 0; i < mm_fid.size(); ++i) {
    ///     std::cout << mm_fid[i] << ", ";
    ///     }
    /// cout << "]" ;
    /// std::cout << std::endl;
    // get all Mother particles, usually there is only one.
    // getMothers(m_Mother, mothers);
    if (!m_mother) return false;
    // for each target Mother, like Psi(2S) 100443
    // find m_fid in their daughters of all generations.
    // // initialize sigID, sigP4, sigX4.
    vector<int> mm_fidcc;
    SmartRefVector<McParticle> daughterList = m_mother->daughterList();

    SmartRefVector<McParticle> tmpcc;
    vector<int> foundPID;
    collectDaughters(m_mother, mm_fid, foundPID, tmpcc);
    if (foundPID.size() == mm_fid.size()) {
        FillAllInfo(mm_fid, tmpcc); 
        return true;
    }
    if (m_decayTree.SelfConjugate()) {
        return false;
    }
    foundPID.clear();
    tmpcc.clear(); 
    collectDaughters(m_mother, m_decayTree.GetChannelCC().GetFID(),
            foundPID, tmpcc);
    if (foundPID.size() == mm_fid.size()) {
        FillAllInfo(mm_fid, tmpcc); 
        return true;
    }
    return false;
}
void MCTruthInfo::ClearAllInfo() {
    for(int i=0; i< m_decayTree.size(); ++i) {
        m_p4AllDaughter[i] = HepLorentzVector(0, 0, 0, 0);
        // m_finalp4AllDaughter[i] = HepLorentzVector(0, 0, 0, 0);
        m_iniPosAllDaughter[i] = HepLorentzVector(0, 0, 0, 0);
        m_finalPosAllDaughter[i] = HepLorentzVector(0, 0, 0, 0);
    }
}
void MCTruthInfo::FillAllInfo(const vector<int>& fid, 
                              SmartRefVector<McParticle>& found){
    std::map<int, const McParticle*> target;
    for (int i = 0; i< fid.size(); ++i) {
        target[i] = NULL;
    }
    int i;
    for (SmartRefVector<McParticle>::iterator itr = found.begin();
        itr != found.end(); ++itr){
        for (map<int, const McParticle*>::iterator tar = target.begin();
                tar!= target.end(); ++tar){
            if (tar->second != NULL) continue;
            i =  tar->first;    
            if (fid[i] != (*itr)->particleProperty()) continue;
            tar->second = *itr;
        } 
    }
    for (map<int, const McParticle*>::iterator tar = target.begin();
            tar!= target.end(); ++tar){
        if (tar->second == NULL) {
            cout << "Error in FillAllInfo: " ;
            cout <<  "Can't find pid: " << tar->first << endl;
            continue;
        }
        i =  tar->first;    
        m_p4AllDaughter[i] = (tar->second)->initialFourMomentum();
        // m_finalp4AllDaughter[i] = (tar->second)->finalFourMomentum();
        m_iniPosAllDaughter[i] = (tar->second)->initialPosition();
        m_finalPosAllDaughter[i] = (tar->second)->finalPosition();
    }
}
