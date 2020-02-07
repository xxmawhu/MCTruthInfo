#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MCTruthInfo/MCTruthInfo.h"

DECLARE_SERVICE_FACTORY( MCTruthInfo )

DECLARE_FACTORY_ENTRIES( MCTruthInfo ) { 
  DECLARE_SERVICE( MCTruthInfo );
}
