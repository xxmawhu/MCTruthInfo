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
StatusCode MCTruthInfo::initialize() {
    MsgStream log(messageService(), name());
    log << MSG::INFO << "@initialize()" << endreq;

    StatusCode sc = Service::initialize();
    sc = serviceLocator()->service("EventDataSvc", eventSvc_, true);
    IMcDecayModeSvc* i_McDecayModeSvc;
    StatusCode sc_McDecayModeSvc = service("McDecayModeSvc", i_McDecayModeSvc);

    if (sc_McDecayModeSvc.isFailure()) {
        log << MSG::FATAL << "could not load McDecayModeSvc" << endreq;
        return sc_McDecayModeSvc;
    }
    m_MCDecayModeSvc = dynamic_cast<McDecayModeSvc*>(i_McDecayModeSvc);
    return sc;
}

StatusCode MCTruthInfo::finalize() {
    MsgStream log(messageService(), name());
    log << MSG::INFO << "@initialize()" << endreq;
    StatusCode sc = Service::finalize();
    return sc;
}

StatusCode MCTruthInfo::queryInterface(const InterfaceID& riid, void** ppvIF) {
    if (IMCTruthInfo::interfaceID().versionMatch(riid)) {
        *ppvIF = dynamic_cast<IMCTruthInfo*>(this);
    } else {
        return Service::queryInterface(riid, ppvIF);
    }
    addRef();
    // cout<<"MCTruthInfo::Inf:queryInterface"<<endl;
    return StatusCode::SUCCESS;
}
