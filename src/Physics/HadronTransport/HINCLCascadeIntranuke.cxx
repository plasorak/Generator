#include "Framework/Conventions/GBuild.h"
#ifdef __GENIE_INCL_ENABLED__

//---------------------
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <map>
#include <cstdlib>
#include <sstream>

// ROOT
#include "TMath.h"

// INCL++
#include "G4INCLParticleTable.hh"
#include "G4INCLGlobals.hh"
#include "DatafilePaths.hh"
#include "G4INCLSignalHandling.hh"

// GENIE
#include "INCLConvertParticle.hh"

// INCL++
#include "G4INCLCascade.hh"
#include "G4INCLConfigEnums.hh"
#include "G4INCLParticle.hh"
#include "G4INCLIAvatar.hh"
#include "G4INCLIPropagationModel.hh"
#include "G4INCLStandardPropagationModel.hh"
#include "G4INCLRandom.hh"
#include "G4INCLRanecu.hh"
#include "G4INCLFinalState.hh"
#include "G4INCLKinematicsUtils.hh"

// GENIE
//#include "HINCLCascade.h"
//#include "INCLCascade.h"
#include "HINCLCascadeIntranuke.h"

// INCL++
#include "G4INCLVersion.hh"
#include "G4INCLUnorderedVector.hh"
#include "G4INCLStore.hh"
#include "G4INCLIAvatar.hh"
#include "G4INCLEventInfo.hh"
#include "G4INCLNucleus.hh"
// signal handler (for Linux and GCC)
#include "G4INCLConfig.hh"
#include "G4INCLVersion.hh"

// GENIE
#include "INCLConfigParser.h"
// INCL++
//#include "ConfigParser.hh"

// signal handler (for Linux and GCC)
#include "G4INCLSignalHandling.hh"

// For I/O (INCL++)
#include "IWriter.hh"
#include "ASCIIWriter.hh"
#include "ProtobufWriter.hh"
#include "INCLTree.hh"
#include "ROOTWriter.hh"
#include "HDF5Writer.hh"

// For configuration

// For logging
#include "G4INCLLogger.hh"

// Generic de-excitation interface
#include "G4INCLIDeExcitation.hh"

// ABLA v3p de-excitation
#ifdef INCL_DEEXCITATION_ABLAXX
#include "G4INCLAblaInterface.hh"
#endif

// ABLA07 de-excitation
#ifdef INCL_DEEXCITATION_ABLA07
#include "G4INCLAbla07Interface.hh"
#endif

// SMM de-excitation
#ifdef INCL_DEEXCITATION_SMM
#include "G4INCLSMMInterface.hh"
#endif

// GEMINIXX de-excitation
#ifdef INCL_DEEXCITATION_GEMINIXX
#include "G4INCLGEMINIXXInterface.hh"
#endif

//#ifdef HAS_BOOST_DATE_TIME
//#include <boost/date_time/posix_time/posix_time.hpp>
//namespace bpt = boost::posix_time;
//#endif

#ifdef HAS_BOOST_TIMER
#include <boost/timer/timer.hpp>
namespace bt = boost::timer;
#endif

// ROOT
#include "TMath.h"
#include "TRootIOCtor.h"
#include "TLorentzVector.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"

// --------------------------------------Include for GENIE---------------------
#include "Framework/Algorithm/AlgConfigPool.h"
#include "Framework/Algorithm/AlgFactory.h"
#include "Framework/Conventions/GBuild.h"
#include "Framework/Conventions/Constants.h"
#include "Framework/Conventions/Controls.h"
//#include "Framework/EventGen/EVGThreadException.h"
#include "Framework/GHEP/GHepFlags.h"
#include "Framework/GHEP/GHepStatus.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Physics/HadronTransport/INukeException.h"
//#include "Physics/HadronTransport/Intranuke2018.h"
//#include "Physics/HadronTransport/HAIntranuke2018.h"
//#include "Physics/HadronTransport/INukeHadroData2018.h"
//#include "Physics/HadronTransport/INukeUtils2018.h"
#include "Framework/Interaction/Interaction.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/Numerical/Spline.h"
#include "Framework/ParticleData/PDGLibrary.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGCodeList.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Utils/PrintUtils.h"
#include "Framework/Utils/StringUtils.h"
#include "Physics/NuclearState/NuclearUtils.h"
#include "Physics/NuclearState/NuclearModelI.h"
#include "Physics/NuclearState/NuclearModelMap.h"

using namespace genie;
using namespace genie::utils;
using namespace genie::constants;
using namespace genie::controls;
using namespace G4INCL;
using std::ostringstream;
using namespace std;


HINCLCascadeIntranuke::HINCLCascadeIntranuke() :
  EventRecordVisitorI("genie::HINCLCascadeIntranuke"),
  theINCLConfig(0), theINCLModel(0), theDeExcitation(0)
{
  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "default ctor";
}

//______________________________________________________________________________
HINCLCascadeIntranuke::HINCLCascadeIntranuke(string config) :
  EventRecordVisitorI("genie::HINCLCascadeIntranuke", config),
  theINCLConfig(0), theINCLModel(0), theDeExcitation(0)
{
  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "ctor from config " << config;
}

//______________________________________________________________________________
HINCLCascadeIntranuke::~HINCLCascadeIntranuke()
{

  // appears Config is owned by model once handed over
 if ( theINCLConfig   ) { theINCLConfig=0;   }
 if ( theINCLModel    ) { delete theINCLModel;    theINCLModel=0;    }
 if ( theDeExcitation ) { delete theDeExcitation; theDeExcitation=0; }

}

//______________________________________________________________________________
void HINCLCascadeIntranuke::LoadConfig(void)
{
  LOG("HINCLCascadeIntranuke", pINFO) << "Settings for INCL++ mode: " ;

#ifdef INCL_SIGNAL_HANDLING
  enableSignalHandling();
#endif

  // appear sConfig is owned by model once handed over
  if ( theINCLConfig   ) { theINCLConfig=0;   }
  if ( theINCLModel    ) { delete theINCLModel;    theINCLModel=0;    }
  if ( theDeExcitation ) { delete theDeExcitation; theDeExcitation=0; }

  INCLConfigParser theParser;

  /* RWH old code from Marc
  // C++ forbids converting string constant to char*
  const char * flags[] = { "NULL",
                           "-pp",
                           "-tFe56",
                           "-N1",
                           "-E1",
                           "-dabla07" };
  int nflags = 6;
  // cast away const-ness for the flags
  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "LoadConfig() create theINCLConfig";
  theINCLConfig = theParser.parse(nflags,(char**)flags);

  */


  size_t maxFlags = 200;
  size_t nflags   = 0;
  char * flags[maxFlags] = { 0 };  // note non-const'ness desired by ConfigParser

  std::string infile;
  GetParamDef( "INCL-infile", infile, std::string("NULL"));
  flags[nflags] = strdup(infile.c_str()); ++nflags;

  std::string pflag;
  GetParamDef( "INCL-pflag",  pflag,  std::string("-pp"));
  flags[nflags] = strdup(pflag.c_str()); ++nflags;

  std::string tflag;
  GetParamDef( "INCL-tflag",  tflag,  std::string("-tFe56"));
  flags[nflags] = strdup(tflag.c_str()); ++nflags;

  std::string Nflag;
  GetParamDef( "INCL-Nflag",  Nflag,  std::string("-N1"));
  flags[nflags] = strdup(Nflag.c_str()); ++nflags;

  std::string Eflag;
  GetParamDef( "INCL-Eflag",  Eflag,  std::string("-E1"));
  flags[nflags] = strdup(Eflag.c_str()); ++nflags;

  std::string dflag;
  GetParamDef( "INCL-dflag",  dflag,  std::string("-dABLA07"));
  flags[nflags] = strdup(dflag.c_str()); ++nflags;

  // arbitary extra flags, need to be tokenized
  std::string extra_incl_flags;
  GetParamDef( "INCL-extra-flags",  extra_incl_flags,  std::string(""));
  std::vector<std::string> extraTokens = genie::utils::str::Split(extra_incl_flags," \t\n");
  for (size_t j=0; j < extraTokens.size(); ++j) {
    std::string& token = extraTokens[j];
    flags[nflags] = strdup(token.c_str()); ++nflags;
  }

  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "LoadConfig() create theINCLConfig";
  theINCLConfig = theParser.parse(nflags,flags);

  // there's currently no way to update *all* of the data paths in the Config
  // after it's been generated, so check whether default file paths "work",
  // add to the flags if necessary, and then regenerate
  bool updateNeeded = AddDataPathFlags(nflags,flags);
  if (updateNeeded) {
    delete theINCLConfig;
    theINCLConfig = theParser.parse(nflags,flags);
  }

  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "doCascade new G4INCL::INCL";
  theINCLModel = new G4INCL::INCL(theINCLConfig);

  // code copied fomr INCL's main/src/INCLCascade.cc
  // with additions for GENIE messenger system
  switch(theINCLConfig->getDeExcitationType()) {
#ifdef INCL_DEEXCITATION_ABLAXX
    case G4INCL::DeExcitationABLAv3p:
      theDeExcitation = new G4INCLAblaInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using ABLAv3p for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_ABLA07
    case G4INCL::DeExcitationABLA07:
      theDeExcitation = new ABLA07CXX::Abla07Interface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using ABLA07CXX for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_SMM
    case G4INCL::DeExcitationSMM:
      theDeExcitation = new SMMCXX::SMMInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using SMMCXX for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_GEMINIXX
    case G4INCL::DeExcitationGEMINIXX:
      theDeExcitation = new G4INCLGEMINIXXInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using GEMINIXX for DeExcitation";
      break;
#endif
    default:
      std::stringstream ss;
      ss << "########################################################\n"
         << "###              WARNING WARNING WARNING             ###\n"
         << "###                                                  ###\n"
         << "### You are running the code without any coupling to ###\n"
         << "###              a de-excitation model!              ###\n"
         << "###    Results will be INCOMPLETE and UNPHYSICAL!    ###\n"
         << "###    Are you sure this is what you want to do?     ###\n"
         << "########################################################\n";
      INCL_INFO(ss.str());
      LOG("HINCLCascadeIntranuke", pWARN)
        << '\n' << ss.str();
        //std::cout << ss.str();
      break;
  }

  // try not to leak strings
  for (size_t i=0; i < nflags; ++i) {
    char * p = flags[i];
    free(p);
    flags[i] = 0;
  }

}

//______________________________________________________________________________
bool HINCLCascadeIntranuke::AddDataPathFlags(size_t& nflags, char** flags) {

  // check if the default INCL path works

  std::stringstream ss;
  for (size_t i=0; i < nflags; ++i) {
    ss << "[" << setw(3) << i << "] " << flags[i] << '\n';
  }
  LOG("HINCLCascadeIntranuke", pNOTICE)
    << "Initial flags passed to INCLConfigParser"
    << '\n' << ss.str();


  /*
#ifdef INCL_DEEXCITATION_ABLAXX
    case G4INCL::DeExcitationABLAv3p:
      theDeExcitation = new G4INCLAblaInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using ABLAv3p for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_ABLA07
    case G4INCL::DeExcitationABLA07:
      theDeExcitation = new ABLA07CXX::Abla07Interface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using ABLA07CXX for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_SMM
    case G4INCL::DeExcitationSMM:
      theDeExcitation = new SMMCXX::SMMInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using SMMCXX for DeExcitation";
      break;
#endif
#ifdef INCL_DEEXCITATION_GEMINIXX
    case G4INCL::DeExcitationGEMINIXX:
      theDeExcitation = new G4INCLGEMINIXXInterface(theINCLConfig);
      LOG("HINCLCascadeIntranuke", pINFO)
        << "using GEMINIXX for DeExcitation";
      break;
#endif
  */

  return false;
}

//______________________________________________________________________________
int HINCLCascadeIntranuke::doCascade(GHepRecord * evrec) const {


  if ( ! theINCLConfig || ! theINCLModel ) return 0;

  int tpos = evrec->TargetNucleusPosition();
  GHepParticle * target = evrec->Particle(tpos);
  GHepParticle * pprobe = evrec->Probe();

  const ParticleType theType = toINCLparticletype(pprobe->Pdg());

  double  E    = (pprobe->E())*1000;
  double massp = G4INCL::ParticleTable::getRealMass(theType);
  double EKin = E - massp;

  G4INCL::ParticleSpecies  theSpecies;
  theSpecies.theType = theType;
  theSpecies.theA    = pdgcpiontoA(pprobe->Pdg());
  theSpecies.theZ    = pdgcpiontoZ(pprobe->Pdg());

  G4INCL::Random::SeedVector const theInitialSeeds = G4INCL::Random::getSeeds();
  GHepStatus_t    ist1  = kIStStableFinalState;
  int pdg_codeProbe = 0;
  // unused // double m_probe(0), m_pnP(0),E_pnP(0), EKinP(0);
  pdg_codeProbe =  INCLpartycleSpecietoPDGCODE(theSpecies);

  G4INCL::EventInfo result;
  result = theINCLModel->processEvent(theSpecies,EKin,target->A(),target->Z());

  // unused // m_probe = ParticleTable::getRealMass(theType);

  double m_target = ParticleTable::getTableMass(result.At, result.Zt);
  GHepParticle * fsProbe = evrec->Probe();

  TLorentzVector p4h   (0.,0.,fsProbe->Pz(),fsProbe->E());
  TLorentzVector x4null(0.,0.,0.,0.);
  TLorentzVector p4tgt (0.,0.,0.,m_target/1000);
  int pdg_codeTarget= genie::pdg::IonPdgCode(target->A(), target->Z());

  if ( result.transparent ) {
    evrec->AddParticle(pdg_codeProbe, ist1, 0,-1,-1,-1, p4h,x4null);
    evrec->AddParticle(pdg_codeTarget,kIStFinalStateNuclearRemnant,1,-1,-1,-1,p4tgt,x4null);
    INCL_DEBUG("Transparent event" << std::endl);
  } else {
    INCL_DEBUG("Number of produced particles: " << result.nParticles << "\n");
    if ( theDeExcitation != 0 ) {
      theDeExcitation->deExcite(&result);
    }
    int mom = 1;
    for (int nP = 0; nP < result.nParticles; nP++){
      if ( nP == result.nParticles-1 ) {
        GHepParticle *p_outR =
          INCLtoGenieParticle(result,nP,kIStFinalStateNuclearRemnant,mom,-1);
        evrec->AddParticle(*p_outR);
      } else {
        GHepParticle *p_outR =
          INCLtoGenieParticle(result,nP,kIStStableFinalState,0,-1);
        evrec->AddParticle(*p_outR);
      }

    }
  }
  return 0;
}

void HINCLCascadeIntranuke::ProcessEventRecord(GHepRecord * evrec) const {

 LOG("HINCLCascadeIntranuke", pNOTICE)
     << "************ Running HINCLCascadeIntranuke MODE INTRANUKE ************";

  fGMode = evrec->EventGenerationMode();
  if ( fGMode == kGMdHadronNucleus ||
       fGMode == kGMdPhotonNucleus    ) {
    HINCLCascadeIntranuke::doCascade(evrec);
  } else if ( fGMode == kGMdLeptonNucleus ) {
    HINCLCascadeIntranuke::TransportHadrons(evrec);
  }

  LOG("HINCLCascadeIntranuke", pINFO) << "Done with this event";
}

bool HINCLCascadeIntranuke::CanRescatter(const GHepParticle * p) const {

  // checks whether a particle that needs to be rescattered, can in fact be
  // rescattered by this cascade MC
  assert(p);
  return  ( p->Pdg() == kPdgPiP     ||
            p->Pdg() == kPdgPiM     ||
            p->Pdg() == kPdgPi0     ||
            p->Pdg() == kPdgProton  ||
            p->Pdg() == kPdgNeutron
            );
}

void HINCLCascadeIntranuke::TransportHadrons(GHepRecord * evrec) const {

  int inucl = -1;
  if ( fGMode == kGMdHadronNucleus ||
       fGMode == kGMdPhotonNucleus    ) {
    inucl = evrec->TargetNucleusPosition();
  } else {
    if ( fGMode == kGMdLeptonNucleus ||
         fGMode == kGMdNucleonDecay  ||
         fGMode == kGMdNeutronOsc       ) {
      inucl = evrec->RemnantNucleusPosition();
    }
  }

  LOG("HINCLCascadeIntranuke", pNOTICE)
    << "Propagating hadrons within nucleus found in position = " << inucl;
  int tpos = evrec->TargetNucleusPosition();
  GHepParticle * target = evrec->Particle(tpos);
  GHepParticle * nucl = evrec->Particle(inucl);
  if ( ! nucl ) {
    LOG("HINCLCascadeIntranuke", pERROR)
      << "No nucleus found in position = " << inucl;
    LOG("HINCLCascadeIntranuke", pERROR)
      << *evrec;
    return;
  }
  fRemnA = nucl->A();
  fRemnZ = nucl->Z();
  int A_f(0), Z_f(0), Aft(0), A_i(target->A()),Z_i(target->Z());
  // unused // Af_Remn(0), Zf_Remn(0);

  LOG("HINCLCascadeIntranuke", pNOTICE)
    << "Nucleus (A,Z) = (" << fRemnA << ", " << fRemnZ << ")";

  const TLorentzVector & p4nucl = *(nucl->P4());
  TLorentzVector x4null(0.,0.,0.,0.);
  fRemnP4 = p4nucl;

  TObjArrayIter piter(evrec);
  GHepParticle * p = 0;

  int icurr = -1;

  bool is_DIS  = evrec->Summary()->ProcInfo().IsDeepInelastic();
  bool is_RES  = evrec->Summary()->ProcInfo().IsResonant();
  // unused // bool is_CCQE = evrec->Summary()->ProcInfo().IsQuasiElastic();

  TLorentzVector * p_4 = nucl->P4();
   // momentum of the remnant nucleus.
  double pxRemn = p_4->Px();
  double pyRemn = p_4->Py();
  double pzRemn = p_4->Pz();
  int pdg_codeTargetRemn= genie::pdg::IonPdgCode(nucl->A(),nucl->Z());
  TLorentzVector p4tgf(p_4->Px(),p_4->Py(),p_4->Pz(),0.0);

   // Loop over GHEP and run intranuclear rescattering on handled particles
  std::vector<G4INCL::EventInfo>ListeOfINCLresult;
  std::vector<int> Postion_evrec;
  GHepParticle * fsl = evrec->FinalStatePrimaryLepton();  // primary Lepton
  double ExcitaionE(0), the_pxRemn(0), the_pyRemn(0), the_pzRemn(0);
  int Zl(0), Aresult(0), Zresult(0), Aexception(0), Zexception(0),
    Pos(0), theA_Remn(0), theZ_Remn(0);

  if ( fsl->Charge() == 0. ) Zl =  0;
  if ( fsl->Charge()  < 0. ) Zl = -1;
  if ( fsl->Charge()  > 0. ) Zl =  1;
  while ( (p = (GHepParticle *) piter.Next() ) ) {
    icurr++;
    // Check whether the particle needs rescattering, otherwise skip it
    if( ! this->NeedsRescattering(p) ) continue;
    GHepParticle * sp = new GHepParticle(*p);

    // Set clone's mom to be the hadron that was cloned
    sp->SetFirstMother(icurr);

    // Check whether the particle can be rescattered
    if ( ! this->CanRescatter(sp) ) {

      // if I can't rescatter it, I will just take it out of the nucleus
      LOG("HINCLCascadeIntranuke", pNOTICE)
        << "... Current version can't rescatter a " << sp->Name();
      sp->SetFirstMother(icurr);
      sp->SetStatus(kIStStableFinalState);
      evrec->AddParticle(*sp);
      delete sp;
      continue; // <-- skip to next GHEP entry
    }

    TLorentzVector *v4= sp->GetX4();

    ThreeVector thePosition(0.,0.,0.);
    ThreeVector momentum (0.,0.,0.);
    thePosition.setX(v4->X());
    thePosition.setY(v4->Y());
    thePosition.setZ(v4->Z());
    TLorentzVector * p4 = sp->P4();

    momentum.setX(p4->Px()*1000);
    momentum.setY(p4->Py()*1000);
    momentum.setZ(p4->Pz()*1000);

    int pdgc = sp->Pdg();

    const ParticleType theType = toINCLparticletype(pdgc);

    if ( theType == G4INCL::UnknownParticle) {
      // INCL++ can't handle the particle
      sp->SetFirstMother(icurr);
      sp->SetStatus(kIStStableFinalState);
      evrec->AddParticle(*sp);
      delete sp;
      continue;
    }

    double  E    = (p4->Energy())*1000;
    double massp = G4INCL::ParticleTable::getRealMass(theType);
    /* unused
    double M4 = momentum.getX()*momentum.getX() +
                momentum.getY()*momentum.getY() +
                momentum.getZ()*momentum.getZ();
    */
    double EKin = E - massp;

    G4INCL::ParticleSpecies  theSpecies;
    theSpecies.theType=theType;
    theSpecies.theA=pdgcpiontoA(sp->Pdg());
    theSpecies.theZ=pdgcpiontoZ(sp->Pdg());


    G4INCL::Particle *pincl =
      new G4INCL::Particle( theType , E , momentum, thePosition);

    //***********************************************************************

    G4INCL::Random::SeedVector const theInitialSeeds =
                                            G4INCL::Random::getSeeds();
    // unused // int i = 0;
    G4INCL::Random::saveSeeds();
    G4INCL::EventInfo result;
    result=theINCLModel->processEvent(theSpecies,pincl,EKin,fRemnA,fRemnZ);

    //if result  give a transparent event FSI=1
    // Store *sp
    // Exception get remnant with Z and A <0
    Aresult += (fRemnA + pdgcpiontoA(sp->Pdg())- result.ARem[0]);
    Zresult += (fRemnZ + pdgcpiontoZ(sp->Pdg())- result.ZRem[0]);
    Aexception = A_i - Aresult;
    Zexception = Z_i - Zresult;
    if ( Zexception <= 0 || Aexception <= 0 ) {
      // Make sure that ARemn and Zremn >0
      sp->SetFirstMother(icurr);
      sp->SetStatus(kIStStableFinalState);
      evrec->AddParticle(*sp);
      delete sp;
      int nP(0);
      for (size_t it=0; it < ListeOfINCLresult.size(); it++) {
        if ( theDeExcitation != 0 ) {
          theDeExcitation->deExcite(&ListeOfINCLresult.at(it));
        }
        Pos = Postion_evrec.at(it);
        if ( nP<ListeOfINCLresult.at(it).nParticles ) {
          GHepParticle *p_outR =
            INCLtoGenieParticle(ListeOfINCLresult.at(it),nP,kIStStableFinalState,Pos,inucl);
          evrec->AddParticle(*p_outR);
          nP++;
        }
      }
      ListeOfINCLresult.clear();
    } else {
      if ( result.transparent == true ) {
        Zl+=pdgcpiontoZ(sp->Pdg());
        Aft+=pdgcpiontoA(sp->Pdg());
        sp->SetFirstMother(icurr);
        sp->SetStatus(kIStStableFinalState);
        evrec->AddParticle(*sp);
        delete sp;
      } else {
        Postion_evrec.push_back(icurr);
        ListeOfINCLresult.push_back(result);
      }

    }
    delete pincl; // rwh added
  } //Ghep-entry

  if ( ListeOfINCLresult.size() != 0 ) {
    for (size_t it=0; it < ListeOfINCLresult.size(); it++) {
      Pos = Postion_evrec.at(it);  // Position of the mother in evrec
      GHepParticle * pinthenucleus = evrec->Particle(Pos);
      theA_Remn += (fRemnA + pdgcpiontoA(pinthenucleus->Pdg())- ListeOfINCLresult.at(it).ARem[0]);
      theZ_Remn += (fRemnZ + pdgcpiontoZ(pinthenucleus->Pdg())- ListeOfINCLresult.at(it).ZRem[0]);
      the_pxRemn += ListeOfINCLresult.at(it).pxRem[0];
      the_pyRemn += ListeOfINCLresult.at(it).pyRem[0];
      the_pzRemn += ListeOfINCLresult.at(it).pzRem[0];

      ExcitaionE += ListeOfINCLresult.at(it).EStarRem[0];

      if ( is_DIS == false && is_RES == false ) {
        // QE - event
        ListeOfINCLresult.at(it).pxRem[0] += pxRemn*1000;
        ListeOfINCLresult.at(it).pyRem[0] += pyRemn*1000;
        ListeOfINCLresult.at(it).pzRem[0] += 1000*pzRemn;
        if ( theDeExcitation != 0 ) {
          theDeExcitation->deExcite(&ListeOfINCLresult.at(it));
        }

        for (int nP=0; nP < ListeOfINCLresult.at(it).nParticles; nP++ ) {
          GHepParticle *p_out =INCLtoGenieParticle(ListeOfINCLresult.at(it),nP,kIStStableFinalState,Pos,inucl);
          evrec->AddParticle(*p_out);
        }// Add to evrec the result
      }
      if ( is_RES || is_DIS ) {
        if( it < ListeOfINCLresult.size()-1 ) {
          /*if(ListeOfINCLresult.at(it).nParticles==0){
          // Needed to avoid segmfault (pointer to daugther always NULL add protection to this in gntpc app)
          TLorentzVector gammad(0.,0.,0.,0.);
          evrec->AddParticle(22, kIStStableFinalState, Pos, inucl,-1,-1,gammad,x4null);
          }*/
          for (int nP=0; nP < ListeOfINCLresult.at(it).nParticles; nP++ ) {
            A_f+=ListeOfINCLresult.at(it).A[nP];
            Z_f+=ListeOfINCLresult.at(it).Z[nP];
            GHepParticle *p_outD =
              INCLtoGenieParticle(ListeOfINCLresult.at(it),nP,kIStStableFinalState,Pos,inucl);
            evrec->AddParticle(*p_outD);
          } //Add result without the remnant nucleus
        } else {
          ListeOfINCLresult.at(it).ARem[0]=A_i-theA_Remn-Aft;
          ListeOfINCLresult.at(it).ZRem[0]=Z_i-theZ_Remn-Zl;
          ListeOfINCLresult.at(it).pxRem[0]= the_pxRemn + (pxRemn*1000);
          ListeOfINCLresult.at(it).pyRem[0]= the_pyRemn + (pyRemn*1000);
          ListeOfINCLresult.at(it).pzRem[0]= the_pzRemn + (1000*pzRemn);
          ListeOfINCLresult.at(it).EStarRem[0]=ExcitaionE;
          if ( theDeExcitation != 0 ) {
            theDeExcitation->deExcite(&ListeOfINCLresult.at(it));
          }
          for (int nP=0; nP < ListeOfINCLresult.at(it).nParticles; nP++ ) {
            A_f+=ListeOfINCLresult.at(it).A[nP];
            Z_f+=ListeOfINCLresult.at(it).Z[nP];

            GHepParticle *p_outR = INCLtoGenieParticle(ListeOfINCLresult.at(it),nP,kIStStableFinalState,Pos,inucl);
            evrec->AddParticle(*p_outR);
            //}
          } //Add all the result with the correct remnant nucleus
        }
      }
    }// Loop over all the result from INCLCascade
  }
  // rwh // delete pincl;

  if ( ListeOfINCLresult.size() == 0 ) {
    GHepParticle remnant(pdg_codeTargetRemn, kIStFinalStateNuclearRemnant, inucl,-1,-1,-1, fRemnP4, x4null);
    evrec->AddParticle(remnant);
  }
  evrec->Particle(inucl)->SetStatus(kIStIntermediateState);
}

//______________________________________________________________________________
int HINCLCascadeIntranuke::pdgcpiontoA(int pdgc) const {
  if      ( pdgc == 2212 || pdgc == 2112 ) return 1;
  else if ( pdgc == 211|| pdgc == -211 || pdgc == 111 ) return 0;
  return 0;  // rwh return something
}

//______________________________________________________________________________
int HINCLCascadeIntranuke::pdgcpiontoZ(int pdgc) const {
  if      ( pdgc == 2212 || pdgc == 211 ) return 1;
  else if ( pdgc == 2112 || pdgc == 111 ) return 0;
  else if ( pdgc == -211 ) return -1;
  return 0; // rwh return something
}

//______________________________________________________________________________
bool HINCLCascadeIntranuke::NeedsRescattering(const GHepParticle * p) const {
  // checks whether the particle should be rescattered
  assert(p);
  // attempt to rescatter anything marked as 'hadron in the nucleus'
  return ( p->Status() == kIStHadronInTheNucleus );
}

//______________________________________________________________________________
void HINCLCascadeIntranuke::Configure(const Registry & config) {

  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "Configure from Registry: '" << config.Name() << "'\n"
    << config;

  Algorithm::Configure(config);
  this->LoadConfig();
}

//___________________________________________________________________________
void HINCLCascadeIntranuke::Configure(string param_set) {

  LOG("HINCLCascadeIntranuke", pDEBUG)
    << "Configure from param_set name: " << param_set;

  Algorithm::Configure(param_set);
  this->LoadConfig();
}

#endif // __GENIE_INCL_ENABLED__
