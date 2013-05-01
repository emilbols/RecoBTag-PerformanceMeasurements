#include "RecoBTag/PerformanceMeasurements/interface/BTagAnalyzer.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"

PFJetIDSelectionFunctor pfjetIDLoose( PFJetIDSelectionFunctor::FIRSTDATA, PFJetIDSelectionFunctor::LOOSE );
PFJetIDSelectionFunctor pfjetIDTight( PFJetIDSelectionFunctor::FIRSTDATA, PFJetIDSelectionFunctor::TIGHT );

pat::strbitset retpf = pfjetIDLoose.getBitTemplate();

BTagAnalyzer::BTagAnalyzer(const edm::ParameterSet& iConfig):
  classifier_(iConfig) ,
  pv(0),
  PFJet80(0),
  thelepton1(0.,0.,0.,0.),
  thelepton2(0.,0.,0.,0.),
  computer(0),
  nsubjettinessCalculator(Njettiness::onepass_kt_axes, NsubParameters(1.0, 0.8, 0.8)),
  cap0(0),
  cap1(0),
  cap2(0),
  cap3(0),
  cap4(0),
  cap5(0),
  cap6(0),
  cap7(0),
  cap8(0),
  can0(0),
  can1(0),
  can2(0),
  can3(0),
  can4(0),
  can5(0),
  can6(0),
  can7(0),
  can8(0)
{
  //now do what ever initialization you need
  cout << "construct BTagAnalyzer " << endl;

  // Parameters
  runSubJets_ = iConfig.getParameter<bool>("runSubJets");
  minJetPt_  = iConfig.getParameter<double>("MinPt");
  maxJetEta_ = iConfig.getParameter<double>("MaxEta");

  selTagger_ = iConfig.getParameter<int>("selTagger");

  use_selected_tracks_ = iConfig.getParameter<bool> ("use_selected_tracks");
  useTrackHistory_     = iConfig.getParameter<bool> ("useTrackHistory");
  produceJetProbaTree_ = iConfig.getParameter<bool> ("produceJetProbaTree");
  producePtRelTemplate_ = iConfig.getParameter<bool>("producePtRelTemplate");

  use_ttbar_filter_     = iConfig.getParameter<bool> ("use_ttbar_filter");
  channel_      = iConfig.getParameter<edm::InputTag> ("channel");

  // Modules
  primaryVertexColl_   = iConfig.getParameter<edm::InputTag>("primaryVertexColl");

  JetCollectionTag_ = iConfig.getParameter<edm::InputTag>("Jets");
  if (runSubJets_) FatJetCollectionTag_ = iConfig.getParameter<edm::InputTag>("FatJets");

  trackCHEBJetTags_    = iConfig.getParameter<std::string>("trackCHEBJetTags");
  trackCNegHEBJetTags_ = iConfig.getParameter<std::string>("trackCNegHEBJetTags");

  trackCHPBJetTags_    = iConfig.getParameter<std::string>("trackCHPBJetTags");
  trackCNegHPBJetTags_ = iConfig.getParameter<std::string>("trackCNegHPBJetTags");

  jetPBJetTags_        = iConfig.getParameter<std::string>("jetPBJetTags");
  jetPNegBJetTags_     = iConfig.getParameter<std::string>("jetPNegBJetTags");
  jetPPosBJetTags_     = iConfig.getParameter<std::string>("jetPPosBJetTags");

  jetBPBJetTags_        = iConfig.getParameter<std::string>("jetBPBJetTags");
  jetBPNegBJetTags_     = iConfig.getParameter<std::string>("jetBPNegBJetTags");
  jetBPPosBJetTags_     = iConfig.getParameter<std::string>("jetBPPosBJetTags");

  simpleSVHighEffBJetTags_      = iConfig.getParameter<std::string>("simpleSVHighEffBJetTags");
  simpleSVNegHighEffBJetTags_   = iConfig.getParameter<std::string>("simpleSVNegHighEffBJetTags");
  simpleSVHighPurBJetTags_      = iConfig.getParameter<std::string>("simpleSVHighPurBJetTags");
  simpleSVNegHighPurBJetTags_   = iConfig.getParameter<std::string>("simpleSVNegHighPurBJetTags");

  combinedSVBJetTags_     = iConfig.getParameter<std::string>("combinedSVBJetTags");
  combinedSVNegBJetTags_  = iConfig.getParameter<std::string>("combinedSVNegBJetTags");
  combinedSVPosBJetTags_  = iConfig.getParameter<std::string>("combinedSVPosBJetTags");

  combinedSVRetrainedBJetTags_     = iConfig.getParameter<std::string>("combinedSVRetrainedBJetTags");
  combinedSVRetrainedNegBJetTags_  = iConfig.getParameter<std::string>("combinedSVRetrainedNegBJetTags");
  combinedSVRetrainedPosBJetTags_  = iConfig.getParameter<std::string>("combinedSVRetrainedPosBJetTags");

  combinedCSVJPBJetTags_     = iConfig.getParameter<std::string>("combinedCSVJPBJetTags");
  combinedCSVJPNegBJetTags_  = iConfig.getParameter<std::string>("combinedCSVJPNegBJetTags");
  combinedCSVJPPosBJetTags_  = iConfig.getParameter<std::string>("combinedCSVJPPosBJetTags");

  combinedCSVSLBJetTags_     = iConfig.getParameter<std::string>("combinedCSVSLBJetTags");
  combinedCSVSLNegBJetTags_  = iConfig.getParameter<std::string>("combinedCSVSLNegBJetTags");
  combinedCSVSLPosBJetTags_  = iConfig.getParameter<std::string>("combinedCSVSLPosBJetTags");

  combinedCSVJPSLBJetTags_     = iConfig.getParameter<std::string>("combinedCSVJPSLBJetTags");
  combinedCSVJPSLNegBJetTags_  = iConfig.getParameter<std::string>("combinedCSVJPSLNegBJetTags");
  combinedCSVJPSLPosBJetTags_  = iConfig.getParameter<std::string>("combinedCSVJPSLPosBJetTags");

  simpleIVFSVHighPurBJetTags_ = iConfig.getParameter<std::string>("simpleIVFSVHighPurBJetTags");
  simpleIVFSVHighEffBJetTags_ = iConfig.getParameter<std::string>("simpleIVFSVHighEffBJetTags");
  doubleIVFSVHighEffBJetTags_ = iConfig.getParameter<std::string>("doubleIVFSVHighEffBJetTags");
  combinedIVFSVBJetTags_      = iConfig.getParameter<std::string>("combinedIVFSVBJetTags");
  combinedIVFSVPosBJetTags_   = iConfig.getParameter<std::string>("combinedIVFSVPosBJetTags");

  //softMuonBJetTags_       = iConfig.getParameter<std::string>("softMuonBJetTags");
  //softMuonNegBJetTags_    = iConfig.getParameter<std::string>("softMuonNegBJetTags");
  //softMuonTagInfoName_    = iConfig.getParameter<std::string>("softMuonTagInfoName");

  softPFMuonBJetTags_       = iConfig.getParameter<std::string>("softPFMuonBJetTags");
  softPFMuonNegBJetTags_    = iConfig.getParameter<std::string>("softPFMuonNegBJetTags");
  softPFMuonPosBJetTags_    = iConfig.getParameter<std::string>("softPFMuonPosBJetTags");

  softPFElectronBJetTags_       = iConfig.getParameter<std::string>("softPFElectronBJetTags");
  softPFElectronNegBJetTags_    = iConfig.getParameter<std::string>("softPFElectronNegBJetTags");
  softPFElectronPosBJetTags_    = iConfig.getParameter<std::string>("softPFElectronPosBJetTags");

  softPFMuonTagInfos_      = iConfig.getParameter<std::string>("softPFMuonTagInfos");
  softPFElectronTagInfos_  = iConfig.getParameter<std::string>("softPFElectronTagInfos");

  muonCollectionName_       = iConfig.getParameter<edm::InputTag>("muonCollectionName");

  triggerTable_             = iConfig.getParameter<edm::InputTag>("triggerTable");

  SVComputer_               = iConfig.getParameter<edm::InputTag>( "svComputer");

  ///////////////
  // Some Histograms

  hData_All_NJets       = fs->make<TH1F>("hData_All_NJets","nb. of jets"     ,21, -0.5, 20.5);
  hData_All_NTracks     = fs->make<TH1F>("hData_All_NTracks","nb. of tracks" ,20,  0.5, 25.5);
  hData_All_JetPt       = fs->make<TH1F>("hData_All_JetPt","pt(jet)"         ,50,  20., 520.);
  hData_All_JetEta      = fs->make<TH1F>("hData_All_JetEta","|#eta(jet)|"    ,24,   0., 2.5 );
  hData_NJets           = fs->make<TH1F>("hData_NJets","nb. of jets"         ,21, -0.5, 20.5);
  hData_NTracks         = fs->make<TH1F>("hData_NTracks","nb. of tracks"     ,20,  0.5, 25.5);
  hData_JetPt           = fs->make<TH1F>("hData_JetPt","pt(jet)"             ,50,  20., 520.);
  hData_JetEta          = fs->make<TH1F>("hData_JetEta","|#eta(jet)|"        ,24,  0. , 2.5 );

  hData_Tagger  	= fs->make<TH1F>("hData_Tagger","Tagger"             ,100,-25.,25.);
  hData_Tagger_TCHE	= fs->make<TH1F>("hData_Tagger_TCHE","Tagger_TCHE"   ,100,-25.,25.);
  hData_Tagger_TCHP	= fs->make<TH1F>("hData_Tagger_TCHP","Tagger_TCHP"   ,100,-25.,25.);
  hData_Tagger_JP	= fs->make<TH1F>("hData_Tagger_JP","Tagger_JP"       ,100,-25.,25.);
  hData_Tagger_SSVHE	= fs->make<TH1F>("hData_Tagger_SSVHE ","Tagger_SSVHE",100,-25.,25.);
  hData_Tagger_SSVHP	= fs->make<TH1F>("hData_Tagger_SSVHP","Tagger_SSVHP" ,100,-25.,25.);
  hData_Tagger_CSV	= fs->make<TH1F>("hData_Tagger_CSV","Tagger_CSV"     ,100,-25.,25.);
  hData_Tagger_MU	= fs->make<TH1F>("hData_Tagger_MU","Tagger_MU"       ,100,-25.,25.);

  hAllFlav_Flavour      = fs->make<TH1F>("hAllFlav_Flavour","Flavour"   ,22,-0.5,21.5);
  hAllFlav_Tagger	= fs->make<TH1F>("hAllFlav_Tagger","Tagger"     ,100,-25.,25.);
  hAllFlav_Tagger_Gam	= fs->make<TH1F>("hAllFlav_Tagger_Gam","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_K0s	= fs->make<TH1F>("hAllFlav_Tagger_K0s","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Lam	= fs->make<TH1F>("hAllFlav_Tagger_Lam","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Bwd	= fs->make<TH1F>("hAllFlav_Tagger_Bwd","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Cwd	= fs->make<TH1F>("hAllFlav_Tagger_Cwd","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Tau	= fs->make<TH1F>("hAllFlav_Tagger_Tau","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Int	= fs->make<TH1F>("hAllFlav_Tagger_Int","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Fak	= fs->make<TH1F>("hAllFlav_Tagger_Fak","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Bad	= fs->make<TH1F>("hAllFlav_Tagger_Bad","Tagger" ,100,-25.,25.);
  hAllFlav_Tagger_Oth	= fs->make<TH1F>("hAllFlav_Tagger_Oth","Tagger" ,100,-25.,25.);

  hLightFlav_Tagger	= fs->make<TH1F>("hLightFlav_Tagger","Tagger",100,-25.,25.);
  hGluonFlav_Tagger	= fs->make<TH1F>("hGluonFlav_Tagger","Tagger",100,-25.,25.);
  hUDSFlav_Tagger	= fs->make<TH1F>("hUDSFlav_Tagger","Tagger"  ,100,-25.,25.);
  hCFlav_Tagger 	= fs->make<TH1F>("hCFlav_Tagger","Tagger"    ,100,-25.,25.);
  hBFlav_Tagger 	= fs->make<TH1F>("hBFlav_Tagger","Tagger"    ,100,-25.,25.);

  IPSign_cat0		= fs->make<TH1F>("IPSign_cat0","-IP/#sigma",50, 0., 25.);
  IPSign_cat1		= fs->make<TH1F>("IPSign_cat1","-IP/#sigma",50, 0., 25.);
  IPSign_cat2		= fs->make<TH1F>("IPSign_cat2","-IP/#sigma",50, 0., 25.);
  IPSign_cat3		= fs->make<TH1F>("IPSign_cat3","-IP/#sigma",50, 0., 25.);
  IPSign_cat4		= fs->make<TH1F>("IPSign_cat4","-IP/#sigma",50, 0., 25.);
  IPSign_cat5		= fs->make<TH1F>("IPSign_cat5","-IP/#sigma",50, 0., 25.);
  IPSign_cat6		= fs->make<TH1F>("IPSign_cat6","-IP/#sigma",50, 0., 25.);
  IPSign_cat7		= fs->make<TH1F>("IPSign_cat7","-IP/#sigma",50, 0., 25.);
  IPSign_cat8		= fs->make<TH1F>("IPSign_cat8","-IP/#sigma",50, 0., 25.);
  IPSign_cat9		= fs->make<TH1F>("IPSign_cat9","-IP/#sigma",50, 0., 25.);
  TrackProbaNeg 	= fs->make<TH1F>("TrackProbaNEG","TrackProbaNEG",51, 0., 1.02);
  TrackProbaNeg_Cat0	= fs->make<TH1F>("TrackProbaNEG Cat0","TrackProbaNEG Cat0",51, 0., 1.02);
  TrackProbaNeg_Cat1	= fs->make<TH1F>("TrackProbaNEG Cat1","TrackProbaNEG Cat1",51, 0., 1.02);
  TrackProbaNeg_Cat2	= fs->make<TH1F>("TrackProbaNEG Cat2","TrackProbaNEG Cat2",51, 0., 1.02);
  TrackProbaNeg_Cat3	= fs->make<TH1F>("TrackProbaNEG Cat3","TrackProbaNEG Cat3",51, 0., 1.02);
  TrackProbaNeg_Cat4	= fs->make<TH1F>("TrackProbaNEG Cat4","TrackProbaNEG Cat4",51, 0., 1.02);
  TrackProbaNeg_Cat5	= fs->make<TH1F>("TrackProbaNEG Cat5","TrackProbaNEG Cat5",51, 0., 1.02);
  TrackProbaNeg_Cat6	= fs->make<TH1F>("TrackProbaNEG Cat6","TrackProbaNEG Cat6",51, 0., 1.02);
  TrackProbaNeg_Cat7	= fs->make<TH1F>("TrackProbaNEG Cat7","TrackProbaNEG Cat7",51, 0., 1.02);
  TrackProbaNeg_Cat8	= fs->make<TH1F>("TrackProbaNEG Cat8","TrackProbaNEG Cat8",51, 0., 1.02);
  TrackProbaNeg_Cat9	= fs->make<TH1F>("TrackProbaNEG Cat9","TrackProbaNEG Cat9",51, 0., 1.02);
  TrackProbJet80	= fs->make<TH1F>("TrackProbJet80","TrackProbJet80",51, 0., 1.02);
  TrackProbJet80_Cat0	= fs->make<TH1F>("TrackProbJet80 Cat0","TrackProbJet80 Cat0",51, 0., 1.02);
  TrackProbJet80_Cat1	= fs->make<TH1F>("TrackProbJet80 Cat1","TrackProbJet80 Cat1",51, 0., 1.02);
  TrackProbJet80_Cat2	= fs->make<TH1F>("TrackProbJet80 Cat2","TrackProbJet80 Cat2",51, 0., 1.02);
  TrackProbJet80_Cat3	= fs->make<TH1F>("TrackProbJet80 Cat3","TrackProbJet80 Cat3",51, 0., 1.02);
  TrackProbJet80_Cat4	= fs->make<TH1F>("TrackProbJet80 Cat4","TrackProbJet80 Cat4",51, 0., 1.02);
  TrackProbJet80_Cat5	= fs->make<TH1F>("TrackProbJet80 Cat5","TrackProbJet80 Cat5",51, 0., 1.02);
  TrackProbJet80_Cat6	= fs->make<TH1F>("TrackProbJet80 Cat6","TrackProbJet80 Cat6",51, 0., 1.02);
  TrackProbJet80_Cat7	= fs->make<TH1F>("TrackProbJet80 Cat7","TrackProbJet80 Cat7",51, 0., 1.02);
  TrackProbJet80_Cat8	= fs->make<TH1F>("TrackProbJet80 Cat8","TrackProbJet80 Cat8",51, 0., 1.02);
  TrackProbJet80_Cat9	= fs->make<TH1F>("TrackProbJet80 Cat9","TrackProbJet80 Cat9",51, 0., 1.02);

  ///////////////
  // TTree

  smalltree = fs->make<TTree>("ttree", "ttree");

  smalltree->Branch("BitTrigger",&BitTrigger,"BitTrigger/I");
  smalltree->Branch("Run"       ,&Run       ,"Run/I");
  smalltree->Branch("Evt"       ,&Evt       ,"Evt/I");
  smalltree->Branch("LumiBlock" ,&LumiBlock ,"LumiBlock/I");
  smalltree->Branch("nPV"       ,&nPV       ,"nPV/I");
  smalltree->Branch("PVz"       ,&PVz       ,"PVz/F");
  smalltree->Branch("pthat"     ,&pthat     ,"pthat/F");
  smalltree->Branch("PVzSim"    ,&PVzSim    ,"PVzSim/F");

  smalltree->Branch("nPUtrue", &nPUtrue, "nPUtrue/I");
  smalltree->Branch("nPU"    , &nPU,     "nPU/I"    );

  //   smalltree->Branch("PU_bunch",      PU_bunch,      "PU_bunch[nPU]/I");
  //   smalltree->Branch("PU_z",          PU_z,          "PU_z[nPU]/F");
  //   smalltree->Branch("PU_sumpT_low",  PU_sumpT_low , "PU_sumpT_low[nPU]/F");
  //   smalltree->Branch("PU_sumpT_high", PU_sumpT_high, "PU_sumpT_high[nPU]/F");
  //   smalltree->Branch("PU_ntrks_low",  PU_ntrks_low , "PU_ntrks_low[nPU]/I");
  //   smalltree->Branch("PU_ntrks_high", PU_ntrks_high, "PU_ntrks_high[nPU]/I");

  if (use_ttbar_filter_) {
    smalltree->Branch("ttbar_chan",  &ttbar_chan,  "ttbar_chan/I");
    smalltree->Branch("lepton1_pT",  &lepton1_pT,  "lepton1_pT/F");
    smalltree->Branch("lepton1_eta", &lepton1_eta, "lepton1_eta/F");
    smalltree->Branch("lepton1_phi", &lepton1_phi, "lepton1_phi/F");
    smalltree->Branch("lepton2_pT",  &lepton2_pT,  "lepton2_pT/F");
    smalltree->Branch("lepton2_eta", &lepton2_eta, "lepton2_eta/F");
    smalltree->Branch("lepton2_phi", &lepton2_phi, "lepton2_phi/F");
    smalltree->Branch("met", &met, "met/F");
    smalltree->Branch("mll", &mll, "mll/F");
    smalltree->Branch("trig_ttbar", &trig_ttbar, "trig_ttbar/I");
  }

  if ( produceJetProbaTree_ ) {

    //--------------------------------------
    // track information
    //--------------------------------------
    smalltree->Branch("nTrack",	       &nTrack, 	    "nTrack/I");
    smalltree->Branch("Track_dxy",     Track_dxy,	    "Track_dxy[nTrack]/F");
    smalltree->Branch("Track_dz",      Track_dz,	    "Track_dz[nTrack]/F");
    smalltree->Branch("Track_zIP",     Track_zIP,	    "Track_zIP[nTrack]/F");
    smalltree->Branch("Track_length",  Track_length,  "Track_length[nTrack]/F");
    smalltree->Branch("Track_dist",    Track_dist,	  "Track_dist[nTrack]/F");
    smalltree->Branch("Track_IP2D",    Track_IP2D,	  "Track_IP2D[nTrack]/F");
    smalltree->Branch("Track_IP2Dsig", Track_IP2Dsig,	"Track_IP2Dsig[nTrack]/F");
    smalltree->Branch("Track_IP2Derr", Track_IP2Derr,	"Track_IP2Derr[nTrack]/F");

    smalltree->Branch("Track_IP",        Track_IP,	  "Track_IP[nTrack]/F");
    smalltree->Branch("Track_IPsig",     Track_IPsig,	"Track_IPsig[nTrack]/F");
    smalltree->Branch("Track_IPerr",     Track_IPerr,	"Track_IPerr[nTrack]/F");

    smalltree->Branch("Track_Proba",     Track_Proba,	  "Track_[nTrack]Proba/F");
    smalltree->Branch("Track_p",         Track_p,	      "Track_p[nTrack]/F");
    smalltree->Branch("Track_pt",        Track_pt,	    "Track_pt[nTrack]/F");
    smalltree->Branch("Track_eta",       Track_eta,	    "Track_eta[nTrack]/F");
    smalltree->Branch("Track_phi",       Track_phi,	    "Track_phi[nTrack]/F");
    smalltree->Branch("Track_chi2",      Track_chi2,	  "Track_chi2[nTrack]/F");
    smalltree->Branch("Track_charge",    Track_charge,  "Track_charge[nTrack]/I");
    smalltree->Branch("Track_history",   Track_history,	"Track_history[nTrack]/I");

    smalltree->Branch("Track_nHitStrip", Track_nHitStrip, "Track_nHitStrip[nTrack]/I");
    smalltree->Branch("Track_nHitPixel", Track_nHitPixel, "Track_nHitPixel[nTrack]/I");
    smalltree->Branch("Track_nHitAll",   Track_nHitAll,	  "Track_nHitAll[nTrack]/I");
    smalltree->Branch("Track_nHitTIB",   Track_nHitTIB,	  "Track_nHitTIB[nTrack]/I");
    smalltree->Branch("Track_nHitTID",   Track_nHitTID,	  "Track_nHitTID[nTrack]/I");
    smalltree->Branch("Track_nHitTOB",   Track_nHitTOB,	  "Track_nHitTOB[nTrack]/I");
    smalltree->Branch("Track_nHitTEC",   Track_nHitTEC,	  "Track_nHitTEC[nTrack]/I");
    smalltree->Branch("Track_nHitPXB",   Track_nHitPXB,	  "Track_nHitPXB[nTrack]/I");
    smalltree->Branch("Track_nHitPXF",   Track_nHitPXF,	  "Track_nHitPXF[nTrack]/I");
    smalltree->Branch("Track_isHitL1",   Track_isHitL1,	  "Track_isHitL1[nTrack]/I");

    smalltree->Branch("Track_PV",        Track_PV,        "Track_PV[nTrack]/I");
    smalltree->Branch("Track_SV",        Track_SV,        "Track_SV[nTrack]/I");
    smalltree->Branch("Track_PVweight",  Track_PVweight,  "Track_PVweight[nTrack]/F");
    smalltree->Branch("Track_SVweight",  Track_SVweight,  "Track_SVweight[nTrack]/F");

    smalltree->Branch("Track_isfromSV",  Track_isfromSV  ,"Track_isfromSV[nTrack]/I");

    smalltree->Branch("Track_category",  Track_category,  "Track_category[nTrack]/I");

    //--------------------------------------
    // primary vertex information
    //--------------------------------------
    smalltree->Branch("PV_x"	   ,PV_x	 ,"PV_x[nPV]/F");
    smalltree->Branch("PV_y"	   ,PV_y	 ,"PV_y[nPV]/F");
    smalltree->Branch("PV_z"	   ,PV_z	 ,"PV_z[nPV]/F");
    smalltree->Branch("PV_ex"	   ,PV_ex	 ,"PV_ex[nPV]/F");
    smalltree->Branch("PV_ey"	   ,PV_ey	 ,"PV_ey[nPV]/F");
    smalltree->Branch("PV_ez"	   ,PV_ez	 ,"PV_ez[nPV]/F");
    smalltree->Branch("PV_chi2"	   ,PV_chi2	 ,"PV_chi2[nPV]/F");
    smalltree->Branch("PV_ndf"	   ,PV_ndf	 ,"PV_ndf[nPV]/F");
    smalltree->Branch("PV_isgood"    ,PV_isgood	 ,"PV_isgood[nPV]/I");
    smalltree->Branch("PV_isfake"    ,PV_isfake	 ,"PV_isfake[nPV]/I");

    //--------------------------------------
    // secondary vertex information
    //--------------------------------------
    smalltree->Branch("nSV"	        , &nSV	           ,"nSV/I");
    smalltree->Branch("SV_x"	        , SV_x	           ,"SV_x[nSV]/F");
    smalltree->Branch("SV_y"	        , SV_y	           ,"SV_y[nSV]/F");
    smalltree->Branch("SV_z"	        , SV_z	           ,"SV_z[nSV]/F");
    smalltree->Branch("SV_ex"	        , SV_ex	           ,"SV_ex[nSV]/F");
    smalltree->Branch("SV_ey"	        , SV_ey	           ,"SV_ey[nSV]/F");
    smalltree->Branch("SV_ez"	        , SV_ez	           ,"SV_ez[nSV]/F");
    smalltree->Branch("SV_chi2"	        , SV_chi2             ,"SV_chi2[nSV]/F");
    smalltree->Branch("SV_ndf"	        , SV_ndf	      ,"SV_ndf[nSV]/F");
    smalltree->Branch("SV_flight"         , SV_flight         ,"SV_flight[nSV]/F");
    smalltree->Branch("SV_flightErr"      , SV_flightErr      ,"SV_flightErr[nSV]/F");
    smalltree->Branch("SV_deltaR_jet"     , SV_deltaR_jet     ,"SV_deltaR_jet[nSV]/F");
    smalltree->Branch("SV_deltaR_sum_jet" , SV_deltaR_sum_jet ,"SV_deltaR_sum_jet[nSV]/F");
    smalltree->Branch("SV_deltaR_sum_dir" , SV_deltaR_sum_dir ,"SV_deltaR_sum_dir[nSV]/F");
    smalltree->Branch("SV_energy_ratio"	  , SV_energy_ratio   ,"SV_energy_ratio[nSV]/F");
    smalltree->Branch("SV_aboveC"	  , SV_aboveC	      ,"SV_aboveC[nSV]/F");
    smalltree->Branch("SV_vtx_pt"	  , SV_vtx_pt	      ,"SV_vtx_pt[nSV]/F");
    smalltree->Branch("SV_flight2D"       , SV_flight2D       ,"SV_flight2D[nSV]/F");
    smalltree->Branch("SV_flight2DErr"    , SV_flight2DErr    ,"SV_flight2DErr[nSV]/F");
    smalltree->Branch("SV_totCharge"      , SV_totCharge      ,"SV_totCharge [nSV]/F");
    smalltree->Branch("SV_vtxDistJetAxis" , SV_vtxDistJetAxis ,"SV_vtxDistJetAxis [nSV]/F");
    smalltree->Branch("SV_nTrk"           , SV_nTrk           ,"SV_nTrk[nSV]/I");
    smalltree->Branch("SV_nTrk_firstVxt"  , SV_nTrk_firstVxt  ,"SV_nTrk_firstVxt[nSV]/I");
    smalltree->Branch("SV_mass"           , SV_mass           ,"SV_mass[nSV]/F");
    smalltree->Branch("SV_vtx_eta"	  , SV_vtx_eta        ,"SV_vtx_eta[nSV]/F");
    smalltree->Branch("SV_vtx_phi"	  , SV_vtx_phi        ,"SV_vtx_phi[nSV]/F");
  }

  //--------------------------------------
  // jet information
  //--------------------------------------
  JetInfo[0].RegisterTree(smalltree,"JetInfo");
  if (runSubJets_) JetInfo[1].RegisterTree(smalltree,"FatJetInfo");

  //// smalltree->Branch("Jet_pt",          Jet_pt	       ,"Jet_pt[nJet]/F");
  //// smalltree->Branch("Jet_genpt",       Jet_genpt       ,"Jet_genpt[nJet]/F");
  //// smalltree->Branch("Jet_residual",    Jet_residual    ,"Jet_residual[nJet]/F");
  //// smalltree->Branch("Jet_jes",         Jet_jes         ,"Jet_jes[nJet]/F");
  //// smalltree->Branch("Jet_eta",         Jet_eta         ,"Jet_eta[nJet]/F");
  //// smalltree->Branch("Jet_phi",         Jet_phi         ,"Jet_phi[nJet]/F");
  //// smalltree->Branch("Jet_ntracks",     Jet_ntracks     ,"Jet_ntracks[nJet]/I");
  //// smalltree->Branch("Jet_flavour",     Jet_flavour     ,"Jet_flavour[nJet]/I");
  //// //  smalltree->Branch("Jet_Ip1N",	   Jet_Ip1N	   ,"Jet_Ip1N[nJet]/F");
  //// //  smalltree->Branch("Jet_Ip1P",	   Jet_Ip1P	   ,"Jet_Ip1P[nJet]/F");
  //// smalltree->Branch("Jet_Ip2N",        Jet_Ip2N        ,"Jet_Ip2N[nJet]/F");
  //// smalltree->Branch("Jet_Ip2P",        Jet_Ip2P        ,"Jet_Ip2P[nJet]/F");
  //// smalltree->Branch("Jet_Ip3N",        Jet_Ip3N        ,"Jet_Ip3N[nJet]/F");
  //// smalltree->Branch("Jet_Ip3P",        Jet_Ip3P        ,"Jet_Ip3P[nJet]/F");
  //// //  smalltree->Branch("Jet_Ip4N",	   Jet_Ip4N	   ,"Jet_Ip4N[nJet]/F");
  //// //  smalltree->Branch("Jet_Ip4P",	   Jet_Ip4P	   ,"Jet_Ip4P[nJet]/F");
  //// //  smalltree->Branch("Jet_Mass4N",	   Jet_Mass4N	   ,"Jet_Mass4N[nJet]/F");
  //// //  smalltree->Branch("Jet_Mass4P",	   Jet_Mass4P	   ,"Jet_Mass4P[nJet]/F");
  //// smalltree->Branch("Jet_ProbaN",      Jet_ProbaN      ,"Jet_ProbaN[nJet]/F");
  //// smalltree->Branch("Jet_ProbaP",      Jet_ProbaP      ,"Jet_ProbaP[nJet]/F");
  //// //$$  smalltree->Branch("Jet_Proba",       Jet_Proba       ,"Jet_Proba[nJet]/F");
  //// smalltree->Branch("Jet_BprobN",      Jet_BprobN      ,"Jet_BprobN[nJet]/F");
  //// smalltree->Branch("Jet_BprobP",      Jet_BprobP      ,"Jet_BprobP[nJet]/F");
  //// //$$  smalltree->Branch("Jet_Bprob",       Jet_Bprob       ,"Jet_Bprob[nJet]/F");
  //// smalltree->Branch("Jet_SvxN",        Jet_SvxN        ,"Jet_SvxN[nJet]/F");
  //// smalltree->Branch("Jet_Svx",         Jet_Svx         ,"Jet_Svx[nJet]/F");
  //// //  smalltree->Branch("Jet_SvxNTracks",  Jet_SvxNTracks  ,"Jet_SvxNTracks[nJet]/I");
  //// //  smalltree->Branch("Jet_SvxTracks",   Jet_SvxTracks   ,"Jet_SvxTracks[nJet]/I");
  //// smalltree->Branch("Jet_SvxNHP",      Jet_SvxNHP      ,"Jet_SvxNHP[nJet]/F");
  //// smalltree->Branch("Jet_SvxHP",       Jet_SvxHP       ,"Jet_SvxHP[nJet]/F");
  //// smalltree->Branch("Jet_SvxMass",     Jet_SvxMass     ,"Jet_SvxMass[nJet]/F");
  //// smalltree->Branch("Jet_CombSvxN",    Jet_CombSvxN    ,"Jet_CombSvxN[nJet]/F");
  //// smalltree->Branch("Jet_CombSvxP",    Jet_CombSvxP    ,"Jet_CombSvxP[nJet]/F");
  //// smalltree->Branch("Jet_CombSvx",     Jet_CombSvx     ,"Jet_CombSvx[nJet]/F");

  //// smalltree->Branch("Jet_RetCombSvxN",	 Jet_RetCombSvxN    ,"Jet_RetCombSvxN[nJet]/F");
  //// smalltree->Branch("Jet_RetCombSvxP",	 Jet_RetCombSvxP    ,"Jet_RetCombSvxP[nJet]/F");
  //// smalltree->Branch("Jet_RetCombSvx",	 Jet_RetCombSvx     ,"Jet_RetCombSvx[nJet]/F");

  //// smalltree->Branch("Jet_CombCSVJP_N",	 Jet_CombCSVJP_N    ,"Jet_CombCSVJP_N[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVJP_P",	 Jet_CombCSVJP_P    ,"Jet_CombCSVJP_P[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVJP", 	 Jet_CombCSVJP      ,"Jet_CombCSVJP[nJet]/F");

  //// smalltree->Branch("Jet_CombCSVSL_N",	 Jet_CombCSVSL_N    ,"Jet_CombCSVSL_N[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVSL_P",	 Jet_CombCSVSL_P    ,"Jet_CombCSVSL_P[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVSL",	 Jet_CombCSVSL      ,"Jet_CombCSVSL[nJet]/F");

  //// smalltree->Branch("Jet_CombCSVJPSL_N", Jet_CombCSVJPSL_N  ,"Jet_CombCSVJPSL_N[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVJPSL_P", Jet_CombCSVJPSL_P  ,"Jet_CombCSVJPSL_P[nJet]/F");
  //// smalltree->Branch("Jet_CombCSVJPSL",	 Jet_CombCSVJPSL    ,"Jet_CombCSVJPSL[nJet]/F");

  //// smalltree->Branch("Jet_SimpIVF_HP",  Jet_SimpIVF_HP  ,"Jet_SimpIVF_HP[nJet]/F");
  //// smalltree->Branch("Jet_SimpIVF_HE",  Jet_SimpIVF_HE  ,"Jet_SimpIVF_HE[nJet]/F");
  //// smalltree->Branch("Jet_DoubIVF_HE",  Jet_DoubIVF_HE  ,"Jet_DoubIVF_HE[nJet]/F");
  //// smalltree->Branch("Jet_CombIVF",     Jet_CombIVF     ,"Jet_CombIVF[nJet]/F");
  //// smalltree->Branch("Jet_CombIVF_P"  , Jet_CombIVF_P   ,"Jet_CombIVF_P[nJet]/F");

  //// smalltree->Branch("Jet_SoftMuN",     Jet_SoftMuN     ,"Jet_SoftMuN[nJet]/F");
  //// smalltree->Branch("Jet_SoftMuP",     Jet_SoftMuP     ,"Jet_SoftMuP[nJet]/F");
  //// smalltree->Branch("Jet_SoftMu",      Jet_SoftMu      ,"Jet_SoftMu[nJet]/F");

  //// smalltree->Branch("Jet_SoftElN",     Jet_SoftElN     ,"Jet_SoftElN[nJet]/F");
  //// smalltree->Branch("Jet_SoftElP",     Jet_SoftElP     ,"Jet_SoftElP[nJet]/F");
  //// smalltree->Branch("Jet_SoftEl",      Jet_SoftEl      ,"Jet_SoftEl[nJet]/F");

  //// smalltree->Branch("Jet_hist1",       Jet_hist1       ,"Jet_hist1[nJet]/I");
  //// smalltree->Branch("Jet_hist2",       Jet_hist2       ,"Jet_hist2[nJet]/I");
  //// smalltree->Branch("Jet_hist3",       Jet_hist3       ,"Jet_hist3[nJet]/I");
  //// smalltree->Branch("Jet_histJet",     Jet_histJet     ,"Jet_histJet[nJet]/I");
  //// smalltree->Branch("Jet_histSvx",     Jet_histSvx     ,"Jet_histSvx[nJet]/I");

  //// smalltree->Branch("Jet_nFirstTrack", Jet_nFirstTrack ,"Jet_nFirstTrack[nJet]/I");
  //// smalltree->Branch("Jet_nLastTrack",  Jet_nLastTrack  ,"Jet_nLastTrack[nJet]/I");
  //// smalltree->Branch("Jet_nFirstSV",    Jet_nFirstSV    ,"Jet_nFirstSV[nJet]/I");
  //// smalltree->Branch("Jet_nLastSV",     Jet_nLastSV     ,"Jet_nLastSV[nJet]/I");
  //// smalltree->Branch("Jet_SV_multi",    Jet_SV_multi      ,"Jet_SV_multi[nJet]/I");
  //// smalltree->Branch("Jet_nFirstTrkInc", Jet_nFirstTrkInc ,"Jet_nFirstTrkInc[nJet]/I");
  //// smalltree->Branch("Jet_nLastTrkInc",  Jet_nLastTrkInc  ,"Jet_nLastTrkInc[nJet]/I");

  //// smalltree->Branch("Jet_VtxCat",      Jet_VtxCat  ,      "Jet_VtxCat[nJet]/I");
  //// smalltree->Branch("Jet_looseID",      Jet_looseID  ,      "Jet_looseID[nJet]/I");
  //// smalltree->Branch("Jet_tightID",      Jet_tightID  ,      "Jet_tightID[nJet]/I");

  //--------------------------------------
  // muon information
  //--------------------------------------
  smalltree->Branch("nMuon"	   ,&nMuon	 ,"nMuon/I");
  smalltree->Branch("Muon_IdxJet",   Muon_IdxJet   ,"Muon_IdxJet[nMuon]/I");
  smalltree->Branch("Muon_nMuHit",   Muon_nMuHit   ,"Muon_nMuHit[nMuon]/I");
  smalltree->Branch("Muon_nTkHit",   Muon_nTkHit   ,"Muon_nTkHit[nMuon]/I");
  smalltree->Branch("Muon_nPixHit",  Muon_nPixHit  ,"Muon_nPixHit[nMuon]/I");
  smalltree->Branch("Muon_nOutHit",  Muon_nOutHit  ,"Muon_nOutHit[nMuon]/I");
  smalltree->Branch("Muon_isGlobal", Muon_isGlobal ,"Muon_isGlobal[nMuon]/I");
  smalltree->Branch("Muon_nMatched", Muon_nMatched ,"Muon_nMatched[nMuon]/I");
  smalltree->Branch("Muon_chi2",     Muon_chi2     ,"Muon_chi2[nMuon]/F");
  smalltree->Branch("Muon_chi2Tk",   Muon_chi2Tk   ,"Muon_chi2Tk[nMuon]/F");
  smalltree->Branch("Muon_pt",       Muon_pt       ,"Muon_pt[nMuon]/F");
  smalltree->Branch("Muon_eta",      Muon_eta      ,"Muon_eta[nMuon]/F");
  smalltree->Branch("Muon_phi",      Muon_phi      ,"Muon_phi[nMuon]/F");
  smalltree->Branch("Muon_ptrel",    Muon_ptrel    ,"Muon_ptrel[nMuon]/F");
  smalltree->Branch("Muon_vz",       Muon_vz       ,"Muon_vz[nMuon]/F");
  smalltree->Branch("Muon_hist",     Muon_hist     ,"Muon_hist[nMuon]/I");
  //  smalltree->Branch("Muon_TrackIdx", Muon_TrackIdx ,"Muon_TrackIdx[nMuon]/I");
  smalltree->Branch("Muon_IPsig",    Muon_IPsig    ,"Muon_IPsig[nMuon]/F");
  smalltree->Branch("Muon_IP",       Muon_IP       ,"Muon_IP[nMuon]/F");
  smalltree->Branch("Muon_IP2Dsig",  Muon_IP2Dsig  ,"Muon_IP2Dsig[nMuon]/F");
  smalltree->Branch("Muon_IP2D",     Muon_IP2D     ,"Muon_IP2D[nMuon]/F");
  smalltree->Branch("Muon_Proba",    Muon_Proba    ,"Muon_Proba[nMuon]/F");
  smalltree->Branch("Muon_deltaR",   Muon_deltaR   ,"Muon_deltaR[nMuon]/F");
  smalltree->Branch("Muon_ratio",    Muon_ratio    ,"Muon_ratio[nMuon]/F");

  //--------------------------------------
  // pf electron information
  //--------------------------------------
  smalltree->Branch("nPFElectron"	  ,&nPFElectron	       ,"nPFElectron/I");
  smalltree->Branch("PFElectron_IdxJet",   PFElectron_IdxJet   ,"PFElectron_IdxJet[nPFElectron]/I");
  smalltree->Branch("PFElectron_pt",       PFElectron_pt       ,"PFElectron_pt[nPFElectron]/F");
  smalltree->Branch("PFElectron_eta",      PFElectron_eta      ,"PFElectron_eta[nPFElectron]/F");
  smalltree->Branch("PFElectron_phi",      PFElectron_phi      ,"PFElectron_phi[nPFElectron]/F");
  smalltree->Branch("PFElectron_ptrel",    PFElectron_ptrel    ,"PFElectron_ptrel[nPFElectron]/F");
  smalltree->Branch("PFElectron_deltaR",   PFElectron_deltaR   ,"PFElectron_deltaR[nPFElectron]/F");
  smalltree->Branch("PFElectron_ratio",    PFElectron_ratio    ,"PFElectron_ratio[nPFElectron]/F");
  smalltree->Branch("PFElectron_ratioRel", PFElectron_ratioRel ,"PFElectron_ratioRel[nPFElectron]/F");
  smalltree->Branch("PFElectron_IPsig",    PFElectron_IPsig    ,"PFElectron_IPsig[nPFElectron]/F");
  //smalltree->Branch("PFElectron_mva_e_pi", PFElectron_mva_e_pi ,"PFElectron_mva_e_pi[nPFElectron]/F");

  //--------------------------------------
  // pf muon information
  //--------------------------------------
  smalltree->Branch("nPFMuon"	         ,&nPFMuon           ,"nPFMuon/I");
  smalltree->Branch("PFMuon_IdxJet",      PFMuon_IdxJet      ,"PFMuon_IdxJet[nPFMuon]/I");
  smalltree->Branch("PFMuon_pt",          PFMuon_pt          ,"PFMuon_pt[nPFMuon]/F");
  smalltree->Branch("PFMuon_eta",         PFMuon_eta         ,"PFMuon_eta[nPFMuon]/F");
  smalltree->Branch("PFMuon_phi",         PFMuon_phi         ,"PFMuon_phi[nPFMuon]/F");
  smalltree->Branch("PFMuon_ptrel",       PFMuon_ptrel       ,"PFMuon_ptrel[nPFMuon]/F");
  smalltree->Branch("PFMuon_deltaR",      PFMuon_deltaR      ,"PFMuon_deltaR[nPFMuon]/F");
  smalltree->Branch("PFMuon_ratio",       PFMuon_ratio       ,"PFMuon_ratio[nPFMuon]/F");
  smalltree->Branch("PFMuon_ratioRel",    PFMuon_ratioRel    ,"PFMuon_ratioRel[nPFMuon]/F");
  smalltree->Branch("PFMuon_IPsig",       PFMuon_IPsig       ,"PFMuon_IPsig[nPFMuon]/F");
  smalltree->Branch("PFMuon_GoodQuality", PFMuon_GoodQuality ,"PFMuon_GoodQuality[nPFMuon]/I");

  //--------------------------------------
  // Inclusive Track information for PtRel template
  //--------------------------------------
  smalltree->Branch("nTrkInc"	   ,&nTrkInc	 ,"nTrkInc/I");
  smalltree->Branch("TrkInc_pt",       TrkInc_pt       ,"TrkInc_pt[nTrkInc]/F");
  smalltree->Branch("TrkInc_eta",      TrkInc_eta      ,"TrkInc_eta[nTrkInc]/F");
  smalltree->Branch("TrkInc_phi",      TrkInc_phi      ,"TrkInc_phi[nTrkInc]/F");
  smalltree->Branch("TrkInc_ptrel",    TrkInc_ptrel    ,"TrkInc_ptrel[nTrkInc]/F");
  smalltree->Branch("TrkInc_IPsig",    TrkInc_IPsig    ,"TrkInc_IPsig[nTrkInc]/F");
  smalltree->Branch("TrkInc_IP",       TrkInc_IP       ,"TrkInc_IP[nTrkInc]/F");

  smalltree->Branch("ncQuarks",          &ncQuarks      ,"ncQuarks/I");
  smalltree->Branch("cQuark_pT",         cQuark_pT      ,"cQuark_pT[ncQuarks]/F");
  smalltree->Branch("cQuark_eta",        cQuark_eta     ,"cQuark_eta[ncQuarks]/F");
  smalltree->Branch("cQuark_phi",        cQuark_phi     ,"cQuark_phi[ncQuarks]/F");
  smalltree->Branch("cQuark_fromGSP",    cQuark_fromGSP ,"cQuark_fromGSP[ncQuarks]/I");

  smalltree->Branch("nbQuarks",          &nbQuarks      ,"nbQuarks/I");
  smalltree->Branch("bQuark_pT",         bQuark_pT      ,"bQuark_pT[nbQuarks]/F");
  smalltree->Branch("bQuark_eta",        bQuark_eta     ,"bQuark_eta[nbQuarks]/F");
  smalltree->Branch("bQuark_phi",        bQuark_phi     ,"bQuark_phi[nbQuarks]/F");
  smalltree->Branch("bQuark_fromGSP",    bQuark_fromGSP ,"bQuark_fromGSP[nbQuarks]/I");

  smalltree->Branch("mcweight",      &mcweight     ,"mcweight/F");

  smalltree->Branch("nBHadrons",     &nBHadrons    ,"nBHadrons/I");
  smalltree->Branch("BHadron_pT",    BHadron_pT    ,"BHadron_pT[nBHadrons]/F");
  smalltree->Branch("BHadron_eta",   BHadron_eta   ,"BHadron_eta[nBHadrons]/F");
  smalltree->Branch("BHadron_phi",   BHadron_phi   ,"BHadron_phi[nBHadrons]/F");
  smalltree->Branch("BHadron_mass",  BHadron_mass  ,"BHadron_mass[nBHadrons]/F");
  smalltree->Branch("BHadron_pdgID", BHadron_pdgID ,"BHadron_pdgID[nBHadrons]/I");
  smalltree->Branch("BHadron_mother", BHadron_mother ,"BHadron_mother[nBHadrons]/I");
  smalltree->Branch("BHadron_hasBdaughter", BHadron_hasBdaughter ,"BHadron_hasBdaughter[nBHadrons]/I");

  //$$
  smalltree->Branch("nGenlep",    &nGenlep      ,"nGenlep/I");
  smalltree->Branch("Genlep_pT",    Genlep_pT    ,"Genlep_pT[nGenlep]/F");
  smalltree->Branch("Genlep_eta",   Genlep_eta   ,"Genlep_eta[nGenlep]/F");
  smalltree->Branch("Genlep_phi",   Genlep_phi   ,"Genlep_phi[nGenlep]/F");
  smalltree->Branch("Genlep_pdgID", Genlep_pdgID ,"Genlep_pdgID[nGenlep]/I");
  smalltree->Branch("Genlep_mother",Genlep_mother,"Genlep_mother[nGenlep]/I");

  smalltree->Branch("nGenquark",    &nGenquark      ,"nGenquark/I");
  smalltree->Branch("Genquark_pT",    Genquark_pT    ,"Genquark_pT[nGenquark]/F");
  smalltree->Branch("Genquark_eta",   Genquark_eta   ,"Genquark_eta[nGenquark]/F");
  smalltree->Branch("Genquark_phi",   Genquark_phi   ,"Genquark_phi[nGenquark]/F");
  smalltree->Branch("Genquark_pdgID", Genquark_pdgID ,"Genquark_pdgID[nGenquark]/I");
  smalltree->Branch("Genquark_mother",Genquark_mother,"Genquark_mother[nGenquark]/I");

  //$$

  //// Tree for storing subjet variables
  ////if (runSubJets_) {

  ////  //--------------------------------------
  ////  // FatJet information
  ////  //--------------------------------------
  ////  smalltree->Branch("nFatJet"            ,    &nFatJet);
  ////  smalltree->Branch("FatJet_pt"          ,    FatJet_pt	   );
  ////  smalltree->Branch("FatJet_genpt"       ,    FatJet_genpt   );
  ////  smalltree->Branch("FatJet_residual"    ,    FatJet_residual    );
  ////  smalltree->Branch("FatJet_jes"         ,    FatJet_jes    );
  ////  smalltree->Branch("FatJet_eta"         ,    FatJet_eta    );
  ////  smalltree->Branch("FatJet_phi"         ,    FatJet_phi    );
  ////  smalltree->Branch("FatJet_ntracks"     ,    FatJet_ntracks    );
  ////  smalltree->Branch("FatJet_flavour"     ,    FatJet_flavour    );
  ////  //  smalltree->Branch("FatJet_Ip1N"	   ,	   FatJet_Ip1N	   );
  ////  //  smalltree->Branch("FatJet_Ip1P"	   ,	   FatJet_Ip1P	   );
  ////  smalltree->Branch("FatJet_Ip2N"        ,    FatJet_Ip2N    );
  ////  smalltree->Branch("FatJet_Ip2P"        ,    FatJet_Ip2P    );
  ////  smalltree->Branch("FatJet_Ip3N"        ,    FatJet_Ip3N    );
  ////  smalltree->Branch("FatJet_Ip3P"        ,    FatJet_Ip3P    );
  ////  //  smalltree->Branch("FatJet_Ip4N"	   ,	   FatJet_Ip4N	   );
  ////  //  smalltree->Branch("FatJet_Ip4P"	   ,	   FatJet_Ip4P	   );
  ////  //  smalltree->Branch("FatJet_Mass4N"	   ,	   FatJet_Mass4N	   );
  ////  //  smalltree->Branch("FatJet_Mass4P"	   ,	   FatJet_Mass4P	   );
  ////  smalltree->Branch("FatJet_ProbaN"      ,      FatJet_ProbaN      );
  ////  smalltree->Branch("FatJet_ProbaP"      ,      FatJet_ProbaP      );
  ////  //  $$  smalltree->Branch("FatJet_Proba"       ,       FatJet_Proba       );
  ////  smalltree->Branch("FatJet_BprobN"      ,      FatJet_BprobN      );
  ////  smalltree->Branch("FatJet_BprobP"      ,      FatJet_BprobP      );
  ////  //  $$  smalltree->Branch("FatJet_Bprob"       ,       FatJet_Bprob       );
  ////  smalltree->Branch("FatJet_SvxN"        ,        FatJet_SvxN        );
  ////  smalltree->Branch("FatJet_Svx"         ,         FatJet_Svx         );
  ////  //  smalltree->Branch("FatJet_SvxNTracks"  ,  FatJet_SvxNTracks  );
  ////  //  smalltree->Branch("FatJet_SvxTracks"   ,   FatJet_SvxTracks   );
  ////  smalltree->Branch("FatJet_SvxNHP"      ,      FatJet_SvxNHP      );
  ////  smalltree->Branch("FatJet_SvxHP"       ,       FatJet_SvxHP       );
  ////  smalltree->Branch("FatJet_SvxMass"     ,     FatJet_SvxMass     );
  ////  smalltree->Branch("FatJet_CombSvxN"    ,    FatJet_CombSvxN    );
  ////  smalltree->Branch("FatJet_CombSvxP"    ,    FatJet_CombSvxP    );
  ////  smalltree->Branch("FatJet_CombSvx"     ,     FatJet_CombSvx     );

  ////  smalltree->Branch("FatJet_RetCombSvxN"	 ,	 FatJet_RetCombSvxN    );
  ////  smalltree->Branch("FatJet_RetCombSvxP"	 ,	 FatJet_RetCombSvxP    );
  ////  smalltree->Branch("FatJet_RetCombSvx"	 ,	 FatJet_RetCombSvx     );

  ////  smalltree->Branch("FatJet_CombCSVJP_N"	 ,	 FatJet_CombCSVJP_N    );
  ////  smalltree->Branch("FatJet_CombCSVJP_P"	 ,	 FatJet_CombCSVJP_P    );
  ////  smalltree->Branch("FatJet_CombCSVJP" 	 , 	 FatJet_CombCSVJP      );

  ////  smalltree->Branch("FatJet_CombCSVSL_N"	 ,	 FatJet_CombCSVSL_N    );
  ////  smalltree->Branch("FatJet_CombCSVSL_P"	 ,	 FatJet_CombCSVSL_P    );
  ////  smalltree->Branch("FatJet_CombCSVSL"	 ,	 FatJet_CombCSVSL      );

  ////  smalltree->Branch("FatJet_CombCSVJPSL_N" , FatJet_CombCSVJPSL_N  );
  ////  smalltree->Branch("FatJet_CombCSVJPSL_P" , FatJet_CombCSVJPSL_P  );
  ////  smalltree->Branch("FatJet_CombCSVJPSL"	 ,	 FatJet_CombCSVJPSL    );

  ////  smalltree->Branch("FatJet_SimpIVF_HP"  ,  FatJet_SimpIVF_HP  );
  ////  smalltree->Branch("FatJet_SimpIVF_HE"  ,  FatJet_SimpIVF_HE  );
  ////  smalltree->Branch("FatJet_DoubIVF_HE"  ,  FatJet_DoubIVF_HE  );
  ////  smalltree->Branch("FatJet_CombIVF"     ,     FatJet_CombIVF     );
  ////  smalltree->Branch("FatJet_CombIVF_P"   , FatJet_CombIVF_P   );

  ////  smalltree->Branch("FatJet_SoftMuN"     ,     FatJet_SoftMuN     );
  ////  smalltree->Branch("FatJet_SoftMuP"     ,     FatJet_SoftMuP     );
  ////  smalltree->Branch("FatJet_SoftMu"      ,      FatJet_SoftMu      );

  ////  smalltree->Branch("FatJet_SoftElN"     , FatJet_SoftElN);
  ////  smalltree->Branch("FatJet_SoftElP"     , FatJet_SoftElP);
  ////  smalltree->Branch("FatJet_SoftEl"      , FatJet_SoftEl );

  ////  smalltree->Branch("FatJet_hist1"       , FatJet_hist1   );
  ////  smalltree->Branch("FatJet_hist2"       , FatJet_hist2   );
  ////  smalltree->Branch("FatJet_hist3"       , FatJet_hist3   );
  ////  smalltree->Branch("FatJet_histFatJet"     , FatJet_histFatJet);
  ////  smalltree->Branch("FatJet_histSvx"     , FatJet_histSvx   );

  ////  smalltree->Branch("FatJet_nFirstTrack" , FatJet_nFirstTrack );
  ////  smalltree->Branch("FatJet_nLastTrack"  , FatJet_nLastTrack  );
  ////  smalltree->Branch("FatJet_nFirstSV"    , FatJet_nFirstSV    );
  ////  smalltree->Branch("FatJet_nLastSV"     , FatJet_nLastSV     );
  ////  smalltree->Branch("FatJet_SV_multi"    , FatJet_SV_multi    );
  ////  smalltree->Branch("FatJet_nFirstTrkInc", FatJet_nFirstTrkInc);
  ////  smalltree->Branch("FatJet_nLastTrkInc" , FatJet_nLastTrkInc );

  ////  smalltree->Branch("FatJet_VtxCat"      , FatJet_VtxCat );
  ////  smalltree->Branch("FatJet_looseID"     , FatJet_looseID);
  ////  smalltree->Branch("FatJet_tightID"     , FatJet_tightID);

  ////}

  cout << "BTagAnalyzer constructed " << endl;
}


BTagAnalyzer::~BTagAnalyzer()
{
}


static std::vector<std::size_t> sortedIndexes(const std::vector<TrackIPTagInfo::TrackIPData >& values)
{
  std::multimap<float, std::size_t> sortedIdx;
  std::vector<std::size_t> result;

  for(size_t i = 0; i < values.size(); ++i)
    sortedIdx.insert( std::pair<float, std::size_t>(values[i].ip3d.significance() , i) );

  for(std::multimap<float, std::size_t>::reverse_iterator it = sortedIdx.rbegin(); it != sortedIdx.rend(); ++it)
    result.push_back(it->second);

  return result;
}


//
// member functions
//

// ------------ method called to for each event  ------------
void BTagAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  using namespace reco;

  //------------------------------------------------------
  //Event informations
  //------------------------------------------------------
  Run = iEvent.id().run();
  isData_ = iEvent.isRealData();

  if ( !isData_ && Run > 0 ) Run = -Run;

  Evt  = iEvent.id().event();
  LumiBlock  = iEvent.luminosityBlock();

  // Tag Jets
  if ( useTrackHistory_ ) classifier_.newEvent(iEvent, iSetup);

  edm::Handle <PatJetCollection> jetsColl;
  iEvent.getByLabel (JetCollectionTag_, jetsColl);

  edm::Handle <PatJetCollection> fatjetsColl;
  if (runSubJets_) {
    iEvent.getByLabel(FatJetCollectionTag_, fatjetsColl) ;
  }

  //------------------------------------------------------
  // MC informations
  //------------------------------------------------------
  pthat = -1.;
  nPUtrue = -1;
  nPU = 0;
  ncQuarks = 0;
  nbQuarks = 0;
  nBHadrons    = 0;
  //$$
  nGenlep     = 0;
  nGenquark   = 0;
  //$$
  PVzSim = -10.;
  mcweight=1.;
  if ( !isData_ ) {
    // pthat
    edm::Handle<GenEventInfoProduct> geninfos;
    iEvent.getByLabel( "generator",geninfos );
    mcweight=geninfos->weight();
    if (geninfos->binningValues().size()>0) pthat = geninfos->binningValues()[0];
    // pileup

    edm::Handle<std::vector <PileupSummaryInfo> > PupInfo;
    iEvent.getByLabel("addPileupInfo", PupInfo);

    std::vector<PileupSummaryInfo>::const_iterator ipu;
    for (ipu = PupInfo->begin(); ipu != PupInfo->end(); ++ipu) {
      if ( ipu->getBunchCrossing() != 0 ) continue;
      for (unsigned int i=0; i<ipu->getPU_zpositions().size(); ++i) {
        PU_bunch[nPU]	  =  ipu->getBunchCrossing();
        PU_z[nPU]          = (ipu->getPU_zpositions())[i];
        PU_sumpT_low[nPU]  = (ipu->getPU_sumpT_lowpT())[i];
        PU_sumpT_high[nPU] = (ipu->getPU_sumpT_highpT())[i];
        PU_ntrks_low[nPU]  = (ipu->getPU_ntrks_lowpT())[i];
        PU_ntrks_high[nPU] = (ipu->getPU_ntrks_highpT())[i];
        ++nPU;
      }
      nPUtrue = ipu->getTrueNumInteractions();
    }

    edm::Handle<reco::GenParticleCollection> genParticles;
    iEvent.getByLabel ("genParticles", genParticles);

    //$$
    // generated particles
    for (size_t i = 0; i < genParticles->size (); ++i) {
      const GenParticle & genIt = (*genParticles)[i];
      int ID = abs(genIt.pdgId());

      // prompt b and c
      if ( (ID == 4 || ID == 5) && genIt.status() == 3 ) {
        Genquark_pT[nGenquark]     = genIt.p4().pt();
        Genquark_eta[nGenquark]    = genIt.p4().eta();
        Genquark_phi[nGenquark]    = genIt.p4().phi();
        Genquark_pdgID[nGenquark]  = genIt.pdgId();
        Genquark_mother[nGenquark] = genIt.mother()->pdgId();
        //  cout << " status " << genIt.status() << " pdgId " << genIt.pdgId()  << " pT " << genIt.p4().pt() << " mother " << moth->pdgId() << endl;
        ++nGenquark;
      }

      // b and c quarks from the end of parton showering and before hadronization
      if ( ( ID == 4 || ID == 5 ) && genIt.status() == 2 )
      {
        if( genIt.numberOfDaughters() > 0 )
        {
          int nparton_daughters = 0;
          for (unsigned d=0; d<genIt.numberOfDaughters(); ++d)
          {
            int daughterID = abs(genIt.daughter(d)->pdgId());
            if( (daughterID == 1 || daughterID == 2 || daughterID == 3 ||
                 daughterID == 4 || daughterID == 5 || daughterID == 6 || daughterID == 21))
              nparton_daughters++;
          }
          if(nparton_daughters == 0)
          {
            if ( ID == 5 )
            {
              bQuark_pT[nbQuarks]  = genIt.p4().pt();
              bQuark_eta[nbQuarks] = genIt.p4().eta();
              bQuark_phi[nbQuarks] = genIt.p4().phi();
              bQuark_fromGSP[nbQuarks] = isFromGSP(&genIt);
              ++nbQuarks;
            }
            if ( ID == 4 )
            {
              cQuark_pT[ncQuarks]  = genIt.p4().pt();
              cQuark_eta[ncQuarks] = genIt.p4().eta();
              cQuark_phi[ncQuarks] = genIt.p4().phi();
              cQuark_fromGSP[ncQuarks] = isFromGSP(&genIt);
              ++ncQuarks;
            }
          }
        }
      }

      // BHadrons
      if ( (ID >=  511 && ID <=  557) || (ID >= 5114 && ID <= 5554) ||
           (ID >=10511 && ID <=10555) || (ID >=20513 && ID <=20555) || ID ==30553 ||
           (ID>=100551 && ID<=100557) || (ID>=110551 && ID<=110557) ||
           (ID>=120551 && ID<=120557) || (ID>=130551 && ID<=130557) ||
           (ID>=200551 && ID<=200557) || (ID>=210551 && ID<=210557) ||
           (ID>=220551 && ID<=220557) || (ID>=300551 && ID<=300557) ||
           ID == 9000553 || ID == 9010553 )
      {
        //  cout << " pdgId " << genIt.pdgId()  << " pT " << genIt.p4().pt() << " mother " << mother->pdgId() << endl;
        BHadron_pT[nBHadrons]    = genIt.p4().pt();
        BHadron_eta[nBHadrons]   = genIt.p4().eta();
        BHadron_phi[nBHadrons]   = genIt.p4().phi();
        BHadron_mass[nBHadrons]  = genIt.mass();
        BHadron_pdgID[nBHadrons] = genIt.pdgId();
        BHadron_mother[nBHadrons] = genIt.mother()->pdgId();
        // check if any of the daughters is also B hadron
        int hasBHadronDaughter = 0;
        for(unsigned d=0; d<genIt.numberOfDaughters(); ++d)
        {
          int daughterID = abs(genIt.daughter(d)->pdgId());
          if ( (daughterID/100)%10 == 5 || (daughterID/1000)%10 == 5 ) { hasBHadronDaughter = 1; break; }
        }
        BHadron_hasBdaughter[nBHadrons] = hasBHadronDaughter;
        ++nBHadrons;
      }

      // Leptons
      if ( (ID == 11 || ID == 13) && genIt.p4().pt() > 3. ) {
        const Candidate * moth1 = genIt.mother();
        if ( moth1->pdgId() != genIt.pdgId() ) {
          Genlep_pT[nGenlep]    = genIt.p4().pt();
          Genlep_eta[nGenlep]   = genIt.p4().eta();
          Genlep_phi[nGenlep]   = genIt.p4().phi();
          Genlep_pdgID[nGenlep] = genIt.pdgId();
          const Candidate * moth2 = moth1->mother();
          const Candidate * moth3 = moth2->mother();
          const Candidate * moth4 = moth3->mother();
          int isWZ = 0, istau = 0, isB = 0, isD = 0;
          int ID1 = abs( moth1->pdgId() );
          int ID2 = abs( moth2->pdgId() );
          int ID3 = abs( moth3->pdgId() );
          int ID4 = abs( moth4->pdgId() );
          if ( ID1 == 15 ) istau = 1;
          if ( ID1 == 24 || ID2 == 24 || ID3 == 24 || ID4 == 24 ||
              ID1 == 25 || ID2 == 25 || ID3 == 25 || ID4 == 25 ) isWZ = 1;
          if ( ID1 == 411 || ID1 == 421 || ID1 == 431 ||
              ID1 ==4112 || ID1 ==4122 || ID1 ==4132 ||
              ID1 ==4212 || ID1 ==4222 || ID1 ==4232 || ID1 ==4332 ) isD = 1;
          if ( ID2 == 411 || ID2 == 421 || ID2 == 431 ||
              ID2 ==4112 || ID2 ==4122 || ID2 ==4132 ||
              ID2 ==4212 || ID2 ==4222 || ID2 ==4232 || ID2 ==4332 ) isD = 1;
          if ( ID1 == 511 || ID1 == 521 || ID1 == 531 || ID1 == 541 ||
              ID1 ==5112 || ID1 ==5122 || ID1 ==5132 ||
              ID1 ==5212 || ID1 ==5222 || ID1 ==5232 || ID1 ==5332 ) isB = 1;
          if ( ID2 == 511 || ID2 == 521 || ID2 == 531 || ID2 == 541 ||
              ID2 ==5112 || ID2 ==5122 || ID2 ==5132 ||
              ID2 ==5212 || ID2 ==5222 || ID2 ==5232 || ID2 ==5332 ) isB = 1;
          if ( ID3 == 511 || ID3 == 521 || ID3 == 531 || ID3 == 541 ||
              ID3 ==5112 || ID3 ==5122 || ID3 ==5132 ||
              ID3 ==5212 || ID3 ==5222 || ID3 ==5232 || ID3 ==5332 ) isB = 1;
          if ( ID4 == 511 || ID4 == 521 || ID4 == 531 || ID4 == 541 ||
              ID4 ==5112 || ID4 ==5122 || ID4 ==5132 ||
              ID4 ==5212 || ID4 ==5222 || ID4 ==5232 || ID4 ==5332 ) isB = 1;
          if ( isB+isD != 0 ) Genlep_mother[nGenlep] = 5*isB + 4*isD;
          else                Genlep_mother[nGenlep] = 10*istau + 100*isWZ;
          //  cout << " lepton " << nGenlep << " pdgID " << Genlep_pdgID[nGenlep]
          //       << " moth1 " << moth1->pdgId() << " moth2 " << moth2->pdgId()
          //       << " moth3 " << moth3->pdgId() << " moth4 " << moth4->pdgId()
          //       << " pT " << Genlep_pT[nGenlep]
          //       << " from " << Genlep_mother[nGenlep] << endl;
          ++nGenlep;
        }
      }

    } // end loop on generated particles
    //$$

    // simulated PV
    if ( useTrackHistory_ ) {
      edm::Handle<edm::HepMCProduct> theHepMCProduct;
      iEvent.getByLabel("generator",theHepMCProduct);
      std::vector<simPrimaryVertex> simpv;
      simpv=getSimPVs(theHepMCProduct);
      //       cout << "simpv.size() " << simpv.size() << endl;
    }
  } // end MC info
  //   cout << "Evt:" <<Evt << endl;
  //   cout << "pthat:" <<pthat << endl;


  //------------------------------------------------------
  // ttbar information
  //------------------------------------------------------

  if (use_ttbar_filter_) {
    Handle<int> pIn;
    iEvent.getByLabel(channel_, pIn);
    ttbar_chan=*pIn;
    Handle<vector< double  >> pIn2;
    iEvent.getByLabel(channel_, pIn2);
    if (pIn2->size()==8) {
      lepton1_pT=(*pIn2)[0];
      lepton1_eta=(*pIn2)[1];
      lepton1_phi=(*pIn2)[2];
      lepton2_pT=(*pIn2)[3];
      lepton2_eta=(*pIn2)[4];
      lepton2_phi=(*pIn2)[5];
      met=(*pIn2)[6];
      mll=(*pIn2)[7];
    }
    else {
      lepton1_pT=-1;
      lepton1_eta=-1;
      lepton1_phi=-1;
      lepton2_pT=-1;
      lepton2_eta=-1;
      lepton2_phi=-1;
      met=-1;
      mll=-1;
    }
  }


  //------------------------------------------------------
  // Muons
  //------------------------------------------------------
  edm::Handle<edm::View<reco::Muon> >  muonsHandle;
  iEvent.getByLabel(muonCollectionName_,muonsHandle);
  muons = *muonsHandle;

  //----------------------------------------
  // Transient track for IP calculation
  //----------------------------------------
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);


  //------------------
  // Primary vertex
  //------------------
  iEvent.getByLabel(primaryVertexColl_,primaryVertex);

  //bool newvertex = false;

  bool pvFound = (primaryVertex->size() != 0);
  if ( pvFound ) {
    pv = &(*primaryVertex->begin());
  }
  else {
    reco::Vertex::Error e;
    e(0,0)=0.0015*0.0015;
    e(1,1)=0.0015*0.0015;
    e(2,2)=15.*15.;
    reco::Vertex::Point p(0,0,0);
    pv=  new reco::Vertex(p,e,1,1,1);
    //newvertex = true;
  }
  //   GlobalPoint Pv_point = GlobalPoint((*pv).x(), (*pv).y(), (*pv).z());
  PVz = (*primaryVertex)[0].z();

  nPV=0;
  for (unsigned int i = 0; i< primaryVertex->size() ; ++i) {
    PV_x[nPV]      = (*primaryVertex)[i].x();
    PV_y[nPV]      = (*primaryVertex)[i].y();
    PV_z[nPV]      = (*primaryVertex)[i].z();
    PV_ex[nPV]     = (*primaryVertex)[i].xError();
    PV_ey[nPV]     = (*primaryVertex)[i].yError();
    PV_ez[nPV]     = (*primaryVertex)[i].zError();
    PV_chi2[nPV]   = (*primaryVertex)[i].normalizedChi2();
    PV_ndf[nPV]    = (*primaryVertex)[i].ndof();
    PV_isgood[nPV] = (*primaryVertex)[i].isValid();
    PV_isfake[nPV] = (*primaryVertex)[i].isFake();

    ++nPV;
  }


  //------------------------------------------------------
  // Trigger info
  //------------------------------------------------------

  Handle<TriggerResults> trigRes;
  iEvent.getByLabel(triggerTable_, trigRes);

  BitTrigger = 0;
  if (use_ttbar_filter_) trig_ttbar = 0;

  vector<string> triggerList;
  Service<service::TriggerNamesService> tns;
  bool foundNames = tns->getTrigPaths(*trigRes,triggerList);
  if ( !foundNames ) cout << "Could not get trigger names!\n";
  if ( trigRes->size() != triggerList.size() ) cout << "ERROR: length of names and paths not the same: " << triggerList.size() << "," << trigRes->size() << endl;

  //int NoJetID = 0;
  for (unsigned int i=0; i< trigRes->size(); ++i) {
    if ( !trigRes->at(i).accept() ) continue;


    if (NameCompatible("HLT_Jet15U*",triggerList[i]) || NameCompatible("HLT_Jet30_v*",triggerList[i])
        ||NameCompatible("HLT_PFJet40_v*",triggerList[i]) ) BitTrigger +=10 ;

    if (NameCompatible("HLT_Jet30U*",triggerList[i]) || NameCompatible("HLT_Jet60_v*",triggerList[i]) ) BitTrigger +=20 ;

    if (NameCompatible("HLT_Jet50U*",triggerList[i]) || NameCompatible("HLT_Jet80_v*",triggerList[i])
        ||NameCompatible("HLT_PFJet80_v*",triggerList[i]) ) BitTrigger +=40 ;

    if (NameCompatible("HLT_Jet70U*",triggerList[i]) || NameCompatible("HLT_Jet110_v*",triggerList[i]) ) BitTrigger +=100 ;

    if (NameCompatible("HLT_Jet100U*",triggerList[i]) || NameCompatible("HLT_Jet150_v*",triggerList[i])
        ||NameCompatible("HLT_PFJet140_v*",triggerList[i]) ) BitTrigger +=200 ;

    if (NameCompatible("HLT_Jet140U*",triggerList[i]) || NameCompatible("HLT_Jet190_v*",triggerList[i])
        ||NameCompatible("HLT_PFJet200_v*",triggerList[i]) ) BitTrigger +=400 ;

    if (NameCompatible("HLT_Jet240_v*",triggerList[i]) || NameCompatible("HLT_PFJet260_v*",triggerList[i]) ) BitTrigger +=1 ;

    if (NameCompatible("HLT_Jet300_v*",triggerList[i]) || NameCompatible("HLT_PFJet320_v*",triggerList[i]) ) BitTrigger +=2 ;

    if (NameCompatible("HLT_PFJet400_v*",triggerList[i]) ) BitTrigger +=4 ;

    if (NameCompatible("HLT_DiJetAve15U*",triggerList[i]) ||
        NameCompatible("HLT_DiJetAve30_v*",triggerList[i])||NameCompatible("HLT_DiPFJetAve40_v*",triggerList[i])) BitTrigger +=1000 ;

    if (NameCompatible("HLT_DiJetAve30U*",triggerList[i]) ||
        NameCompatible("HLT_DiJetAve60_v*",triggerList[i])||NameCompatible("HLT_DiPFJetAve80_v*",triggerList[i]) ) BitTrigger +=2000 ;

    if (NameCompatible("HLT_DiJetAve50U*",triggerList[i]) ||
        NameCompatible("HLT_DiJetAve80_v*",triggerList[i])||NameCompatible("HLT_DiPFJetAve140_v*",triggerList[i])) BitTrigger +=4000 ;

    if (NameCompatible("HLT_BTagMu_Jet10U*",triggerList[i]) ||
        NameCompatible("HLT_BTagMu_Jet20U*",triggerList[i])||NameCompatible("HLT_BTagMu_DiJet20U*",triggerList[i])
        ||NameCompatible("HLT_BTagMu_DiJet20U_Mu5*",triggerList[i])||NameCompatible("HLT_BTagMu_DiJet20_Mu5*",triggerList[i])
        ||NameCompatible("HLT_BTagMu_DiJet20_L1FastJet_Mu5_v*",triggerList[i])) BitTrigger +=10000 ;

    if (NameCompatible("HLT_BTagMu_DiJet30U",triggerList[i]) ||
        NameCompatible("HLT_BTagMu_DiJet30U_v*",triggerList[i])||
        NameCompatible("HLT_BTagMu_DiJet30U_Mu5*",triggerList[i])||NameCompatible("HLT_BTagMu_DiJet60_Mu7*",triggerList[i])
        ||NameCompatible("HLT_BTagMu_DiJet40_Mu5*",triggerList[i])||NameCompatible("HLT_BTagMu_DiJet20_L1FastJet_Mu5*",triggerList[i])) BitTrigger +=20000 ;

    if (NameCompatible("HLT_BTagMu_DiJet80_Mu9*",triggerList[i]) ||
        NameCompatible("HLT_BTagMu_DiJet70_Mu5*",triggerList[i]) ||NameCompatible("HLT_BTagMu_DiJet70_L1FastJet_Mu5*",triggerList[i]) )BitTrigger +=40000 ;

    if (NameCompatible("HLT_BTagMu_DiJet100_Mu9_v*",triggerList[i]) ||
        NameCompatible("HLT_BTagMu_DiJet110_Mu5*",triggerList[i])
        ||NameCompatible("HLT_BTagMu_DiJet110_L1FastJet_Mu5*",triggerList[i]) )BitTrigger +=100000 ;

    if (NameCompatible("HLT_BTagMu_Jet300_L1FastJet_Mu5*",triggerList[i]) ||
        NameCompatible("HLT_BTagMu_Jet300_Mu5*",triggerList[i]) )BitTrigger +=200000 ;

    if (use_ttbar_filter_) {
      // trigger for ttbar  : https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiTopRefEventSel#Triggers
      if (NameCompatible("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*",triggerList[i]) ) trig_ttbar +=1 ;  // 2-electron
      if (NameCompatible("HLT_Mu17_Mu8_v*",triggerList[i]) )  trig_ttbar +=2 ;  // 2-muon case1
      if (NameCompatible("HLT_Mu17_TkMu8_v*",triggerList[i]) ) trig_ttbar +=4 ;  // 2-muon case2
      if (NameCompatible("HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*",triggerList[i]) ) trig_ttbar +=10;  //muon + electron case1
      if (NameCompatible("HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*",triggerList[i]) ) trig_ttbar +=20 ;  // muon + electron
    }

    // std::cout << " Run Evt " << Run << " " << Evt << " trigger list " << triggerList[i] << std::endl;
  }
  // std::cout << " Run Evt " << Run << " " << Evt << " bit trigger " << BitTrigger << std::endl;

  if (use_ttbar_filter_) {
    if (trig_ttbar>0) BitTrigger+=1000000; // to fill the ntuple!
  }


  //cout << "BitTrigger:" <<BitTrigger << endl;
  PFJet80 = false;
  if ( (BitTrigger%100 - BitTrigger%10)/10 >= 4 ) PFJet80 = true;

  nTrack = 0;
  nTrkInc = 0;
  nSV = 0;

  //------------- added by Camille-----------------------------------------------------------//
  //cout << "SVComputer_.label() " << SVComputer_.label() << endl;
  edm::ESHandle<JetTagComputer> computerHandle;
  iSetup.get<JetTagComputerRecord>().get( SVComputer_.label(), computerHandle );

  computer = dynamic_cast<const GenericMVAJetTagComputer*>( computerHandle.product() );

  computer->passEventSetup(iSetup);
  //------------- end added-----------------------------------------------------------//


  if (use_ttbar_filter_) {
    thelepton1.SetPtEtaPhiM(lepton1_pT, lepton1_eta, lepton1_phi, 0.);
    thelepton2.SetPtEtaPhiM(lepton2_pT, lepton2_eta, lepton2_phi, 0.);
  }

  int iJetColl = 0 ;
  //// Do jets
  processJets(jetsColl, iEvent, iSetup, iJetColl) ;
  if (runSubJets_) {
          iJetColl = 1 ;
	  processJets(fatjetsColl, iEvent, iSetup, iJetColl) ;
  }

  //
  // Fill TTree
  //
  //$$
  if ( BitTrigger > 0 || Run < 0 ) {
    smalltree->Fill();
  }
  //$$
  //$$  if ( nMuon > 0 ) smalltree->Fill();
  //$$

}


float BTagAnalyzer::calculPtRel(const reco::Track& theMuon, const pat::Jet& theJet )
{
  double pmu = TMath::Sqrt( theMuon.px()*theMuon.px() + theMuon.py()*theMuon.py()  + theMuon.pz()*theMuon.pz() );

  double jetpx = 0 ;
  double jetpy = 0 ;
  double jetpz = 0 ;

  if( theJet.isCaloJet() ){
    jetpx = theJet.px() + theMuon.px();
    jetpy = theJet.py() + theMuon.py();
    jetpz = theJet.pz() + theMuon.pz();
  }else{
    jetpx = theJet.px();
    jetpy = theJet.py();
    jetpz = theJet.pz();
  }

  double jetp = TMath::Sqrt(jetpx*jetpx + jetpy*jetpy + jetpz*jetpz);

  double ptrel  = ( jetpx * theMuon.px()  + jetpy * theMuon.py() + jetpz * theMuon.pz() ) / jetp;
  ptrel = TMath::Sqrt( pmu * pmu  - ptrel * ptrel );

  return ptrel;
}

void BTagAnalyzer:: processJets (edm::Handle <PatJetCollection>& jetsColl, const edm::Event& iEvent, const edm::EventSetup& iSetup, int& iJetColl) {

  int numjet = 0;
  nMuon = 0;
  nPFElectron = 0;
  nPFMuon = 0;

  JetInfo[iJetColl].nJet = 0;

  //*********************************
  // Loop over the jets
  for ( PatJetCollection::const_iterator pjet = jetsColl->begin(); pjet != jetsColl->end(); ++pjet ) {

    double JES = pjet->pt()/pjet->correctedJet("Uncorrected").pt();

    double jetpt  = pjet->pt()  ;
    double jeteta = pjet->eta() ;
    double jetphi = pjet->phi() ;
    double ptjet  = jetpt;

    if ( ptjet < minJetPt_ || std::fabs( jeteta ) > maxJetEta_ ) continue;
    // if ( jetpt*JES > minJetPt_ && std::fabs( jeteta ) < maxJetEta_ ) ++Njets;

    // overlap removal with lepton from ttbar selection
    if (use_ttbar_filter_) {
      TLorentzVector thejet;
      thejet.SetPtEtaPhiM(ptjet, jeteta, jetphi, 0.);
      double deltaR1 = thejet.DeltaR(thelepton1);
      double deltaR2 = thejet.DeltaR(thelepton2);
      if (ttbar_chan>=0 && (deltaR1 < 0.5 || deltaR2 < 0.5)) continue;
    }
    // end of removal

    ++numjet;

    int flavour  =-1  ;

    if ( !isData_ ) {
      flavour = abs( pjet->partonFlavour() );
      if ( flavour >= 1 && flavour <= 3 ) flavour = 1;
      JetInfo[iJetColl].Jet_genpt[JetInfo[iJetColl].nJet]   = ( pjet->genJet()!=0 ? pjet->genJet()->pt() : -1. );
    }

    JetInfo[iJetColl].Jet_flavour[JetInfo[iJetColl].nJet] = flavour;
    JetInfo[iJetColl].Jet_eta[JetInfo[iJetColl].nJet]     = pjet->eta();
    JetInfo[iJetColl].Jet_phi[JetInfo[iJetColl].nJet]     = pjet->phi();
    JetInfo[iJetColl].Jet_pt[JetInfo[iJetColl].nJet]      = pjet->pt();

    retpf.set(false);
    JetInfo[iJetColl].Jet_looseID[JetInfo[iJetColl].nJet] = ( pfjetIDLoose( *pjet, retpf ) ? 1 : 0 );
    retpf.set(false);
    JetInfo[iJetColl].Jet_tightID[JetInfo[iJetColl].nJet] = ( pfjetIDTight( *pjet, retpf ) ? 1 : 0 );

    //     cout << "jet n" <<nJet<<endl;
    //     cout << "Jet_eta:" <<Jet_eta[JetInfo[iJetColl].nJet] << endl;
    //     cout << "Jet_phi:" << Jet_phi[JetInfo[iJetColl].nJet]<< endl;
    //     cout << "Jet_pt:" << Jet_pt[JetInfo[iJetColl].nJet]<< endl;

    JetInfo[iJetColl].Jet_jes[JetInfo[iJetColl].nJet]     = JES;
    JetInfo[iJetColl].Jet_residual[JetInfo[iJetColl].nJet]   = pjet->pt()/pjet->correctedJet("L3Absolute").pt();
    // JetInfo[iJetColl].Jet_residual[JetInfo[iJetColl].nJet]   = isData_ ? pjet->pt()/pjet->correctedJet("L3Absolute").pt() : 1.;

    float etajet = TMath::Abs( pjet->eta() );
    float phijet = pjet->phi();
    if (phijet < 0.) phijet += 2*TMath::Pi();


    if (iJetColl == 0) {

      //*****************************************************************
      // Taggers
      //*****************************************************************

      // Loop on Selected Tracks
      //
      const edm::RefVector<reco::TrackCollection>  &selected_tracks( pjet->tagInfoTrackIP("impactParameter")->selectedTracks() );
      const edm::RefVector<reco::TrackCollection>  &no_sel_tracks( pjet->tagInfoTrackIP("impactParameter")->tracks() );

      edm::ESHandle<TransientTrackBuilder> builder;
      iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);

      int ntagtracks;
      if (use_selected_tracks_) ntagtracks = pjet->tagInfoTrackIP("impactParameter")->probabilities(0).size();
      else ntagtracks=no_sel_tracks.size();

      JetInfo[iJetColl].Jet_ntracks[JetInfo[iJetColl].nJet] = ntagtracks;

      JetInfo[iJetColl].Jet_nFirstTrack[JetInfo[iJetColl].nJet]  = nTrack;
      JetInfo[iJetColl].Jet_nFirstTrkInc[JetInfo[iJetColl].nJet] = nTrkInc;
      int k=0;

      unsigned int trackSize = selected_tracks.size();
      if(!use_selected_tracks_)trackSize = no_sel_tracks.size();

      if (trackSize==0) continue;
      for (unsigned int itt=0; itt < trackSize; ++itt) {
        //pjet->tagInfoTrackIP("impactParameter")->probability(itt,0);

        reco::Track  ptrack;
        if(use_selected_tracks_) ptrack = *selected_tracks[itt];
        else ptrack= *no_sel_tracks[itt];

        TransientTrack transientTrack = builder->build(ptrack);
        GlobalVector direction(pjet->px(), pjet->py(), pjet->pz());

        //--------------------------------
        Double_t decayLength=-1;
        TrajectoryStateOnSurface closest =
          IPTools::closestApproachToJet(transientTrack.impactPointState(), *pv, direction, transientTrack.field());
        if (closest.isValid()){
          decayLength =  (closest.globalPosition()-   RecoVertex::convertPos(pv->position())).mag() ;
        }
        else{
          decayLength = -1;
        }

        Double_t distJetAxis =  IPTools::jetTrackDistance(transientTrack, direction, *pv).second.value();

        Track_dist[nTrack]     =distJetAxis;
        Track_length[nTrack]   =decayLength;

        Track_dxy[nTrack]      = ptrack.dxy(pv->position());
        Track_dz[nTrack]       = ptrack.dz(pv->position());
        Track_zIP[nTrack]      = ptrack.dz()-(*pv).z();

        float deta = ptrack.eta() - JetInfo[iJetColl].Jet_eta[JetInfo[iJetColl].nJet];
        float dphi = ptrack.phi() - JetInfo[iJetColl].Jet_phi[JetInfo[iJetColl].nJet];

        if ( dphi > TMath::Pi() ) dphi = 2.*TMath::Pi() - dphi;
        float deltaR = TMath::Sqrt(deta*deta + dphi*dphi);

        bool pass_cut_trk =false;

        //if (std::fabs(distJetAxis) < 0.07 && decayLength < 5.0 && deltaR < 0.3)  pass_cut_trk=true;
        if (std::fabs(distJetAxis) < 0.07 && decayLength < 5.0)  pass_cut_trk=true;

        // track selection

        if ( (use_selected_tracks_ && pass_cut_trk) ||  !use_selected_tracks_) {

          if ( use_selected_tracks_ ) {
            Track_IP2D[nTrack]     = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip2d.value();
            Track_IP2Dsig[nTrack]  = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip2d.significance();
            Track_IP[nTrack]       = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.value();
            Track_IPsig[nTrack]    = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.significance();
            Track_IP2Derr[nTrack]  = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip2d.error();
            Track_IPerr[nTrack]    = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.error();
            Track_Proba[nTrack]    = pjet->tagInfoTrackIP("impactParameter")->probabilities(0)[k];
          }
          else {
            Measurement1D ip2d    = IPTools::signedTransverseImpactParameter(transientTrack, direction, *pv).second;
            Measurement1D ip3d    = IPTools::signedImpactParameter3D(builder->build(ptrack), direction, *pv).second;
            Measurement1D ip2dsig = IPTools::signedTransverseImpactParameter(transientTrack, direction, *pv).first;
            Measurement1D ip3dsig = IPTools::signedImpactParameter3D(builder->build(ptrack), direction, *pv).first;

            Track_IP2D[nTrack]     = (ip2d.value());
            Track_IP2Dsig[nTrack]  = (ip2d.value())/(ip2d.error());
            Track_IP[nTrack]       = (ip3d.value());
            Track_IPsig[nTrack]    = (ip3d.value())/(ip3d.error());
            Track_IP2Derr[nTrack]  = (ip2d.error());
            Track_IPerr[nTrack]    = (ip3d.error());
            Track_Proba[nTrack]    = -1000;
          }

          Track_p[nTrack]        = ptrack.p();
          Track_pt[nTrack]       = ptrack.pt();
          Track_eta[nTrack]      = ptrack.eta();
          Track_phi[nTrack]      = ptrack.phi();
          // 	  Track_chi2[nTrack]     = (assotracks[itt])->chi2();
          Track_chi2[nTrack]     = ptrack.normalizedChi2();
          Track_charge[nTrack]   = ptrack.charge();

          Track_nHitAll[nTrack]  = ptrack.numberOfValidHits();
          Track_nHitPixel[nTrack]= ptrack.hitPattern().numberOfValidPixelHits();
          Track_nHitStrip[nTrack]= ptrack.hitPattern().numberOfValidStripHits();
          Track_nHitTIB[nTrack]  = ptrack.hitPattern().numberOfValidStripTIBHits();
          Track_nHitTID[nTrack]  = ptrack.hitPattern().numberOfValidStripTIDHits();
          Track_nHitTOB[nTrack]  = ptrack.hitPattern().numberOfValidStripTOBHits();
          Track_nHitTEC[nTrack]  = ptrack.hitPattern().numberOfValidStripTECHits();
          Track_nHitPXB[nTrack]  = ptrack.hitPattern().numberOfValidPixelBarrelHits();
          Track_nHitPXF[nTrack]  = ptrack.hitPattern().numberOfValidPixelEndcapHits();
          Track_isHitL1[nTrack]  = ptrack.hitPattern().hasValidHitInFirstPixelBarrel();

          Track_PV[nTrack] = 0;
          Track_SV[nTrack] = 0;
          Track_PVweight[nTrack] = 0.;
          Track_SVweight[nTrack] = 0.;

          Track_isfromSV[nTrack] = 0.;
          /*
             cout <<"track n" << nTrack<< endl;
             cout <<"Track_pt" <<Track_pt[nTrack] << endl;
             cout <<"Track_eta" <<Track_eta[nTrack] << endl;
             cout <<"Track_dz" << Track_dz[nTrack]<< endl;
             cout << "Track_length" << Track_length[nTrack]<< endl;
             cout << "Track_dist" <<Track_dist[nTrack] << endl;
             cout << "Track_IP2D" << Track_IP2D[nTrack]<< endl;
             cout << "Track_IP2Derr" << Track_IP2Derr[nTrack]<< endl;
             cout << "Track_IP" <<Track_IP[nTrack] << endl;
             cout << "Track_IPerr" << Track_IPerr[nTrack]<< endl;
             cout << "Track_nHitPixel" << Track_nHitPixel[nTrack]<< endl;
             cout << "Track_nHitStrip" <<Track_nHitStrip[nTrack] << endl;*/

          Track_history[nTrack] = 0;

          if ( useTrackHistory_ && !isData_ ) {
            TrackCategories::Flags theFlag ;
            if(use_selected_tracks_) theFlag  = classifier_.evaluate( pjet->tagInfoTrackIP("impactParameter")->selectedTracks()[k] ).flags();
            else                     theFlag  = classifier_.evaluate( pjet->tagInfoTrackIP("impactParameter")->tracks()[k] ).flags();

            if ( theFlag[TrackCategories::BWeakDecay] )	      Track_history[nTrack] += pow(10, -1 + 1);
            if ( theFlag[TrackCategories::CWeakDecay] )	      Track_history[nTrack] += pow(10, -1 + 2);
            if ( theFlag[TrackCategories::TauDecay] )	      Track_history[nTrack] += pow(10, -1 + 3);
            if ( theFlag[TrackCategories::ConversionsProcess] ) Track_history[nTrack] += pow(10, -1 + 4);
            if ( theFlag[TrackCategories::KsDecay] )	      Track_history[nTrack] += pow(10, -1 + 5);
            if ( theFlag[TrackCategories::LambdaDecay] )        Track_history[nTrack] += pow(10, -1 + 6);
            if ( theFlag[TrackCategories::HadronicProcess] )    Track_history[nTrack] += pow(10, -1 + 7);
            if ( theFlag[TrackCategories::Fake] ) 	      Track_history[nTrack] += pow(10, -1 + 8);
            if ( theFlag[TrackCategories::SharedInnerHits] )    Track_history[nTrack] += pow(10, -1 + 9);
          }

          // Track categories for Jet Probability calibration
          //$$$$ if ( Track_IPsig[nTrack] < 0 ) TrackProbaNeg->Fill(-Track_Proba[nTrack]);
          //$$$$ if ( Track_IPsig[nTrack] < 0 && PFJet80 ) TrackProbJet80->Fill(-Track_Proba[nTrack]);

          // 	  std::vector<float > theVectOrdered  = getTrackProbabilies((pjet->tagInfoTrackIP("impactParameter")->probabilities(0)), 0);
          // 	  JetInfo[iJetColl].Jet_TkProba[JetInfo[iJetColl].nJet]  = calculProbability( theVectOrdered );
          // 	  std::vector<float > theVectOrdered2 = getTrackProbabilies((pjet->tagInfoTrackIP("impactParameter")->probabilities(0)), 1);
          // 	  JetInfo[iJetColl].Jet_TkProbaP[JetInfo[iJetColl].nJet] = calculProbability( theVectOrdered2 );
          // 	  std::vector<float > theVectOrdered3 = getTrackProbabilies((pjet->tagInfoTrackIP("impactParameter")->probabilities(0)), 2);
          // 	  JetInfo[iJetColl].Jet_TkProbaN[JetInfo[iJetColl].nJet] = calculProbability( theVectOrdered3 );

          Track_category[nTrack] = -1;

          //for deltaR(track-jet) < 0.3
          TLorentzVector track4P, jet4P;
          track4P.SetPtEtaPhiM(ptrack.pt(), ptrack.eta(), ptrack.phi(), 0. );
          jet4P.SetPtEtaPhiM( pjet->pt(), pjet->eta(), pjet->phi(), pjet->energy() );


          if(jet4P.DeltaR(track4P) < 0.3 && std::fabs(distJetAxis) < 0.07 && decayLength < 5.0 ){

            //$$$$
            if ( Track_IPsig[nTrack] < 0 ) TrackProbaNeg->Fill(-Track_Proba[nTrack]);
            if ( Track_IPsig[nTrack] < 0 && PFJet80 ) TrackProbJet80->Fill(-Track_Proba[nTrack]);
            //$$$$

            if ( findCat( &ptrack, *&cat0 ) ) {
              Track_category[nTrack] = 0;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat0->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat0->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat0->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat0->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat1 ) && Track_category[nTrack] != 0 ) {
              Track_category[nTrack]  = 1;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat1->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat1->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat1->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat1->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat2 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 ) {
              Track_category[nTrack] = 2;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat2->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat2->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat2->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat2->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat3 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 ) {
              Track_category[nTrack] = 3;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat3->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat3->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat3->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat3->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat4 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 ) {
              Track_category[nTrack] = 4;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat4->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat4->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat4->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat4->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat5 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 && Track_category[nTrack] != 4 ) {
              Track_category[nTrack] = 5;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat5->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat5->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat5->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat5->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat6 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 && Track_category[nTrack] != 4 && Track_category[nTrack] != 5 ) {
              Track_category[nTrack] = 6;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat6->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat6->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat6->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat6->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat7 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 && Track_category[nTrack] != 4 && Track_category[nTrack] != 5 && Track_category[nTrack] != 6 ) {
              Track_category[nTrack] = 7;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat7->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat7->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat7->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat7->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat8 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 && Track_category[nTrack] != 4 && Track_category[nTrack] != 5 && Track_category[nTrack] != 6 && Track_category[nTrack] != 7 ) {
              Track_category[nTrack] = 8;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat8->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat8->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat8->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat8->Fill( -Track_Proba[nTrack] );
                }
              }
            }

            if ( findCat( &ptrack, *&cat9 ) && Track_category[nTrack] != 0 && Track_category[nTrack] != 1 && Track_category[nTrack] != 2 && Track_category[nTrack] != 3 && Track_category[nTrack]!= 4 && Track_category[nTrack] != 5 && Track_category[nTrack] != 6 && Track_category[nTrack] != 7 && Track_category[nTrack] != 8 ) {
              Track_category[nTrack] = 9;
              if ( Track_IPsig[nTrack] < 0 ) {
                IPSign_cat9->Fill( Track_IPsig[nTrack] );
                TrackProbaNeg_Cat9->Fill( -Track_Proba[nTrack] );
                if ( PFJet80 ) {
                  IPSign_cat9->Fill( -Track_IPsig[nTrack] );
                  TrackProbJet80_Cat9->Fill( -Track_Proba[nTrack] );
                }
              }
            }
          }

          ++nTrack;
        }
        if ( use_selected_tracks_ ) JetInfo[iJetColl].Jet_ntracks[JetInfo[iJetColl].nJet] = nTrack-JetInfo[iJetColl].Jet_nFirstTrack[JetInfo[iJetColl].nJet];

        if ( producePtRelTemplate_ ) {
          if ( ptrack.quality(reco::TrackBase::highPurity)
              // Remove the tracks that are pixel-less
              && ptrack.algo()!=(reco::TrackBase::iter4)
              && ptrack.algo()!=(reco::TrackBase::iter5)
              && deltaR < 0.4
              && ptrack.pt() > 5.
              && ptrack.numberOfValidHits() >= 11
              && ptrack.hitPattern().numberOfValidPixelHits() >= 2
              && ptrack.normalizedChi2() < 10
              && ptrack.trackerExpectedHitsOuter().numberOfHits() <= 2
              && ptrack.dz()-(*pv).z() < 1. ) {

            if ( use_selected_tracks_ ) {
              TrkInc_IP[nTrkInc]	  = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.value();
              TrkInc_IPsig[nTrkInc] = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.significance();
            }

            TrkInc_pt[nTrkInc]	= ptrack.pt();
            TrkInc_eta[nTrkInc]	= ptrack.eta();
            TrkInc_phi[nTrkInc]   = ptrack.phi();
            TrkInc_ptrel[nTrkInc] = calculPtRel( ptrack , *pjet);

            ++nTrkInc;
          }
        }
        ++k;

      } // end loop on tracks

      // // get the 4 highest positive and 4 lowest negative IP/sig tracks
      //     k = 0;
      //     int k1 = -1, k2 = -1, k3 = -1, k4 = -1;
      //     int n1 = -1, n2 = -1, n3 = -1, n4 = -1;
      //     float kip1 = -100., kip2 = -100., kip3 = -100., kip4 = -100.;
      //     float nip1 =  100., nip2 =  100., nip3 =  100., nip4 =  100.;
      //     for (unsigned int itt=0; itt < assotracks.size(); ++itt) {
      //       GlobalPoint maximumClose = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].closestToJetAxis;
      //       float decayLen = (maximumClose - (Pv_point)).mag();
      //       float distJetAxis = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].distanceToJetAxis.value();
      //       if ( std::fabs(distJetAxis) < 0.07 && decayLen < 5.0 ) {
      // 	float kip = pjet->tagInfoTrackIP("impactParameter")->impactParameterData()[k].ip3d.significance();
      // //         cout << k << " " << kip << endl;
      //         if ( kip > kip1 ) {
      //           k4 = k3;
      //           k3 = k2;
      //           k2 = k1;
      // 	  k1 = k;
      // 	  kip4 = kip3;
      // 	  kip3 = kip2;
      // 	  kip2 = kip1;
      // 	  kip1 = kip;
      // 	}
      //         else if ( kip > kip2 ) {
      //           k4 = k3;
      //           k3 = k2;
      //           k2 = k;
      // 	  kip4 = kip3;
      // 	  kip3 = kip2;
      // 	  kip2 = kip;
      // 	}
      //         else if ( kip > kip3 ) {
      //           k4 = k3;
      //           k3 = k;
      // 	  kip4 = kip3;
      // 	  kip3 = kip;
      // 	}
      //         else if ( kip > kip4 ) {
      //           k4 = k;
      // 	  kip4 = kip;
      // 	}
      //         if ( kip < nip1 ) {
      //           n4 = n3;
      //           n3 = n2;
      //           n2 = n1;
      // 	  n1 = k;
      // 	  nip4 = nip3;
      // 	  nip3 = nip2;
      // 	  nip2 = nip1;
      // 	  nip1 = kip;
      // 	}
      //         else if ( kip < nip2 ) {
      //           n4 = n3;
      //           n3 = n2;
      //           n2 = k;
      // 	  nip4 = nip3;
      // 	  nip3 = nip2;
      // 	  nip2 = kip;
      // 	}
      //         else if ( kip < nip3 ) {
      //           n4 = n3;
      //           n3 = k;
      // 	  nip4 = nip3;
      // 	  nip3 = kip;
      // 	}
      //         else if ( kip < nip4 ) {
      //           n4 = k;
      // 	  nip4 = kip;
      // 	}
      //       }
      //       ++k;
      //     } // end loop on tracks
      //     if ( kip1 < 0. ) k1 = -1;
      //     if ( kip2 < 0. ) k2 = -1;
      //     if ( kip3 < 0. ) k3 = -1;
      //     if ( kip4 < 0. ) k4 = -1;
      //     if ( nip1 > 0. ) n1 = -1;
      //     if ( nip2 > 0. ) n2 = -1;
      //     if ( nip3 > 0. ) n3 = -1;
      //     if ( nip4 > 0. ) n4 = -1;
      // //     cout << " ordered " << n1 << " " << n2 << " " << n3 << " " << n4 << " "
      // //                         << k4 << " " << k3 << " " << k2 << " " << k1 << endl;
      // //     cout << endl;
      //
      // // compute the invariant mass of this set of tracks
      //     float Tpx = 0., Tpy = 0., Tpz = 0., Tee = 0., Tmass = 0.;
      //     float Tmass2P = -1., Tmass3P = -1., Tmass4P = -1.;
      //     if ( k2 >= 0 ) {
      //       Tpx = (assotracks[k1])->px() + (assotracks[k2])->px();
      //       Tpy = (assotracks[k1])->py() + (assotracks[k2])->py();
      //       Tpz = (assotracks[k1])->pz() + (assotracks[k2])->pz();
      //       Tee = TMath::Sqrt( (assotracks[k1])->p()*(assotracks[k1])->p()+0.135*0.135)
      //           + TMath::Sqrt( (assotracks[k2])->p()*(assotracks[k2])->p()+0.135*0.135);
      //       Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //       if ( Tmass > 0. ) Tmass2P = TMath::Sqrt( Tmass );
      //       if ( k3 >= 0 ) {
      //         Tpx += (assotracks[k3])->px();
      //         Tpy += (assotracks[k3])->py();
      //         Tpz += (assotracks[k3])->pz();
      //         Tee += TMath::Sqrt( (assotracks[k3])->p()*(assotracks[k3])->p()+0.135*0.135);
      //         Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //         if ( Tmass > 0. ) Tmass3P = TMath::Sqrt( Tmass );
      //         if ( k4 >= 0 ) {
      //           Tpx += (assotracks[k4])->px();
      //           Tpy += (assotracks[k4])->py();
      //           Tpz += (assotracks[k4])->pz();
      //           Tee += TMath::Sqrt( (assotracks[k4])->p()*(assotracks[k4])->p()+0.135*0.135);
      //           Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //           if ( Tmass > 0. ) Tmass4P = TMath::Sqrt( Tmass );
      //         }
      //       }
      //     }
      //     Tpx = 0., Tpy = 0., Tpz = 0., Tee = 0., Tmass = 0.;
      //     float Tmass2N = -1., Tmass3N = -1., Tmass4N = -1.;
      //     if ( n2 >= 0 ) {
      //       Tpx = (assotracks[n1])->px() + (assotracks[n2])->px();
      //       Tpy = (assotracks[n1])->py() + (assotracks[n2])->py();
      //       Tpz = (assotracks[n1])->pz() + (assotracks[n2])->pz();
      //       Tee = TMath::Sqrt( (assotracks[n1])->p()*(assotracks[n1])->p()+0.135*0.135)
      //           + TMath::Sqrt( (assotracks[n2])->p()*(assotracks[n2])->p()+0.135*0.135);
      //       Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //       if ( Tmass > 0. ) Tmass2N = TMath::Sqrt( Tmass );
      //       if ( n3 >= 0 ) {
      //         Tpx += (assotracks[n3])->px();
      //         Tpy += (assotracks[n3])->py();
      //         Tpz += (assotracks[n3])->pz();
      //         Tee += TMath::Sqrt( (assotracks[n3])->p()*(assotracks[n3])->p()+0.135*0.135);
      //         Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //         if ( Tmass > 0. ) Tmass3N = TMath::Sqrt( Tmass );
      //         if ( n4 >= 0 ) {
      //           Tpx += (assotracks[n4])->px();
      //           Tpy += (assotracks[n4])->py();
      //           Tpz += (assotracks[n4])->pz();
      //           Tee += TMath::Sqrt( (assotracks[n4])->p()*(assotracks[n4])->p()+0.135*0.135);
      //           Tmass = Tee*Tee - Tpx*Tpx - Tpy*Tpy - Tpz*Tpz;
      //           if ( Tmass > 0. ) Tmass4N = TMath::Sqrt( Tmass );
      //         }
      //       }
      //     }

      JetInfo[iJetColl].Jet_nLastTrack[JetInfo[iJetColl].nJet]   = nTrack;
      JetInfo[iJetColl].Jet_nLastTrkInc[JetInfo[iJetColl].nJet]  = nTrkInc;
      // b-tagger discriminants
      float Proba  = pjet->bDiscriminator(jetPBJetTags_.c_str());
      float ProbaN = pjet->bDiscriminator(jetPNegBJetTags_.c_str());
      float ProbaP = pjet->bDiscriminator(jetPPosBJetTags_.c_str());

      float Bprob  = pjet->bDiscriminator(jetBPBJetTags_.c_str());
      float BprobN = pjet->bDiscriminator(jetBPNegBJetTags_.c_str());
      float BprobP = pjet->bDiscriminator(jetBPPosBJetTags_.c_str());

      float CombinedSvtx  = pjet->bDiscriminator(combinedSVBJetTags_.c_str());
      float CombinedSvtxN = pjet->bDiscriminator(combinedSVNegBJetTags_.c_str());
      float CombinedSvtxP = pjet->bDiscriminator(combinedSVPosBJetTags_.c_str());

      std::vector<const reco::BaseTagInfo*>  baseTagInfos_test;
      JetTagComputer::TagInfoHelper helper_test(baseTagInfos_test);
      baseTagInfos_test.push_back( pjet->tagInfoTrackIP("impactParameter") );
      baseTagInfos_test.push_back( pjet->tagInfoSecondaryVertex("secondaryVertex") );
      TaggingVariableList vars_test = computer->taggingVariables(helper_test);
      float JetVtxCategory = -9999.;
      if(vars_test.checkTag(reco::btau::vertexCategory)) JetVtxCategory = ( vars_test.get(reco::btau::vertexCategory) );

      float RetCombinedSvtx  = pjet->bDiscriminator(combinedSVRetrainedBJetTags_.c_str());
      float RetCombinedSvtxN = pjet->bDiscriminator(combinedSVRetrainedNegBJetTags_.c_str());
      float RetCombinedSvtxP = pjet->bDiscriminator(combinedSVRetrainedPosBJetTags_.c_str());

      float CombinedCSVJP  = pjet->bDiscriminator(combinedCSVJPBJetTags_.c_str());
      float CombinedCSVJPN = pjet->bDiscriminator(combinedCSVJPNegBJetTags_.c_str());
      float CombinedCSVJPP = pjet->bDiscriminator(combinedCSVJPPosBJetTags_.c_str());

      float CombinedCSVSL  = pjet->bDiscriminator(combinedCSVSLBJetTags_.c_str());
      float CombinedCSVSLN = pjet->bDiscriminator(combinedCSVSLNegBJetTags_.c_str());
      float CombinedCSVSLP = pjet->bDiscriminator(combinedCSVSLPosBJetTags_.c_str());

      float CombinedCSVJPSL  = pjet->bDiscriminator(combinedCSVJPSLBJetTags_.c_str());
      float CombinedCSVJPSLN = pjet->bDiscriminator(combinedCSVJPSLNegBJetTags_.c_str());
      float CombinedCSVJPSLP = pjet->bDiscriminator(combinedCSVJPSLPosBJetTags_.c_str());

      float SimpleIVF_HP    = pjet->bDiscriminator(simpleIVFSVHighPurBJetTags_.c_str());
      float SimpleIVF_HE    = pjet->bDiscriminator(simpleIVFSVHighEffBJetTags_.c_str());
      float DoubleIVF_HE    = pjet->bDiscriminator(doubleIVFSVHighEffBJetTags_.c_str());
      float CombinedIVF     = pjet->bDiscriminator(combinedIVFSVBJetTags_.c_str());
      float CombinedIVF_P   = pjet->bDiscriminator(combinedIVFSVPosBJetTags_.c_str());

      float Svtx    = pjet->bDiscriminator(simpleSVHighEffBJetTags_.c_str());
      float SvtxN   = pjet->bDiscriminator(simpleSVNegHighEffBJetTags_.c_str());
      float SvtxHP  = pjet->bDiscriminator(simpleSVHighPurBJetTags_.c_str());
      float SvtxNHP = pjet->bDiscriminator(simpleSVNegHighPurBJetTags_.c_str());

      float SoftM  = pjet->bDiscriminator(softPFMuonBJetTags_.c_str());
      float SoftMN = pjet->bDiscriminator(softPFMuonNegBJetTags_.c_str());
      float SoftMP = pjet->bDiscriminator(softPFMuonPosBJetTags_.c_str());

      // PFMuon information
      for (unsigned int leptIdx = 0; leptIdx < pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->leptons(); ++leptIdx) {

        PFMuon_IdxJet[nPFMuon]    = JetInfo[iJetColl].nJet;
        PFMuon_pt[nPFMuon]        = pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->lepton(leptIdx)->pt();
        PFMuon_eta[nPFMuon]       = pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->lepton(leptIdx)->eta();
        PFMuon_phi[nPFMuon]       = pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->lepton(leptIdx)->phi();
        PFMuon_ptrel[nPFMuon]     = calculPtRel( *(pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->lepton(leptIdx)), *pjet );
        PFMuon_ratio[nPFMuon]     = (pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->properties(leptIdx).ratio);
        PFMuon_ratioRel[nPFMuon]  = (pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->properties(leptIdx).ratioRel);
        PFMuon_deltaR[nPFMuon]    = (pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->properties(leptIdx).deltaR);
        PFMuon_IPsig[nPFMuon]     = (pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->properties(leptIdx).sip3d);

        PFMuon_GoodQuality[nPFMuon] = 0;
        int muIdx = matchMuon( pjet->tagInfoSoftLepton(softPFMuonTagInfos_.c_str())->lepton(leptIdx), muons );
        if ( muIdx != -1 && muons[muIdx].isGlobalMuon() == 1 ) {

          PFMuon_GoodQuality[nPFMuon] = 1;

          if (muons[muIdx].outerTrack()->hitPattern().numberOfValidMuonHits()>0 &&
              muons[muIdx].numberOfMatches()>1 && muons[muIdx].innerTrack()->hitPattern().numberOfValidHits()>10 &&
              muons[muIdx].innerTrack()->hitPattern().numberOfValidPixelHits()>1 &&
              muons[muIdx].innerTrack()->trackerExpectedHitsOuter().numberOfHits()<3 &&
              muons[muIdx].globalTrack()->normalizedChi2()<10. && muons[muIdx].innerTrack()->normalizedChi2()<10.)
            PFMuon_GoodQuality[nPFMuon] = 2;

        }
        ++nPFMuon;
      }

      // Old soft muon information for perfomance study safety
      for (unsigned int leptIdx = 0; leptIdx < pjet->tagInfoSoftLepton("softMuon")->leptons(); ++leptIdx){

        int muIdx = matchMuon( pjet->tagInfoSoftLepton("softMuon")->lepton(leptIdx), muons );
        //      if ( muIdx != -1 ) std::cout << " Jet " << nJet << " pt " << Jet_pt[JetInfo[iJetColl].nJet]*JES*Jet_residual[JetInfo[iJetColl].nJet] << " Global " << muons[muIdx].isGlobalMuon() << " Matches " << muons[muIdx].numberOfMatches() << std::endl;
        if ( muIdx != -1 && muons[muIdx].isGlobalMuon() == 1 ) {
          //if(muIdx > (muons.size())) cout << "wrong idx for muon " << muIdx << "  while muon size is " << pjet->tagInfoSoftLepton("softMuon")->leptons() << endl;
          //if (nMuon !=0) continue;
          Muon_IdxJet[nMuon] = JetInfo[iJetColl].nJet;
          Muon_ptrel[nMuon]   = calculPtRel( *(pjet->tagInfoSoftLepton("softMuon")->lepton(leptIdx)), *pjet );
          Muon_nTkHit[nMuon]    = muons[muIdx].innerTrack()->hitPattern().numberOfValidHits();
          Muon_nPixHit[nMuon]   = muons[muIdx].innerTrack()->hitPattern().numberOfValidPixelHits();
          Muon_nMuHit[nMuon]    = muons[muIdx].outerTrack()->hitPattern().numberOfValidMuonHits();
          Muon_nOutHit[nMuon]   = muons[muIdx].innerTrack()->trackerExpectedHitsOuter().numberOfHits();
          Muon_nMuHit[nMuon]    = muons[muIdx].outerTrack()->hitPattern().numberOfValidMuonHits();
          Muon_chi2[nMuon]      = muons[muIdx].globalTrack()->normalizedChi2() ;
          Muon_chi2Tk[nMuon]    = muons[muIdx].innerTrack()->normalizedChi2()  ;
          Muon_pt[nMuon]       = muons[muIdx].pt() 			   ;
          Muon_eta[nMuon]      = muons[muIdx].eta()			   ;
          Muon_phi[nMuon]      = muons[muIdx].phi()			   ;
          Muon_ratio[nMuon]    = (pjet->tagInfoSoftLepton("softMuon")->properties(leptIdx).ratio);
          Muon_deltaR[nMuon]   = (pjet->tagInfoSoftLepton("softMuon")->properties(leptIdx).deltaR);

          //         cout <<"Muon_ptrel:" << Muon_ptrel[nMuon]<< endl;
          //         cout <<"Muon_eta:" <<Muon_eta[nMuon] << endl;
          //         cout <<"Muon_pt:" << Muon_pt[nMuon]<< endl;
          //         cout <<"Muon_deltaR:" << Muon_deltaR[nMuon]<< endl;


          Muon_isGlobal[nMuon] = muons[muIdx].isGlobalMuon();
          Muon_nMatched[nMuon] = muons[muIdx].numberOfMatches() ;
          Muon_vz[nMuon]       = muons[muIdx].vz();
          // Muon_TrackIdx[nMuon] = getMuonTk( muons[muIdx].innerTrack()->pt());
          int mutkid = getMuonTk( muons[muIdx].innerTrack()->pt());
          Muon_IPsig[nMuon] = -100.;
          Muon_Proba[nMuon] = -100.;

          if ( mutkid >= 0 ) {
            //          Muon_IPsig[nMuon] = Track_IPsig[mutkid];
            Muon_Proba[nMuon] = Track_Proba[mutkid];
          }

          Muon_hist[nMuon] = 0;
          if ( useTrackHistory_ && !isData_ ) {
            TrackCategories::Flags theFlagP = classifier_.evaluate( pjet->tagInfoSoftLepton("softMuon")->lepton(leptIdx) ).flags();
            if ( theFlagP[TrackCategories::BWeakDecay] )         Muon_hist[nMuon] += int(pow(10., -1 + 1));
            if ( theFlagP[TrackCategories::CWeakDecay] )         Muon_hist[nMuon] += int(pow(10., -1 + 2));
            if ( theFlagP[TrackCategories::TauDecay] )	       Muon_hist[nMuon] += int(pow(10., -1 + 3));
            if ( theFlagP[TrackCategories::ConversionsProcess] ) Muon_hist[nMuon] += int(pow(10., -1 + 4));
            if ( theFlagP[TrackCategories::KsDecay] )	       Muon_hist[nMuon] += int(pow(10., -1 + 5));
            if ( theFlagP[TrackCategories::LambdaDecay] )        Muon_hist[nMuon] += int(pow(10., -1 + 6));
            if ( theFlagP[TrackCategories::HadronicProcess] )    Muon_hist[nMuon] += int(pow(10., -1 + 7));
            if ( theFlagP[TrackCategories::Fake] )	    Muon_hist[nMuon] += int(pow(10., -1 + 8));
            if ( theFlagP[TrackCategories::SharedInnerHits] )    Muon_hist[nMuon] += int(pow(10., -1 + 9));
          }

          //---------------------------------
          // calculate IP/s of muons' tracks
          //---------------------------------
          reco::TransientTrack tt = trackBuilder->build(muons[muIdx].innerTrack());
          GlobalVector direction(pjet->px(), pjet->py(), pjet->pz());
          Measurement1D ip   = IPTools::signedImpactParameter3D(tt, direction, (*primaryVertex)[0]).second;
          Measurement1D ip2d = IPTools::signedTransverseImpactParameter(tt, direction, (*primaryVertex)[0]).second;

          Muon_IP[nMuon]   = ip.value();
          Muon_IP2D[nMuon] = ip2d.value();
          Muon_IPsig[nMuon]   = (ip.value())/(ip.error());
          Muon_IP2Dsig[nMuon] = (ip2d.value())/(ip2d.error());



          // if ( muons[muIdx].numberOfMatches() < 1 ) {
          // std::cout << " Muon " << nMuon << " pt " << muons[muIdx].pt() << " eta " << muons[muIdx].eta() << " ptrel " << Muon_ptrel[nMuon] << std::endl;
          // std::cout << "     nTkHit " << Muon_nTkHit[nMuon] << std::endl;
          // std::cout << "     nPixHit " << Muon_nPixHit[nMuon] << std::endl;
          // std::cout << "     vz " << muons[muIdx].vz() << std::endl;
          // std::cout << "     globalTrackChi2 " << muons[muIdx].globalTrack()->normalizedChi2() << std::endl;
          // std::cout << "     innerTrackChi2 " << muons[muIdx].innerTrack()->normalizedChi2() << std::endl;linearImpactParameter
          // std::cout << "     nMuHit " << Muon_nMuHit[nMuon] << std::endl;
          // std::cout << "     nOutHit " << Muon_nOutHit[nMuon] << std::endl;
          // }
          ++nMuon;
        }
      }

      float SoftE  = pjet->bDiscriminator(softPFElectronBJetTags_.c_str());
      float SoftEN = pjet->bDiscriminator(softPFElectronNegBJetTags_.c_str());
      float SoftEP = pjet->bDiscriminator(softPFElectronPosBJetTags_.c_str());

      // PFElectron information
      for (unsigned int leptIdx = 0; leptIdx < pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->leptons(); ++leptIdx) {

        PFElectron_IdxJet[nPFElectron]    = JetInfo[iJetColl].nJet;
        PFElectron_pt[nPFElectron]        = pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->lepton(leptIdx)->pt();
        PFElectron_eta[nPFElectron]       = pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->lepton(leptIdx)->eta();
        PFElectron_phi[nPFElectron]       = pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->lepton(leptIdx)->phi();
        PFElectron_ptrel[nPFElectron]     = calculPtRel( *(pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->lepton(leptIdx)), *pjet );
        PFElectron_ratio[nPFElectron]     = (pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->properties(leptIdx).ratio);
        PFElectron_ratioRel[nPFElectron]  = (pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->properties(leptIdx).ratioRel);
        PFElectron_deltaR[nPFElectron]    = (pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->properties(leptIdx).deltaR);
        PFElectron_IPsig[nPFElectron]     = (pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->properties(leptIdx).sip3d);
        PFElectron_mva_e_pi[nPFElectron]       = -999.;//pjet->tagInfoSoftLepton(softPFElectronTagInfos_.c_str())->properties(leptIdx).mva_e_pi;

        ++nPFElectron;
      }

      // Jet information
      JetInfo[iJetColl].Jet_ProbaN[JetInfo[iJetColl].nJet]   = ProbaN;
      JetInfo[iJetColl].Jet_ProbaP[JetInfo[iJetColl].nJet]   = ProbaP;
      JetInfo[iJetColl].Jet_Proba[JetInfo[iJetColl].nJet]    = Proba;
      JetInfo[iJetColl].Jet_BprobN[JetInfo[iJetColl].nJet]   = BprobN;
      JetInfo[iJetColl].Jet_BprobP[JetInfo[iJetColl].nJet]   = BprobP;
      JetInfo[iJetColl].Jet_Bprob[JetInfo[iJetColl].nJet]    = Bprob;
      JetInfo[iJetColl].Jet_SvxN[JetInfo[iJetColl].nJet]     = SvtxN;
      JetInfo[iJetColl].Jet_Svx[JetInfo[iJetColl].nJet]      = Svtx;
      JetInfo[iJetColl].Jet_SvxNHP[JetInfo[iJetColl].nJet]   = SvtxNHP;
      JetInfo[iJetColl].Jet_SvxHP[JetInfo[iJetColl].nJet]    = SvtxHP;
      JetInfo[iJetColl].Jet_CombSvxN[JetInfo[iJetColl].nJet] = CombinedSvtxN;
      JetInfo[iJetColl].Jet_CombSvxP[JetInfo[iJetColl].nJet] = CombinedSvtxP;
      JetInfo[iJetColl].Jet_CombSvx[JetInfo[iJetColl].nJet]  = CombinedSvtx;
      JetInfo[iJetColl].Jet_RetCombSvxN[JetInfo[iJetColl].nJet] = RetCombinedSvtxN;
      JetInfo[iJetColl].Jet_RetCombSvxP[JetInfo[iJetColl].nJet] = RetCombinedSvtxP;
      JetInfo[iJetColl].Jet_RetCombSvx[JetInfo[iJetColl].nJet]  = RetCombinedSvtx;
      JetInfo[iJetColl].Jet_CombCSVJP_N[JetInfo[iJetColl].nJet] = CombinedCSVJPN;
      JetInfo[iJetColl].Jet_CombCSVJP_P[JetInfo[iJetColl].nJet] = CombinedCSVJPP;
      JetInfo[iJetColl].Jet_CombCSVJP[JetInfo[iJetColl].nJet]  = CombinedCSVJP;
      JetInfo[iJetColl].Jet_CombCSVSL_N[JetInfo[iJetColl].nJet] = CombinedCSVSLN;
      JetInfo[iJetColl].Jet_CombCSVSL_P[JetInfo[iJetColl].nJet] = CombinedCSVSLP;
      JetInfo[iJetColl].Jet_CombCSVSL[JetInfo[iJetColl].nJet]  = CombinedCSVSL;
      JetInfo[iJetColl].Jet_CombCSVJPSL_N[JetInfo[iJetColl].nJet] = CombinedCSVJPSLN;
      JetInfo[iJetColl].Jet_CombCSVJPSL_P[JetInfo[iJetColl].nJet] = CombinedCSVJPSLP;
      JetInfo[iJetColl].Jet_CombCSVJPSL[JetInfo[iJetColl].nJet]  = CombinedCSVJPSL;
      JetInfo[iJetColl].Jet_SimpIVF_HP[JetInfo[iJetColl].nJet]  = SimpleIVF_HP;
      JetInfo[iJetColl].Jet_SimpIVF_HE[JetInfo[iJetColl].nJet]  = SimpleIVF_HE;
      JetInfo[iJetColl].Jet_DoubIVF_HE[JetInfo[iJetColl].nJet]  = DoubleIVF_HE;
      JetInfo[iJetColl].Jet_CombIVF[JetInfo[iJetColl].nJet]     = CombinedIVF;
      JetInfo[iJetColl].Jet_CombIVF_P[JetInfo[iJetColl].nJet]   = CombinedIVF_P;
      JetInfo[iJetColl].Jet_SoftMuN[JetInfo[iJetColl].nJet]  = SoftMN;
      JetInfo[iJetColl].Jet_SoftMuP[JetInfo[iJetColl].nJet]  = SoftMP;
      JetInfo[iJetColl].Jet_SoftMu[JetInfo[iJetColl].nJet]   = SoftM;
      JetInfo[iJetColl].Jet_SoftElN[JetInfo[iJetColl].nJet]  = SoftEN;
      JetInfo[iJetColl].Jet_SoftElP[JetInfo[iJetColl].nJet]  = SoftEP;
      JetInfo[iJetColl].Jet_SoftEl[JetInfo[iJetColl].nJet]   = SoftE;

      JetInfo[iJetColl].Jet_VtxCat[JetInfo[iJetColl].nJet] = JetVtxCategory;

      std::vector<TrackIPTagInfo::TrackIPData>  ipdata = pjet->tagInfoTrackIP("impactParameter")->impactParameterData();

      TrackRefVector tracks( pjet->tagInfoTrackIP("impactParameter")->selectedTracks() );


      std::vector<std::size_t> indexes( sortedIndexes(ipdata) );

      TrackCategories::Flags flags1P;
      TrackCategories::Flags flags2P;
      TrackCategories::Flags flags3P;
      TrackCategories::Flags flags1N;
      TrackCategories::Flags flags2N;
      TrackCategories::Flags flags3N;
      int idSize = 0;

      if ( useTrackHistory_ && indexes.size() != 0 && !isData_ ) {

        idSize = indexes.size();
        flags1P = classifier_.evaluate( tracks[indexes[0]] ).flags();
        if(idSize > 1) flags2P = classifier_.evaluate( tracks[indexes[1]] ).flags();
        if(idSize > 2) flags3P = classifier_.evaluate( tracks[indexes[2]] ).flags();
        flags1N = classifier_.evaluate( tracks[indexes[idSize-1]] ).flags();
        if(idSize > 1) flags2N = classifier_.evaluate( tracks[indexes[idSize-2]] ).flags();
        if(idSize > 2) flags3N = classifier_.evaluate( tracks[indexes[idSize-3]] ).flags();
      }

      JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet]   = pjet->bDiscriminator(trackCHEBJetTags_.c_str());
      JetInfo[iJetColl].Jet_Ip2N[JetInfo[iJetColl].nJet]   = pjet->bDiscriminator(trackCNegHEBJetTags_.c_str());

      JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet]   = pjet->bDiscriminator(trackCHPBJetTags_.c_str());
      JetInfo[iJetColl].Jet_Ip3N[JetInfo[iJetColl].nJet]   = pjet->bDiscriminator(trackCNegHPBJetTags_.c_str());

      //cout << "TCHE" <<JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet] << endl;
      //cout << "TCHP" << JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet]<< endl;


      //     JetInfo[iJetColl].Jet_Ip1P[JetInfo[iJetColl].nJet]  = -100;
      //     JetInfo[iJetColl].Jet_Ip1N[JetInfo[iJetColl].nJet]  =  100;
      //     for (std::vector<TrackIPTagInfo::TrackIPData>::const_iterator itipdata = ipdata.begin();
      // 	 itipdata != ipdata.end(); ++itipdata) {
      //
      //       float decayLen = ((*itipdata).closestToJetAxis - (Pv_point)).mag();
      //       float distJetAxis = (*itipdata).distanceToJetAxis.value();
      //       if ( abs(distJetAxis) > 0.07 || decayLen > 5.0 ) continue;
      //       double ip3D = (*itipdata).ip3d.significance();
      //       if( ip3D > Jet_Ip1P[JetInfo[iJetColl].nJet] ) Jet_Ip1P[JetInfo[iJetColl].nJet] = ip3D;
      //       if( ip3D < Jet_Ip1N[JetInfo[iJetColl].nJet] ) Jet_Ip1N[JetInfo[iJetColl].nJet] = ip3D;
      //     }
      //     Jet_Ip1N[JetInfo[iJetColl].nJet] = -Jet_Ip1N[JetInfo[iJetColl].nJet];

      //     if ( Jet_Ip2P[JetInfo[iJetColl].nJet] != kip2  || Jet_Ip3P[JetInfo[iJetColl].nJet] != kip3
      //       || Jet_Ip2N[JetInfo[iJetColl].nJet] !=-nip2  || Jet_Ip3N[JetInfo[iJetColl].nJet] !=-nip3 ){
      //  cout << " ERROR " << n2 << " " << n3 << " " << k3 << " " << k2 << endl;
      //  cout << "       " << nip2 << " " << nip3 << " " << kip3 << " " << kip2 << endl;
      //  cout << "       " << -Jet_Ip2N[JetInfo[iJetColl].nJet] << " " << -Jet_Ip3N[JetInfo[iJetColl].nJet] << " "
      //                    <<  Jet_Ip3P[JetInfo[iJetColl].nJet] << " " <<  Jet_Ip2P[JetInfo[iJetColl].nJet] << endl;
      //  cout << endl;
      //     }

      //     JetInfo[iJetColl].Jet_Ip4P[JetInfo[iJetColl].nJet]  = kip4;
      //     JetInfo[iJetColl].Jet_Ip4N[JetInfo[iJetColl].nJet]  = nip4;
      //     Jet_Ip4N[JetInfo[iJetColl].nJet] = -Jet_Ip4N[JetInfo[iJetColl].nJet];
      //     Jet_Mass4N[JetInfo[iJetColl].nJet] = Tmass4N;
      //     Jet_Mass4P[JetInfo[iJetColl].nJet] = Tmass4P;

      //     cout << " Jet " << nJet << " pt " << Jet_pt[JetInfo[iJetColl].nJet]*JES*Jet_residual[JetInfo[iJetColl].nJet] << " "
      //          << n1 << " " << n2 << " " << n3 << " " << n4 << " "
      //          << k4 << " " << k3 << " " << k2 << " " << k1 << endl;
      //     cout << "    Mass neg " << Jet_Mass2N[JetInfo[iJetColl].nJet] << " " << Jet_Mass3N[JetInfo[iJetColl].nJet] << " " << Jet_Mass4N[JetInfo[iJetColl].nJet]
      //          << "    Mass pos " << Jet_Mass2P[JetInfo[iJetColl].nJet] << " " << Jet_Mass3P[JetInfo[iJetColl].nJet] << " " << Jet_Mass4P[JetInfo[iJetColl].nJet] << endl;

      //*****************************************************************
      //get track histories for 1st, 2nd and 3rd track (TC)
      //*****************************************************************
      JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] = 0;
      JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] = 0;
      JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] = 0;

      // Track history
      if ( useTrackHistory_ && indexes.size()!=0 && !isData_ ) {
        if ( flags1P[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 1));
        if ( flags1P[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 2));
        if ( flags1P[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 3));
        if ( flags1P[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 4));
        if ( flags1P[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 5));
        if ( flags1P[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 6));
        if ( flags1P[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 7));
        if ( flags1P[TrackCategories::Fake] )	          JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 8));
        if ( flags1P[TrackCategories::SharedInnerHits] )	  JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 9));
        if ( idSize > 1 ) {
          if ( flags2P[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 1));
          if ( flags2P[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 2));
          if ( flags2P[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 3));
          if ( flags2P[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 4));
          if ( flags2P[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 5));
          if ( flags2P[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 6));
          if ( flags2P[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 7));
          if ( flags2P[TrackCategories::Fake] )	            JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 8));
          if ( flags2P[TrackCategories::SharedInnerHits] )    JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 9));
        }
        if ( idSize > 2 ) {
          if ( flags3P[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 1));
          if ( flags3P[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 2));
          if ( flags3P[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 3));
          if ( flags3P[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 4));
          if ( flags3P[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 5));
          if ( flags3P[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 6));
          if ( flags3P[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 7));
          if ( flags3P[TrackCategories::Fake] )	            JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 8));
          if ( flags3P[TrackCategories::SharedInnerHits] )    JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += int(pow(10., -1 + 9));
        }
        if ( flags1N[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 1));
        if ( flags1N[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 2));
        if ( flags1N[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 3));
        if ( flags1N[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 4));
        if ( flags1N[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 5));
        if ( flags1N[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 6));
        if ( flags1N[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 7));
        if ( flags1N[TrackCategories::Fake] )	          JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 8));
        if ( flags1N[TrackCategories::SharedInnerHits] )	  JetInfo[iJetColl].Jet_hist1[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 9));
        if ( idSize > 1 ) {
          if ( flags2N[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 1));
          if ( flags2N[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 2));
          if ( flags2N[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 3));
          if ( flags2N[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 4));
          if ( flags2N[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 5));
          if ( flags2N[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 6));
          if ( flags2N[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 7));
          if ( flags2N[TrackCategories::Fake] )	            JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 8));
          if ( flags2N[TrackCategories::SharedInnerHits] )    JetInfo[iJetColl].Jet_hist2[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 9));
        }
        if ( idSize > 2 ) {
          if ( flags3N[TrackCategories::BWeakDecay] )         JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 1));
          if ( flags3N[TrackCategories::CWeakDecay] )         JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 2));
          if ( flags3N[TrackCategories::TauDecay] )           JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 3));
          if ( flags3N[TrackCategories::ConversionsProcess] ) JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 4));
          if ( flags3N[TrackCategories::KsDecay] )            JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 5));
          if ( flags3N[TrackCategories::LambdaDecay] )        JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 6));
          if ( flags3N[TrackCategories::HadronicProcess] )    JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 7));
          if ( flags3N[TrackCategories::Fake] )	            JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 8));
          if ( flags3N[TrackCategories::SharedInnerHits] )    JetInfo[iJetColl].Jet_hist3[JetInfo[iJetColl].nJet] += 2*int(pow(10., -1 + 9));
        }
      }

      //*****************************************************************
      //get track histories of tracks in jets (for Jet Proba)
      //*****************************************************************
      JetInfo[iJetColl].Jet_histJet[JetInfo[iJetColl].nJet] = 0;

      if(useTrackHistory_ && isData_!=1){
        TrackRefVector jetProbTracks( pjet->tagInfoTrackIP("impactParameter")->selectedTracks() );

        cap0=0; cap1=0; cap2=0; cap3=0; cap4=0; cap5=0; cap6=0; cap7=0; cap8=0;
        can0=0; can1=0; can2=0; can3=0; can4=0; can5=0; can6=0; can7=0; can8=0;

        for (unsigned int i=0; i<jetProbTracks.size(); ++i) {
          reco::TrackIPTagInfo::TrackIPData ip = (pjet->tagInfoTrackIP("impactParameter")->impactParameterData())[i];

          if ( ip.ip3d.significance() > 0 ) {
            TrackCategories::Flags theFlag = classifier_.evaluate( jetProbTracks[i] ).flags();
            if ( theFlag[TrackCategories::BWeakDecay] )	      cap0 = 1;
            if ( theFlag[TrackCategories::CWeakDecay] )	      cap1 = 1;
            if ( theFlag[TrackCategories::TauDecay] )	      cap2 = 1;
            if ( theFlag[TrackCategories::ConversionsProcess] ) cap3 = 1;
            if ( theFlag[TrackCategories::KsDecay] )	      cap4 = 1;
            if ( theFlag[TrackCategories::LambdaDecay] )        cap5 = 1;
            if ( theFlag[TrackCategories::HadronicProcess] )    cap6 = 1;
            if ( theFlag[TrackCategories::Fake] ) 	      cap7 = 1;
            if ( theFlag[TrackCategories::SharedInnerHits] )    cap8 = 1;
          }
          else {
            TrackCategories::Flags theFlag = classifier_.evaluate( jetProbTracks[i] ).flags();
            if ( theFlag[TrackCategories::BWeakDecay] )	      can0 = 2;
            if ( theFlag[TrackCategories::CWeakDecay] )	      can1 = 2;
            if ( theFlag[TrackCategories::TauDecay] )	      can2 = 2;
            if ( theFlag[TrackCategories::ConversionsProcess] ) can3 = 2;
            if ( theFlag[TrackCategories::KsDecay] )	      can4 = 2;
            if ( theFlag[TrackCategories::LambdaDecay] )        can5 = 2;
            if ( theFlag[TrackCategories::HadronicProcess] )    can6 = 2;
            if ( theFlag[TrackCategories::Fake] ) 	      can7 = 2;
            if ( theFlag[TrackCategories::SharedInnerHits] )    can8 = 2;
          }
        }
        JetInfo[iJetColl].Jet_histJet[JetInfo[iJetColl].nJet] =  cap0+can0 + (cap1+can1)*10 + (cap2+can2)*100
          + (cap3+can3)*1000     + (cap4+can4)*10000
          + (cap5+can5)*100000   + (cap6+can6)*1000000
          + (cap7+can7)*10000000 + (cap8+can8)*100000000;
      }

      //*****************************************************************
      //get track histories  associated to sec. vertex (for simple SV)
      //*****************************************************************
      JetInfo[iJetColl].Jet_histSvx[JetInfo[iJetColl].nJet] = 0;
      JetInfo[iJetColl].Jet_nFirstSV[JetInfo[iJetColl].nJet]  = nSV;
      JetInfo[iJetColl].Jet_SV_multi[JetInfo[iJetColl].nJet]  = pjet->tagInfoSecondaryVertex("secondaryVertex")->nVertices();

      if ( produceJetProbaTree_  &&  JetInfo[iJetColl].Jet_SV_multi[JetInfo[iJetColl].nJet]> 0) {


        SV_x[nSV]    = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).x();
        SV_y[nSV]    = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).y();
        SV_z[nSV]    = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).z();
        SV_ex[nSV]   = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).xError();
        SV_ey[nSV]   = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).yError();
        SV_ez[nSV]   = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).zError();
        SV_chi2[nSV] = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).chi2();
        SV_ndf[nSV]  = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).ndof();

        //cout << "pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).ndof() " << pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).ndof() << endl;

        SV_flight[nSV]	  = pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDistance(0).value();
        SV_flightErr[nSV]   = pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDistance(0).error();
        SV_flight2D[nSV]	  = pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDistance(0, true).value();
        SV_flight2DErr[nSV] = pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDistance(0, true).error();
        SV_nTrk[nSV]        = pjet->tagInfoSecondaryVertex("secondaryVertex")->nVertexTracks();
        SV_nTrk_firstVxt[nSV] =  pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).tracksSize();

        // ------------------------added by Camille ---------------------------------------------------------------------------------------//


        edm::RefToBase<Jet> jet = pjet->tagInfoTrackIP("impactParameter")->jet();


        std::vector<const reco::BaseTagInfo*>  baseTagInfos;
        JetTagComputer::TagInfoHelper helper(baseTagInfos);
        baseTagInfos.push_back( pjet->tagInfoTrackIP("impactParameter") );
        baseTagInfos.push_back( pjet->tagInfoSecondaryVertex("secondaryVertex") );

        TaggingVariableList vars = computer->taggingVariables(helper);
        if(vars.checkTag(reco::btau::vertexEnergyRatio)) SV_energy_ratio[nSV] = ( vars.get(reco::btau::vertexEnergyRatio) );
        else SV_energy_ratio[nSV] = ( -9999 );

        if(vars.checkTag(reco::btau::vertexJetDeltaR)) SV_deltaR_jet[nSV] = (  vars.get( reco::btau::vertexJetDeltaR) );
        else SV_deltaR_jet[nSV] = ( -9999 );

        if(vars.checkTag(reco::btau::trackSip3dSigAboveCharm) ) SV_aboveC[nSV] = (  vars.get( reco::btau::trackSip3dSigAboveCharm ));
        else SV_aboveC[nSV] = (  -9999 );

        if(vars.checkTag(reco::btau::vertexMass)) SV_mass[nSV] = ( vars.get( reco::btau::vertexMass));
        else  SV_mass[nSV] = ( -9999 );

        Int_t totcharge=0;
        TrackKinematics vertexKinematics;

        const Vertex &vertex = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0);

        Bool_t hasRefittedTracks = vertex.hasRefittedTracks();

        TrackRefVector tracks = pjet->tagInfoSecondaryVertex("secondaryVertex")->vertexTracks(0);
        for(TrackRefVector::const_iterator track = tracks.begin();
            track != tracks.end(); ++track) {
          Double_t w = pjet->tagInfoSecondaryVertex("secondaryVertex")->trackWeight(0, *track);
          if (w < 0.5)
            continue;
          if (hasRefittedTracks) {
            Track actualTrack = vertex.refittedTrack(*track);
            vertexKinematics.add(actualTrack, w);
            totcharge+=actualTrack.charge();
          }
          else {
            vertexKinematics.add(**track, w);
            const reco::Track& mytrack = **track;
            totcharge+=mytrack.charge();
          }
        }


        math::XYZVector jetDir = jet->momentum().Unit();

        Bool_t useTrackWeights = true;
        math::XYZTLorentzVector vertexSum = useTrackWeights
          ? vertexKinematics.weightedVectorSum()
          : vertexKinematics.vectorSum();

        math::XYZTLorentzVector flightDir( pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDirection(0).x(), pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDirection(0).y(), pjet->tagInfoSecondaryVertex("secondaryVertex")->flightDirection(0).z(), 0  );
        SV_deltaR_sum_jet[nSV] = ( Geom::deltaR(vertexSum, jetDir) );
        SV_deltaR_sum_dir[nSV] = ( Geom::deltaR( flightDir, vertexSum ) );
        SV_vtx_pt[nSV] = vertex.p4().pt();
        SV_vtx_eta[nSV] = vertex.p4().eta();
        SV_vtx_phi[nSV] = vertex.p4().phi();



        jetVertex = (math::XYZVector(jet->vx(),jet->vy(),jet->vz()));

        Line::PositionType pos(GlobalPoint(vertex.p4().x(),vertex.p4().y(),vertex.p4().z()));
        Line::DirectionType dir(GlobalVector(flightDir.px(),flightDir.py(),flightDir.pz()));
        Line trackline(pos,dir);
        // get the Jet  line
        Line::PositionType pos2(GlobalPoint(jetVertex.x(),jetVertex.y(),jetVertex.z()));
        Line::DirectionType dir2(GlobalVector(jetDir.x(),jetDir.y(),jetDir.z()));
        Line jetline(pos2,dir2);
        // now compute the distance between the two lines
        SV_vtxDistJetAxis[nSV] = (jetline.distance(trackline)).mag();


        // total charge at the secondary vertex
        SV_totCharge[nSV]=totcharge;
        // ------------------------end added ---------------------------------------------------------------------------------------//


        setTracksPV( &pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0), false );
        ++nSV;
      }
      JetInfo[iJetColl].Jet_nLastSV[JetInfo[iJetColl].nJet]  = nSV;

      cap0=0; cap1=0; cap2=0; cap3=0; cap4=0; cap5=0; cap6=0; cap7=0; cap8=0;
      can0=0; can1=0; can2=0; can3=0; can4=0; can5=0; can6=0; can7=0; can8=0;

      TrackRefVector  svxPostracks( pjet->tagInfoSecondaryVertex("secondaryVertex")->vertexTracks(0) );

      if ( useTrackHistory_ && !isData_ ) {
        for (unsigned int i=0; i<svxPostracks.size(); ++i) {
          TrackCategories::Flags theFlag = classifier_.evaluate( svxPostracks[i] ).flags();
          if ( theFlag[TrackCategories::BWeakDecay] )         cap0 = 1;
          if ( theFlag[TrackCategories::CWeakDecay] )         cap1 = 1;
          if ( theFlag[TrackCategories::TauDecay] )           cap2 = 1;
          if ( theFlag[TrackCategories::ConversionsProcess] ) cap3 = 1;
          if ( theFlag[TrackCategories::KsDecay] )            cap4 = 1;
          if ( theFlag[TrackCategories::LambdaDecay] )        cap5 = 1;
          if ( theFlag[TrackCategories::HadronicProcess] )    cap6 = 1;
          if ( theFlag[TrackCategories::Fake] )	            cap7 = 1;
          if ( theFlag[TrackCategories::SharedInnerHits] )    cap8 = 1;
        }
      }

      TrackRefVector svxNegtracks( pjet->tagInfoSecondaryVertex("secondaryVertexNegative")->vertexTracks(0) );

      if ( useTrackHistory_ && !isData_ ) {
        for (unsigned int i=0; i<svxNegtracks.size(); ++i) {
          TrackCategories::Flags theFlag = classifier_.evaluate( svxNegtracks[i] ).flags();
          if ( theFlag[TrackCategories::BWeakDecay] )         can0 = 2;
          if ( theFlag[TrackCategories::CWeakDecay] )         can1 = 2;
          if ( theFlag[TrackCategories::TauDecay] )           can2 = 2;
          if ( theFlag[TrackCategories::ConversionsProcess] ) can3 = 2;
          if ( theFlag[TrackCategories::KsDecay] )            can4 = 2;
          if ( theFlag[TrackCategories::LambdaDecay] )        can5 = 2;
          if ( theFlag[TrackCategories::HadronicProcess] )    can6 = 2;
          if ( theFlag[TrackCategories::Fake] )	            can7 = 2;
          if ( theFlag[TrackCategories::SharedInnerHits] )    can8 = 2;
        }
        JetInfo[iJetColl].Jet_histSvx[JetInfo[iJetColl].nJet] =  cap0+can0 + (cap1+can1)*10 + (cap2+can2)*100
          + (cap3+can3)*1000     + (cap4+can4)*10000
          + (cap5+can5)*100000   + (cap6+can6)*1000000
          + (cap7+can7)*10000000 + (cap8+can8)*100000000;
      }

      //    JetInfo[iJetColl].Jet_SvxNTracks[JetInfo[iJetColl].nJet] = svxNegtracks.size();
      //    JetInfo[iJetColl].Jet_SvxTracks[JetInfo[iJetColl].nJet]  = svxPostracks.size();

      float SVmass = 0.;
      if ( pjet->tagInfoSecondaryVertex("secondaryVertex")->nVertices() > 0) {
        SVmass = pjet->tagInfoSecondaryVertex("secondaryVertex")->secondaryVertex(0).p4().mass();
      }
      JetInfo[iJetColl].Jet_SvxMass[JetInfo[iJetColl].nJet] = SVmass;
      // std:: << " Jet " << nJet << " pt " << Jet_pt[JetInfo[iJetColl].nJet]*JES*Jet_residual[JetInfo[iJetColl].nJet] << " SVmass " << SVmass << std::endl;


      //*********************************
      // Jet analysis

      hData_All_NTracks->Fill( ntagtracks ) ;
      hData_All_JetPt->Fill( ptjet );
      hData_All_JetEta->Fill( etajet );



      //*****************************************************************
      //define positive and negative tags
      //*****************************************************************
      float varpos = -1000.;
      float varneg = -1000.;

      if ( selTagger_ == 0 ) {       // jet proba
        if ( ProbaP > 0 ) varpos = 20.*ProbaP;
        if ( ProbaN > 0 ) varneg = 20.*ProbaN;
      }
      else if ( selTagger_ == 2 ) {     // TC High Eff.
        if ( JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet] > 0. ) varpos = JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet];
        if ( JetInfo[iJetColl].Jet_Ip2N[JetInfo[iJetColl].nJet] > 0. ) varneg = JetInfo[iJetColl].Jet_Ip2N[JetInfo[iJetColl].nJet];
      }
      else if ( selTagger_ == 3 ) {     // TC High Pure.
        if ( JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet] > 0. ) varpos = JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet];
        if ( JetInfo[iJetColl].Jet_Ip3N[JetInfo[iJetColl].nJet] > 0. ) varneg = JetInfo[iJetColl].Jet_Ip3N[JetInfo[iJetColl].nJet];
      }
      else if ( selTagger_ == 4 ) {    // SSVHE
        if  (JetInfo[iJetColl].Jet_Svx[JetInfo[iJetColl].nJet] > 1)   varpos =  10.*JetInfo[iJetColl].Jet_Svx[JetInfo[iJetColl].nJet] - 10.;
        if  (JetInfo[iJetColl].Jet_SvxN[JetInfo[iJetColl].nJet] < -1) varneg = -10.*JetInfo[iJetColl].Jet_SvxN[JetInfo[iJetColl].nJet] - 10.;
      }
      else if ( selTagger_ == 5 ) {    // SV combined
        if  (JetInfo[iJetColl].Jet_CombSvx[JetInfo[iJetColl].nJet] > 0)  varpos = 50.*JetInfo[iJetColl].Jet_CombSvx[JetInfo[iJetColl].nJet];
        if  (JetInfo[iJetColl].Jet_CombSvxN[JetInfo[iJetColl].nJet] > 0) varneg = 50.*JetInfo[iJetColl].Jet_CombSvxN[JetInfo[iJetColl].nJet];
      }
      else if ( selTagger_ == 6 ) {    // soft muon ptrel
        if  ( JetInfo[iJetColl].Jet_SoftMu[JetInfo[iJetColl].nJet]  > 0) varpos =  5*JetInfo[iJetColl].Jet_SoftMu[JetInfo[iJetColl].nJet];
        if  ( JetInfo[iJetColl].Jet_SoftMuN[JetInfo[iJetColl].nJet] < 0) varneg = -5*JetInfo[iJetColl].Jet_SoftMuN[JetInfo[iJetColl].nJet];
      }
      else if ( selTagger_ == 7 ) {    // SSVHP
        if  (JetInfo[iJetColl].Jet_SvxHP[JetInfo[iJetColl].nJet] > 1)   varpos =  10.*JetInfo[iJetColl].Jet_SvxHP[JetInfo[iJetColl].nJet] - 10.;
        if  (JetInfo[iJetColl].Jet_SvxNHP[JetInfo[iJetColl].nJet] < -1) varneg = -10.*JetInfo[iJetColl].Jet_SvxNHP[JetInfo[iJetColl].nJet] - 10.;
      }


      hData_NTracks->Fill( ntagtracks ) ;
      hData_JetPt->Fill( ptjet );
      hData_JetEta->Fill( etajet );



      //*********************************
      // Tagging

      if ( varneg > 0 ) hData_Tagger->Fill(-varneg );
      if ( varpos > 0 ) hData_Tagger->Fill( varpos );

      if ( JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet] > 0 )            hData_Tagger_TCHE->Fill( JetInfo[iJetColl].Jet_Ip2P[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_Ip2N[JetInfo[iJetColl].nJet] > 0 )            hData_Tagger_TCHE->Fill(-JetInfo[iJetColl].Jet_Ip2N[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet] > 0 )            hData_Tagger_TCHP->Fill( JetInfo[iJetColl].Jet_Ip3P[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_Ip3N[JetInfo[iJetColl].nJet] > 0 )            hData_Tagger_TCHP->Fill(-JetInfo[iJetColl].Jet_Ip3N[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_ProbaP[JetInfo[iJetColl].nJet] > 0 )          hData_Tagger_JP->Fill( 10*JetInfo[iJetColl].Jet_ProbaP[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_ProbaN[JetInfo[iJetColl].nJet] > 0 )          hData_Tagger_JP->Fill(-10*JetInfo[iJetColl].Jet_ProbaN[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_Svx[JetInfo[iJetColl].nJet]  >  1 )           hData_Tagger_SSVHE->Fill( 5*JetInfo[iJetColl].Jet_Svx[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_SvxN[JetInfo[iJetColl].nJet] < -1 )           hData_Tagger_SSVHE->Fill( 5*JetInfo[iJetColl].Jet_SvxN[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_SvxHP[JetInfo[iJetColl].nJet]  >  1 )         hData_Tagger_SSVHP->Fill( 5*JetInfo[iJetColl].Jet_SvxHP[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_SvxNHP[JetInfo[iJetColl].nJet] < -1 )         hData_Tagger_SSVHP->Fill( 5*JetInfo[iJetColl].Jet_SvxNHP[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_CombSvx[JetInfo[iJetColl].nJet] > 0  )        hData_Tagger_CSV->Fill( 25*JetInfo[iJetColl].Jet_CombSvx[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_CombSvxN[JetInfo[iJetColl].nJet] > 0 )        hData_Tagger_CSV->Fill(-25*JetInfo[iJetColl].Jet_CombSvxN[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_SoftMu[JetInfo[iJetColl].nJet]  > 0        )  hData_Tagger_MU->Fill( 2.5*JetInfo[iJetColl].Jet_SoftMu[JetInfo[iJetColl].nJet] );
      if ( JetInfo[iJetColl].Jet_SoftMuN[JetInfo[iJetColl].nJet] < 0        )  hData_Tagger_MU->Fill( 2.5*JetInfo[iJetColl].Jet_SoftMuN[JetInfo[iJetColl].nJet] );

      // MC only
      if ( isData_ ) continue;

      hAllFlav_Flavour->Fill( flavour );
      if ( varneg > 0 ) hAllFlav_Tagger->Fill(-varneg );
      if ( varpos > 0 ) hAllFlav_Tagger->Fill( varpos );
      if ( flavour == 1 || flavour == 21 ) { // light q+gluon
        if ( varneg > 0 ) hLightFlav_Tagger->Fill(-varneg );
        if ( varpos > 0 ) hLightFlav_Tagger->Fill( varpos );
      } // light q+gluon
      if ( flavour == 21 ) { // gluon jets
        if ( varneg > 0 ) hGluonFlav_Tagger->Fill(-varneg );
        if ( varpos > 0 ) hGluonFlav_Tagger->Fill( varpos );
      } // gluon jets
      else if ( flavour == 1 ) { // uds jets
        if ( varneg > 0 ) hUDSFlav_Tagger->Fill(-varneg );
        if ( varpos > 0 ) hUDSFlav_Tagger->Fill( varpos );
      } // uds jets
      else if ( flavour == 4 ) { // c jets
        if ( varneg > 0 ) hCFlav_Tagger->Fill(-varneg );
        if ( varpos > 0 ) hCFlav_Tagger->Fill( varpos );
      } // c jets
      else if ( flavour == 5 ) { // b jets
        if ( varneg > 0 ) hBFlav_Tagger->Fill(-varneg );
        if ( varpos > 0 ) hBFlav_Tagger->Fill( varpos );
      } // b jets

      // Track history
      if ( useTrackHistory_ && indexes.size()!=0 && !isData_ ) {

        int cat1P = 0, cat2P = 0, cat3P = 0, catP = 0;
        int cat1N = 0, cat2N = 0, cat3N = 0, catN = 0;
        flags1P[TrackCategories::ConversionsProcess] ;
        if ( flags1P[TrackCategories::BWeakDecay] )  cat1P = 1;
        else if ( flags1P[TrackCategories::CWeakDecay] )  cat1P = 2;
        else if ( flags1P[TrackCategories::TauDecay] )    cat1P = 3;
        else if ( flags1P[TrackCategories::ConversionsProcess] )  cat1P = 4;
        else if ( flags1P[TrackCategories::KsDecay] )     cat1P = 5;
        else if ( flags1P[TrackCategories::LambdaDecay] ) cat1P = 6;
        else if ( flags1P[TrackCategories::HadronicProcess] ) cat1P = 7;
        else if ( flags1P[TrackCategories::Fake] )        cat1P = 8;
        else if ( flags1P[TrackCategories::SharedInnerHits] )         cat1P = 9;
        if(idSize > 1){
          if ( flags2P[TrackCategories::BWeakDecay] )  cat2P = 1;
          else if ( flags2P[TrackCategories::CWeakDecay] )  cat2P = 2;
          else if ( flags2P[TrackCategories::TauDecay] )    cat2P = 3;
          else if ( flags2P[TrackCategories::ConversionsProcess] )  cat2P = 4;
          else if ( flags2P[TrackCategories::KsDecay] )     cat2P = 5;
          else if ( flags2P[TrackCategories::LambdaDecay] ) cat2P = 6;
          else if ( flags2P[TrackCategories::HadronicProcess] ) cat2P = 7;
          else if ( flags2P[TrackCategories::Fake] )        cat2P = 8;
          else if ( flags2P[TrackCategories::SharedInnerHits] )         cat2P = 9;
        }
        if(idSize > 2){
          if ( flags3P[TrackCategories::BWeakDecay] )  cat3P = 1;
          else if ( flags3P[TrackCategories::CWeakDecay] )  cat3P = 2;
          else if ( flags3P[TrackCategories::TauDecay] )    cat3P = 3;
          else if ( flags3P[TrackCategories::ConversionsProcess] )  cat3P = 4;
          else if ( flags3P[TrackCategories::KsDecay] )     cat3P = 5;
          else if ( flags3P[TrackCategories::LambdaDecay] ) cat3P = 6;
          else if ( flags3P[TrackCategories::HadronicProcess] ) cat3P = 7;
          else if ( flags3P[TrackCategories::Fake] )        cat3P = 8;
          else if ( flags3P[TrackCategories::SharedInnerHits] )         cat3P = 9;
        }
        if ( flags1N[TrackCategories::BWeakDecay] )  cat1N = 1;
        else if ( flags1N[TrackCategories::CWeakDecay] )  cat1N = 2;
        else if ( flags1N[TrackCategories::TauDecay] )    cat1N = 3;
        else if ( flags1N[TrackCategories::ConversionsProcess] )  cat1N = 4;
        else if ( flags1N[TrackCategories::KsDecay] )     cat1N = 5;
        else if ( flags1N[TrackCategories::LambdaDecay] ) cat1N = 6;
        else if ( flags1N[TrackCategories::HadronicProcess] ) cat1N = 7;
        else if ( flags1N[TrackCategories::Fake] )        cat1N = 8;
        else if ( flags1N[TrackCategories::SharedInnerHits] )         cat1N = 9;
        if(idSize > 1){
          if ( flags2N[TrackCategories::BWeakDecay] )  cat2N = 1;
          else if ( flags2N[TrackCategories::CWeakDecay] )  cat2N = 2;
          else if ( flags2N[TrackCategories::TauDecay] )    cat2N = 3;
          else if ( flags2N[TrackCategories::ConversionsProcess] )  cat2N = 4;
          else if ( flags2N[TrackCategories::KsDecay] )     cat2N = 5;
          else if ( flags2N[TrackCategories::LambdaDecay] ) cat2N = 6;
          else if ( flags2N[TrackCategories::HadronicProcess] ) cat2N = 7;
          else if ( flags2N[TrackCategories::Fake] )        cat2N = 8;
          else if ( flags2N[TrackCategories::SharedInnerHits] )         cat2N = 9;
        }
        if(idSize > 2){
          if ( flags3N[TrackCategories::BWeakDecay] )  cat3N = 1;
          else if ( flags3N[TrackCategories::CWeakDecay] )  cat3N = 2;
          else if ( flags3N[TrackCategories::TauDecay] )    cat3N = 3;
          else if ( flags3N[TrackCategories::ConversionsProcess] )  cat3N = 4;
          else if ( flags3N[TrackCategories::KsDecay] )     cat3N = 5;
          else if ( flags3N[TrackCategories::LambdaDecay] ) cat3N = 6;
          else if ( flags3N[TrackCategories::HadronicProcess] ) cat3N = 7;
          else if ( flags3N[TrackCategories::Fake] )        cat3N = 8;
          else if ( flags3N[TrackCategories::SharedInnerHits] )         cat3N = 9;
        }

        if ( selTagger_ == 0 || selTagger_ == 3 ) {
          catP = cat3P;
          catN = cat3N;
        }
        else if ( selTagger_ == 2 ) {
          catP = cat2P;
          catN = cat2N;
        }
        else if ( selTagger_ == 1 ) {
          catP = cat1P;
          catN = cat1N;
        }


        if ( varneg > 0 ) {
          if      ( catN == 1 ) hAllFlav_Tagger_Bwd->Fill(-varneg );
          else if ( catN == 2 ) hAllFlav_Tagger_Cwd->Fill(-varneg );
          else if ( catN == 3 ) hAllFlav_Tagger_Tau->Fill(-varneg );
          else if ( catN == 4 ) hAllFlav_Tagger_Gam->Fill(-varneg );
          else if ( catN == 5 ) hAllFlav_Tagger_K0s->Fill(-varneg );
          else if ( catN == 6 ) hAllFlav_Tagger_Lam->Fill(-varneg );
          else if ( catN == 7 ) hAllFlav_Tagger_Int->Fill(-varneg );
          else if ( catN == 8 ) hAllFlav_Tagger_Fak->Fill(-varneg );
          else if ( catN == 9 ) hAllFlav_Tagger_Bad->Fill(-varneg );
          else                  hAllFlav_Tagger_Oth->Fill(-varneg );
        }
        if ( varpos > 0 ) {
          if      ( catP == 1 ) hAllFlav_Tagger_Bwd->Fill( varpos );
          else if ( catP == 2 ) hAllFlav_Tagger_Cwd->Fill( varpos );
          else if ( catP == 3 ) hAllFlav_Tagger_Tau->Fill( varpos );
          else if ( catP == 4 ) hAllFlav_Tagger_Gam->Fill( varpos );
          else if ( catP == 5 ) hAllFlav_Tagger_K0s->Fill( varpos );
          else if ( catP == 6 ) hAllFlav_Tagger_Lam->Fill( varpos );
          else if ( catP == 7 ) hAllFlav_Tagger_Int->Fill( varpos );
          else if ( catP == 8 ) hAllFlav_Tagger_Fak->Fill( varpos );
          else if ( catP == 9 ) hAllFlav_Tagger_Bad->Fill( varpos );
          else                  hAllFlav_Tagger_Oth->Fill( varpos );
        }
      }

    }

    ++JetInfo[iJetColl].nJet;

  } // end loop on jet

  hData_All_NJets->Fill( JetInfo[iJetColl].nJet );
  hData_NJets->Fill( numjet );

  return ;
}

void BTagAnalyzer::setTracksPV( const reco::Vertex *pv, bool isPV )
{
  for (reco::Vertex::trackRef_iterator itt = (*pv).tracks_begin(); itt != (*pv).tracks_end(); ++itt) {
    for (int i=0; i<nTrack; ++i) {
      if ( fabs( (&**itt)->pt() - Track_pt[i] )  < 1.e-5 ) {
        if ( isPV ) {
          Track_PV[i] = nPV + 1;
          Track_PVweight[i] = (*pv).trackWeight(*itt);
        }
        else {
          Track_isfromSV[i]= 1;
          Track_SV[i] = nSV + 1;
          Track_SVweight[i] = (*pv).trackWeight(*itt);
          //   std::cout << " track " << i << " at SV " << Track_SV[i] << std::endl;
        }
      }
    }
  }
}


// ------------ method called once each job just before starting event loop  ------------
void BTagAnalyzer::beginJob() {
  //cat 0
  cat0.etaMax = 2.5;
  cat0.etaMin = 0;
  cat0.nHitsMax= 50;
  cat0.nHitsMin= 8;
  cat0.nPixelHitsMax = 1;
  cat0.nPixelHitsMin = 1;
  cat0.pMax= 5000;
  cat0.pMin= 0;
  cat0.chiMin= 0;
  cat0.chiMax= 5;
  cat0.withFirstPixel = 0;

  //cat 1
  cat1.etaMax        = 2.5;
  cat1.etaMin        = 0;
  cat1.nHitsMax      = 50;
  cat1.nHitsMin      = 8;
  cat1.nPixelHitsMax = 8;
  cat1.nPixelHitsMin = 2;
  cat1.pMax          = 5000;
  cat1.pMin          = 0;
  cat1.chiMin        = 2.5;
  cat1.chiMax        = 5;
  cat1.withFirstPixel = 0;

  //cat 2
  cat2.etaMax        = 0.8;
  cat2.etaMin        = 0;
  cat2.nHitsMax      = 50;
  cat2.nHitsMin      = 8;
  cat2.nPixelHitsMax = 8;
  cat2.nPixelHitsMin = 3;
  cat2.pMax          = 8;
  cat2.pMin          = 0;
  cat2.chiMin        = 0;
  cat2.chiMax        = 2.5;
  cat2.withFirstPixel = 0;

  //cat 3
  cat3.etaMax        = 1.6;
  cat3.etaMin        = 0.8;
  cat3.nHitsMax      = 50;
  cat3.nHitsMin      = 8;
  cat3.nPixelHitsMax = 8;
  cat3.nPixelHitsMin = 3;
  cat3.pMax          = 8;
  cat3.pMin          = 0;
  cat3.chiMin        = 0;
  cat3.chiMax        = 2.5;
  cat3.withFirstPixel = 0;

  //cat 4
  cat4.etaMax        = 2.5;
  cat4.etaMin        = 1.6;
  cat4.nHitsMax      = 50;
  cat4.nHitsMin      = 8;
  cat4.nPixelHitsMax = 8;
  cat4.nPixelHitsMin = 3;
  cat4.pMax          = 8;
  cat4.pMin          = 0;
  cat4.chiMin        = 0;
  cat4.chiMax        = 2.5;
  cat4.withFirstPixel = 0;

  //cat 5
  cat5.etaMax        = 2.5;
  cat5.etaMin        = 0;
  cat5.nHitsMax      = 50;
  cat5.nHitsMin      = 8;
  cat5.nPixelHitsMax = 8;
  cat5.nPixelHitsMin = 2;
  cat5.pMax          = 8;
  cat5.pMin          = 0;
  cat5.chiMin        = 0;
  cat5.chiMax        = 2.5;
  cat5.withFirstPixel = 0;

  //cat 6
  cat6.etaMax        = 0.8;
  cat6.etaMin        = 0;
  cat6.nHitsMax      = 50;
  cat6.nHitsMin      = 8;
  cat6.nPixelHitsMax = 8;
  cat6.nPixelHitsMin = 3;
  cat6.pMax          = 5000;
  cat6.pMin          = 8;
  cat6.chiMin        = 0;
  cat6.chiMax        = 2.5;
  cat6.withFirstPixel = 0;

  //cat 7
  cat7.etaMax        = 1.6;
  cat7.etaMin        = 0.8;
  cat7.nHitsMax      = 50;
  cat7.nHitsMin      = 8;
  cat7.nPixelHitsMax = 8;
  cat7.nPixelHitsMin = 3;
  cat7.pMax          = 5000;
  cat7.pMin          = 8;
  cat7.chiMin        = 0;
  cat7.chiMax        = 2.5;
  cat7.withFirstPixel = 0;

  //cat 8
  cat8.etaMax        = 2.5;
  cat8.etaMin        = 1.6;
  cat8.nHitsMax      = 50;
  cat8.nHitsMin      = 8;
  cat8.nPixelHitsMax = 8;
  cat8.nPixelHitsMin = 3;
  cat8.pMax          = 5000;
  cat8.pMin          = 8;
  cat8.chiMin        = 0;
  cat8.chiMax        = 2.5;
  cat8.withFirstPixel = 0;

  //cat 9
  cat9.etaMax        = 2.5;
  cat9.etaMin        = 0;
  cat9.nHitsMax      = 50;
  cat9.nHitsMin      = 8;
  cat9.nPixelHitsMax = 2;
  cat9.nPixelHitsMin = 2;
  cat9.pMax          = 5000;
  cat9.pMin          = 8;
  cat9.chiMin        = 0;
  cat9.chiMax        = 2.5;
  cat9.withFirstPixel = 0;
}


// ------------ method called once each job just after ending the event loop  ------------
void BTagAnalyzer::endJob() {
}


std::vector< float > BTagAnalyzer::getTrackProbabilies(std::vector< float > v, int ipType){

  std::vector< float > vectTrackProba;

  for (std::vector<float>::const_iterator q = v.begin(); q != v.end(); ++q) {
    //positives and negatives tracks
    double p3d = 0;
    if(ipType == 0){
      if ( *q>=0){p3d= (*q)/2.;}else{p3d=1.+(*q)/2.;}
      //if(-log(p3d)> 5) p3d=exp(-5.0);
      vectTrackProba.push_back(p3d);
    }

    //positives tracks only
    if(ipType == 1 && *q >=0 ){
      vectTrackProba.push_back(*q);
    }

    //negatives tracks only
    if(ipType == 2 && *q <0){
      vectTrackProba.push_back(-(*q));
    }
  }

  return vectTrackProba;
}


double BTagAnalyzer::calculProbability(std::vector< float > v) {

  int ngoodtracks=v.size();
  double SumJet=0.;
  double m_minTrackProb = 0.005;
  for (std::vector<float>::const_iterator q = v.begin(); q != v.end(); ++q) {
    SumJet += (*q>m_minTrackProb)?log(*q):log(m_minTrackProb);
  }

  double ProbJet;
  double Loginvlog = 0;

  if ( SumJet < 0. ) {
    if ( ngoodtracks >= 2 ) {
      Loginvlog = log(-SumJet);
    }
    double Prob = 1.;
    double lfact = 1.;
    for (int l=1; l!=ngoodtracks; ++l) {
      lfact *= l;
      Prob += exp( l*Loginvlog-log(1.*lfact) );
    }
    double LogProb = log(Prob);
    ProbJet = std::min(exp( std::max(LogProb+SumJet,-30.) ), 1.);
  } else {
    ProbJet = 1.;
  }
  return ProbJet;
}


bool BTagAnalyzer::findCat(const reco::Track* track, CategoryFinder& d) {
  //int numcat=-1;

  double   p = track->p();
  double eta = track->eta();
  double chi = track->normalizedChi2();
  int   nhit = track->numberOfValidHits();
  int   npix = track->hitPattern().numberOfValidPixelHits();

  bool result = ( p > d.pMin	          &&  p  < d.pMax	      &&
      fabs(eta) > d.etaMin	  &&  fabs(eta) < d.etaMax    &&
      nhit >= d.nHitsMin	  &&  nhit <= d.nHitsMax      &&
      npix >= d.nPixelHitsMin &&  npix <= d.nPixelHitsMax &&
      chi >= d.chiMin         &&  chi <= d.chiMax );

  return result;
}


int BTagAnalyzer::matchMuon(const edm::RefToBase<reco::Track>& theMuon, const edm::View<reco::Muon>& muons ){
  double small = 1.e-3;
  int matchedMu = -1;
  for(unsigned int i=0; i<muons.size(); ++i){
    double muonpt = -10000;
    if( muons[i].isGlobalMuon() )                               muonpt = muons[i].globalTrack()->pt() ;
    if(!muons[i].isGlobalMuon() &&  muons[i].isTrackerMuon())   muonpt = muons[i].innerTrack()->pt() ;

    if ( fabs(theMuon->pt() - muonpt )  < small  ){ matchedMu = i; }

  }
  return matchedMu;
}


std::vector<BTagAnalyzer::simPrimaryVertex> BTagAnalyzer::getSimPVs(const edm::Handle<edm::HepMCProduct>& evtMC){

  std::vector<BTagAnalyzer::simPrimaryVertex> simpv;
  const HepMC::GenEvent* evt=evtMC->GetEvent();
  if (evt) {

    for ( HepMC::GenEvent::vertex_const_iterator vitr= evt->vertices_begin();
        vitr != evt->vertices_end(); ++vitr ) { // loop for vertex ...
      HepMC::FourVector pos = (*vitr)->position();
      //HepLorentzVector pos = (*vitr)->position();

      bool hasMotherVertex=false;

      for ( HepMC::GenVertex::particle_iterator
          mother  = (*vitr)->particles_begin(HepMC::parents);
          mother != (*vitr)->particles_end(HepMC::parents);
          ++mother ) {
        HepMC::GenVertex * mv=(*mother)->production_vertex();
        if (mv) {
          hasMotherVertex=true;
          break; //if verbose_, print all particles of gen vertices
        }
      }

      if (hasMotherVertex) {continue;}

      // could be a new vertex, check  all primaries found so far to avoid multiple entries
      const double mm=0.1;
      simPrimaryVertex sv(pos.x()*mm,pos.y()*mm,pos.z()*mm);
      simPrimaryVertex *vp=NULL;  // will become non-NULL if a vertex is found and then point to it
      for (std::vector<simPrimaryVertex>::iterator v0=simpv.begin();
          v0!=simpv.end(); ++v0) {
        if ( (fabs(sv.x-v0->x)<1e-5) && (fabs(sv.y-v0->y)<1e-5)
            && (fabs(sv.z-v0->z)<1e-5) ) {
          vp=&(*v0);
          break;
        }
      }

      if (!vp) {	  // this is a new vertex
        simpv.push_back(sv);
        vp=&simpv.back();
      }
      vp->genVertex.push_back((*vitr)->barcode());
    }
  }
  return simpv;
}


int BTagAnalyzer::getMuonTk(double pt) {
  int idxTk = -1;
  for (int itk = 0; itk < nTrack ; ++itk) {
    if ( fabs(pt-Track_pt[itk]) < 1e-5 ) idxTk = itk;
  }
  return idxTk;
}


int BTagAnalyzer::isFromGSP(const reco::Candidate* c)
{
  int isFromGSP = 0;

  if( c->numberOfMothers() == 1 ) {
    const reco::Candidate* dau = c;
    const reco::Candidate* mom = c->mother();
    while( dau->numberOfMothers() == 1 && !( mom->status()==3 && (abs(mom->pdgId())==4 || abs(mom->pdgId())==5) ) ) {
      if( abs(mom->pdgId())==21 )
      {
        isFromGSP = 1;
        break;
      }
      dau = mom;
      mom = dau->mother();
    }
  }

  return isFromGSP;
}


// -------------------------------------------------------------------------
// NameCompatible
// -------------------------------------------------------------------------
bool BTagAnalyzer::NameCompatible(const std::string& pattern, const std::string& name)
{
  const boost::regex regexp(edm::glob2reg(pattern));

  return boost::regex_match(name,regexp);
}


//define this as a plug-in
DEFINE_FWK_MODULE(BTagAnalyzer);

