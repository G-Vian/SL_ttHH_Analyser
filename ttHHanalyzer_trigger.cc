#include "tnm.h"
#include <cmath> 
#include <algorithm>
#include <vector>
#include <map>
#include "TVector3.h"
#include "ttHHanalyzer_trigger.h"
#include <iostream>

using namespace std;

#include "Logger.h"
using namespace Logger;
 
void ttHHanalyzer::performAnalysis(){
    loop(noSys, false);
    /*    getbJetEffMap();
    initHistograms(kJES, false);
    initTree(kJES, false);
    loop(kJES, false);
    initHistograms(kJES, true);
    initTree(kJES, true);
    loop(kJES, true);

    initHistograms(kJER, false);
    initTree(kJER, false);
    loop(kJER, false);
    initHistograms(kJER, true);
    initTree(kJER, true);
    loop(kJER, true);

    initHistograms(kbTag, false);
    initTree(kbTag, false);
    loop(kbTag, false);
    initHistograms(kbTag, true);
    initTree(kbTag, true);
    loop(kbTag, true); */

}




void ttHHanalyzer::loop(sysName sysType, bool up){

    int nevents = _ev->size();
    ////int nevents = 1000;

    cout<<endl;
    print("This analyzer commented out [ \"WTF\" log ] in the header, Please check if you want!!!", "magenta", "warning");

    cout<<endl;
    print("--------------------------------------------------------------------------", "b");
    print("Before start, Let's check the analysis information", "b");
    print("Run Year    ----> [  " + _runYear + "  ]", "b");
    print("Data or MC  ----> [  " + _DataOrMC + "  ]", "b");
    print("Sample Name ----> [  " + _sampleName + "  ]", "b");
    
    string checklist = "[ tnm.cc ] & [ analyzer header ] & [ main ] & [ analyzer constructor ]";
    bool exitFlag = false;
    if(_runYear == "nothing"){
        print("RunYear is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_DataOrMC == "nothing"){
        print("Whether Data or MC is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    if(_sampleName == "nothing"){
        print("SampleName is not defined, Please check the" + checklist, "r", "error");
        exitFlag = true;
    }
    print("--------------------------------------------------------------------------", "b");
    cout<<endl;
    if(exitFlag) std::exit(EXIT_FAILURE);

    std::string analysisInfo = _runYear + ", " + _DataOrMC + ", " + _sampleName;

    for(int entry=0; entry < nevents; entry++){
	event * currentEvent = new event;
        ////cout << "Processed events: " << entry << endl;
	_ev->read(entry);       // read an event into event buffer
	process(currentEvent, sysType, up);

	if (entry % 1000 == 0){
            print("Processed events of " + analysisInfo + ": " + to_string(entry) ,"c");
            currentEvent->summarize();
        }

	events.push_back(currentEvent);
    }
    //    events.back()->summarize();

    
    writeHistos();
    writeTree();
    
    

    for (const auto& x : cutflow){
	std::cout << x.first  // string (key)
		  << ':' 
		  << x.second // string's value 
		  << std::endl;
    } 
    hCutFlow->Write();
    hCutFlow_w->Write();
}

void ttHHanalyzer::createObjects(event * thisEvent, sysName sysType, bool up){
    // Function to create and initialize event objects for analysis.
    // Parameters:
    // - thisEvent: Pointer to the current event object containing event data.
    // - sysType: Type of systematic uncertainty being handled (e.g., JES, JER, etc.).
    // - up: Boolean flag indicating whether the systematic variation is "up" (true) or "down" (false).	
    _ev->fillObjects();
    // Calls the fillObjects() method on the _ev member, which populates collections of objects 
    // (e.g., jets, leptons, MET) from the raw event data. This prepares all necessary objects 
    // for subsequent analysis steps. [g. vian]
 
    thisEvent->setMuonTrigger(
        _ev->HLT_IsoMu27
    );
///Gamze's 
	
    if(_year == "2017"){
	if(_DataOrMC == "Data"){
	    thisEvent->setFilter(_ev->Flag_goodVertices ||
				 _ev->Flag_globalSuperTightHalo2016Filter ||
				 _ev->Flag_HBHENoiseFilter ||
				 _ev->Flag_HBHENoiseIsoFilter ||
				 _ev->Flag_EcalDeadCellTriggerPrimitiveFilter ||
				 _ev->Flag_BadPFMuonFilter ||
				 _ev->Flag_eeBadScFilter ||
				 _ev->Flag_ecalBadCalibFilter);
	    if(_sampleName == "ee"){
		thisEvent->setTrigger((_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
				       _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ) // ||
				      // _ev->HLT_Ele27_WPTight_Gsf)
				      && !(_ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ ||
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8));// ||
					   //_ev->HLT_IsoMu24_eta2p1 ||
					   //_ev->HLT_IsoMu27));
	    } else if(_sampleName == "emu"){ 
		thisEvent->setTrigger((_ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL ||
				       _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				       _ev->HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				       _ev->HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ) 
				      && !(_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
					   _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ  ||
					   //  _ev->HLT_Ele27_WPTight_Gsf || 
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ ||
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8)); //||
					   //_ev->HLT_IsoMu24_eta2p1 || 
					   //_ev->HLT_IsoMu27));
	    } else if(_sampleName == "mumu"){
		thisEvent->setTrigger(_ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ ||
                                      _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8); // ||
		//_ev->HLT_IsoMu24_eta2p1 ||
		//_ev->HLT_IsoMu27);
	    }
	} else if(_DataOrMC == "MC"){ 
	    thisEvent->setFilter(_ev->Flag_goodVertices ||
				 _ev->Flag_globalSuperTightHalo2016Filter ||
				 _ev->Flag_HBHENoiseFilter ||
				 _ev->Flag_HBHENoiseIsoFilter ||
				 _ev->Flag_EcalDeadCellTriggerPrimitiveFilter ||
				 _ev->Flag_BadPFMuonFilter ||
				 _ev->Flag_eeBadScFilter ||
				 _ev->Flag_ecalBadCalibFilter);
	    thisEvent->setTrigger(_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
				  _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Ele27_WPTight_Gsf ||
				  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL ||
				  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Ele32_WPTight_Gsf ||
				  _ev->HLT_IsoMu24_eta2p1 ||
				  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ ||
				  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 ||
				  _ev->HLT_IsoMu24_eta2p1 ||
				  _ev->HLT_IsoMu27);
	}
    }  else if(_year == "2018"){
	if(_DataOrMC == "Data"){
	    thisEvent->setFilter(_ev->Flag_goodVertices ||
				 _ev->Flag_globalSuperTightHalo2016Filter ||
				 _ev->Flag_HBHENoiseFilter ||
				 _ev->Flag_HBHENoiseIsoFilter ||
				 _ev->Flag_EcalDeadCellTriggerPrimitiveFilter ||
				 _ev->Flag_BadPFMuonFilter ||
				 _ev->Flag_eeBadScFilter ||
				 _ev->Flag_eeBadScFilter ||
				 _ev->Flag_ecalBadCalibFilter);
	    if(_sampleName == "ee"){
		thisEvent->setTrigger((_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
				       _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				       _ev->HLT_Ele32_WPTight_Gsf)
				      && !(_ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 ||
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8  ||
					   _ev->HLT_IsoMu24));
	    } else if(_sampleName == "emu"){ 
		thisEvent->setTrigger((_ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL ||
				       _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				       _ev->HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				       _ev->HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ) 
				      && !(_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
					   _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ  ||
					   _ev->HLT_Ele32_WPTight_Gsf || 
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 ||
					   _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 ||
					   _ev->HLT_IsoMu24)); 
					  
	    } else if(_sampleName == "mumu"){
		thisEvent->setTrigger(_ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 ||
                                      _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 ||
				      _ev->HLT_IsoMu24);
	    }
	} else if(_DataOrMC == "MC"){
	    thisEvent->setFilter(_ev->Flag_goodVertices ||
				 _ev->Flag_globalSuperTightHalo2016Filter ||
				 _ev->Flag_HBHENoiseFilter ||
				 _ev->Flag_HBHENoiseIsoFilter ||
				 _ev->Flag_EcalDeadCellTriggerPrimitiveFilter ||
				 _ev->Flag_BadPFMuonFilter ||
				 _ev->Flag_eeBadScFilter ||
				 _ev->Flag_ecalBadCalibFilter);
	    thisEvent->setTrigger(_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
				  _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL ||
				  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
				  _ev->HLT_Ele32_WPTight_Gsf ||
				  _ev->HLT_IsoMu24 ||
				  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass8 ||
				  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8);
	}
    }






////	
if (_DataOrMC == "Data") {
    if (_sampleName == "JetHT_B") {
        thisEvent->setHadTrigger(
            (_ev->HLT_PFHT1050 ||
             _ev->HLT_PFHT430_SixJet40_BTagCSV_p080 ||
             _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075) &&
            !(_ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07 ||
              _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0)
        );
    } else if (_sampleName == "JetHT_C" || _sampleName == "JetHT_D" || 
               _sampleName == "JetHT_E" || _sampleName == "JetHT_F") {
        thisEvent->setHadTrigger(
            (_ev->HLT_PFHT1050 ||
             _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5 ||
             _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2) &&
            !(_ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07 ||
              _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0)
        );
    } else if (_sampleName == "BTagCSV_B") {
        thisEvent->setHadTrigger(
            _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07
        );
    } else if (_sampleName == "BTagCSV_C" || _sampleName == "BTagCSV_D" || 
               _sampleName == "BTagCSV_E" || _sampleName == "BTagCSV_F") {
        thisEvent->setHadTrigger(
            _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0
        );
    } else if (_sampleName == "SingleMuon_B" || _sampleName == "SingleMuon_C" || 
               _sampleName == "SingleMuon_D" || _sampleName == "SingleMuon_E" || 
               _sampleName == "SingleMuon_F") {
        thisEvent->setHadTrigger(
            (_ev->HLT_PFHT1050 ||
             _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5 ||
             _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2) &&
            !(_ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07 ||
              _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0)
        );
        print("This is the Single Muon Data Set", "b");
    } else {
        print("This is weird, I assume there is/are more than 1 Data sample without JetHT & BTagCSV", "r", "error");
        std::exit(EXIT_FAILURE);
    }
} else if (_DataOrMC == "MC") {
    thisEvent->setHadTrigger(
        _ev->HLT_PFHT1050 ||
        _ev->HLT_PFHT430_SixJet40_BTagCSV_p080 ||
        _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5 ||
        _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075 ||
        _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2 ||
        _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07 ||
        _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0
    );
} else {
    print("This is weird, You need to set the data type as Data or MC. Please check the arguments", "r", "error");
    std::exit(EXIT_FAILURE);
}

    
    thisEvent->setTrigger(_ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL ||
			  _ev->HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
			  _ev->HLT_Ele27_WPTight_Gsf ||
			  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL ||
			  _ev->HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ ||
			  _ev->HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
			  _ev->HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ ||
			  _ev->HLT_Ele32_WPTight_Gsf ||
			  _ev->HLT_IsoMu24_eta2p1 ||
			  _ev->HLT_IsoMu27 ||
			  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ ||
			  _ev->HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_Mass3p8 ||
			  _ev->HLT_IsoMu24_eta2p1 ||
			  _ev->HLT_IsoMu27);
    thisEvent->setFilter(_ev->Flag_goodVertices ||
    			 _ev->Flag_globalSuperTightHalo2016Filter ||
    			 _ev->Flag_HBHENoiseFilter ||
    			 _ev->Flag_HBHENoiseIsoFilter ||
    			 _ev->Flag_EcalDeadCellTriggerPrimitiveFilter ||
    			 _ev->Flag_BadPFMuonFilter);
   
	
    thisEvent->setPV(_ev->PV_npvsGood); // Sets the primary vertex (PV) of the event.
    std::vector<eventBuffer::GenPart_s> genPart = _ev->GenPart; // Creates a vector 'genPart' to hold all generator-level particles (GenPart), getting the data from '_ev'.
    std::vector<eventBuffer::Jet_s> jet = _ev->Jet; 
    std::vector<eventBuffer::Muon_s> muonT = _ev->Muon;
    std::vector<eventBuffer::Electron_s> ele = _ev->Electron;
    std::vector<eventBuffer::FatJet_s> boostedJet = _ev->FatJet;
    objectGenPart * currentGenPart; // Declares a pointer to an 'objectGenPart'.
    objectBoostedJet * currentBoostedJet;
    objectJet * currentJet;
    objectLep * currentMuon;
    objectLep * currentEle;
    int nVetoMuons = 0, nVetoEle = 0;// // Initializes counters for "veto" muons and electrons to 0.
    objectMET * MET = new objectMET(_ev->PuppiMET_pt, 0, _ev->PuppiMET_phi, 0);
    float e = 1., es  = 1., pe = 1., pes = 1.; // Declares and initializes several float variables 
    float me = 1., mes = 1., pme = 1.,  pmes = 1.;   
    thisEvent->setMET(MET);


    for(int i=0; i < boostedJet.size(); i++){
       	currentBoostedJet = new objectBoostedJet(boostedJet[i].pt, boostedJet[i].eta, boostedJet[i].phi, boostedJet[i].mass);
	currentBoostedJet->softDropMass = boostedJet[i].msoftdrop;
	
	if(currentBoostedJet->getp4()->Pt() > cut["boostedJetPt"] && fabs(currentBoostedJet->getp4()->Eta()) < fabs(cut["boostedJetEta"])){
	    //	    if((boostedJet[i].jetId & 4) == true){  	     
	    thisEvent->selectBoostedJet(currentBoostedJet);	
	    if(currentBoostedJet->getp4()->Pt() > cut["hadHiggsPt"]){
		if(boostedJet[i].particleNet_HbbvsQCD > cut["bTagDisc"]){
		    thisEvent->selectHadronicHiggs(currentBoostedJet);
		}
	      }
	    }
	}
    
// verify if there is a leading lepton candidate --> if yes, accept and store all leptons with PT > subleading PT (this will include also the leading candidate)
// order all the leptons. This will provide a list of leptons ordered by PT that obey PT >  subleading PT

    bool thereIsALeadLepton = false;
	
    for(int i = 0; i < muonT.size(); i++){
	if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all  < cut["muonIso"]){
	//if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].mvaTTH > 0.15 && muonT[i].pfRelIso04_all  < cut["muonIso"]){
	    if(muonT[i].pt > cut["leadMuonPt"]){
		thereIsALeadLepton = true;
		break;
	    }
	}
    }
    if(!thereIsALeadLepton){
	for(int i = 0; i < ele.size(); i++){
	    if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
		if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
		    if(ele[i].pt > cut["leadElePt"]){
			thereIsALeadLepton = true;
			break;
		    }
		}
	    }
	}
    }
     if(thereIsALeadLepton){ //we can add all leptons passing to the sublead selection to our containers
         for(int i = 0; i < muonT.size(); i++){
             if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].tightId == true && muonT[i].pfRelIso04_all < cut["muonIso"]){
             //	    if(fabs(muonT[i].eta) < cut["muonEta"] && muonT[i].mvaTTH > 0.15 && muonT[i].pfRelIso04_all  < cut["muonIso"]){	
         	if(muonT[i].pt > cut["subLeadMuonPt"]){
         	    currentMuon = new objectLep(muonT[i].pt, muonT[i].eta, muonT[i].phi, 0.);
         	    currentMuon->charge = muonT[i].charge;
         	    currentMuon->miniPFRelIso = muonT[i].miniPFRelIso_all;
         	    currentMuon->pfRelIso04 = muonT[i].pfRelIso04_all;
         	    thisEvent->selectMuon(currentMuon);
         	}
             }
         }
         for(int i = 0; i < ele.size(); i++){
             if(fabs(ele[i].deltaEtaSC + ele[i].eta) < 1.4442 || fabs(ele[i].deltaEtaSC + ele[i].eta) > 1.5660){  //Electrons tracked neither in the barrel nor in the endcap are discarded.
         	      if(fabs(ele[i].eta) < cut["eleEta"] && ele[i].mvaFall17V2Iso_WP90 == true && ele[i].pfRelIso03_all  < cut["eleIso"]){ 
                  if(ele[i].pt > cut["subLeadElePt"]){
         		currentEle = new objectLep(ele[i].pt, ele[i].eta, ele[i].phi, 0.);	 
         		currentEle->charge = ele[i].charge;
         		currentEle->miniPFRelIso = ele[i].miniPFRelIso_all;
         		currentEle->pfRelIso03 = ele[i].pfRelIso03_all;
         		thisEvent->selectEle(currentEle);
         	      }
               }
         	}
         }
     }
	
    thisEvent->orderLeptons(); 
// store the leptons with PT>15, then we select events with Nleptons=1 and PT>cut [g. vian]



	
	
    float dR = 0., deltaEta = 0., deltaPhi = 0.;
    for(int i=0; i < jet.size(); i++){
       	currentJet = new objectJet(jet[i].pt, jet[i].eta, jet[i].phi, jet[i].mass);
	currentJet->bTagCSV = jet[i].btagDeepFlavB;
	currentJet->jetID = jet[i].jetId;
	currentJet->jetPUid = jet[i].puId;
	if(_sys && sysType == kJES){
	    if(jet[i].btagDeepFlavB > objectJet::valbTagMedium){  	       
		currentJet->scale(getSysJES(_hbJES, currentJet->getp4()->Pt()), up);
	    } else {
		currentJet->scale(getSysJES(_hJES, currentJet->getp4()->Pt()), up);
	    }
	    if(up) thisEvent->getMET()->subtractp4(currentJet->getOffset());
	    else thisEvent->getMET()->addp4(currentJet->getOffset());
	}else if(_sys && sysType == kJER){
	    if(up) currentJet->scale(getSysJER(0.03));
	    else currentJet->scale(getSysJER(0.001));
	    thisEvent->getMET()->subtractp4(currentJet->getOffset());
	}
	if(currentJet->getp4()->Pt() > cut["jetPt"] && fabs(currentJet->getp4()->Eta()) < abs(cut["jetEta"]) && currentJet->jetID >= cut["jetID"]){
	    ////if((currentJet->getp4()->Pt() < cut["maxPt_PU"] && currentJet->jetPUid >= cut["jetPUid"]) || (currentJet->getp4()->Pt() >= cut["maxPt_PU"])){
		if(jet[i].btagDeepFlavB <= objectJet::valbTagLoose){  	     
		    thisEvent->selectLightJet(currentJet);
		} else if(jet[i].btagDeepFlavB > objectJet::valbTagMedium){  	     
		    thisEvent->selectbJet(currentJet);
		    if(!_sys || sysType == noSys) _hbJetEff->Fill(currentJet->getp4()->Pt());
		    if(_sys && sysType==kbTag){
			e = _hbJetEff->GetBinContent(_hbJetEff->FindBin(currentJet->getp4()->Pt()));
			if(e < cEps) e = cEps;
			pe *= e; 
			if(up)
			    pes *= (1.+_hSysbTagM->GetBinContent(_hSysbTagM->FindBin(currentJet->getp4()->Pt())))*e;
			else 
			    pes *= (1.-_hSysbTagM->GetBinContent(_hSysbTagM->FindBin(currentJet->getp4()->Pt())))*e;
		    }
		} else {
		    if(_sys && sysType==kbTag){
			me = _hbJetEff->GetBinContent(_hbJetEff->FindBin(currentJet->getp4()->Pt()));
			if(me < cEps) me = cEps;
			else if(me == 1) me = 1 - cEps;
			pme *= 1 - me;
			if(up)
			    pmes *= (1. - me * (1.+_hSysbTagM->GetBinContent(_hSysbTagM->FindBin(currentJet->getp4()->Pt()))));
			else
			    pmes *= (1. - me * (1.-_hSysbTagM->GetBinContent(_hSysbTagM->FindBin(currentJet->getp4()->Pt()))));
		    }
		}
		thisEvent->selectJet(currentJet);
		if(!_sys || sysType == noSys) _hJetEff->Fill(currentJet->getp4()->Pt());
		if(jet[i].btagDeepFlavB > objectJet::valbTagLoose){       	   
		    thisEvent->selectLoosebJet(currentJet);
		}
	}	    
    }
    thisEvent->orderJets();

    if(_sys && sysType==kbTag) thisEvent->setbTagSys( pes*pmes/(pe*pme));
    else thisEvent->setbTagSys(1.);
    
    //    thisEvent->setnVetoLepton( nVetoMuons + nVetoEle);

	
    // Selecting b jets from genParticle info
    for (int i = 0; i < genPart.size(); i++){
       	currentGenPart = new objectGenPart(genPart[i].pt, genPart[i].eta, genPart[i].phi, genPart[i].mass);
	currentGenPart->hasHiggsMother = false;
	currentGenPart->hasTopMother = false;

      	if (bool((abs(genPart[i].pdgId) == 5) && (genPart[i].statusFlags & 256)) == true){
	    thisEvent->selectGenPart(currentGenPart);
	    int motherInd = genPart[i].genPartIdxMother;
	    if(abs(genPart[motherInd].pdgId) == 25 && (genPart[motherInd].statusFlags & 256)) {
		currentGenPart->hasHiggsMother = true;
	    } else if(abs(genPart[motherInd].pdgId) == 6 && (genPart[motherInd].statusFlags & 256)) {
		currentGenPart->hasTopMother = true;
	    } else {
		while((abs(genPart[motherInd].pdgId) != 25 || abs(genPart[motherInd].pdgId) != 6 || !(genPart[motherInd].statusFlags & 256)) && motherInd > 1){
		    motherInd = genPart[motherInd].genPartIdxMother;
		    if(abs(genPart[motherInd].pdgId) == 25 && (genPart[motherInd].statusFlags & 256)) {
			currentGenPart->hasHiggsMother = true;
		    } else if(abs(genPart[motherInd].pdgId) == 6 && (genPart[motherInd].statusFlags & 256)) {
			currentGenPart->hasTopMother = true;
		    }
		}
	    }
	    thisEvent->selectGenPart(currentGenPart);
	}
    }
}



bool ttHHanalyzer::selectObjects(event *thisEvent){


    cutflow["noCut"]+=1;
    hCutFlow->Fill("noCut",1);
    hCutFlow_w->Fill("noCut",_weight);

    ////if(cut["trigger"] > 0 && thisEvent->getTriggerAccept() == false){
    ////    return false;
    ////}

    if(cut["trigger"] > 0 && thisEvent->getMuonTriggerAccept() == false)
    {
        return false;
    }
    cutflow["MuonTrigger"]+=1;                 
    hCutFlow->Fill("MuonTrigger",1);
    hCutFlow_w->Fill("MuonTrigger",_weight);

    ////if(cut["filter"] > 0 && thisEvent->getMETFilter() == false){
    ////    return false;
    ////}
    
    ////if(cut["pv"] < 0 && thisEvent->getPVvalue() == false){
    ////    return false;
    ////}
	
    if(!(thisEvent->getnSelJet() >= cut["nJets"] )){
	return false;
    }
    cutflow["njets>=4"]+=1;                 
    hCutFlow->Fill("njets>=4",1);
    hCutFlow_w->Fill("njets>=4",_weight);


    if(!(thisEvent->getnbJet() >= cut["nbJets"])){
	    return false;
    }
    cutflow["nbjets>=3"]+=1;                 
    hCutFlow->Fill("nbjets>=3",1);
    hCutFlow_w->Fill("nbjets>=3",_weight);
    

	
   ////This part is problematic for the SL channel; we need to ensure that there are indeed six jets in the event before the program accesses the sixth one.
    ///if(!(thisEvent->getSelJets()->at(5)->getp4()->Pt() > cut["6thJetsPT"])){
    ///        return false;
    ///}

// Ensure there are at least 6 jets before accessing index 5
//  (actually if we do that we will be rejecting events with less than 6 jets)	
//    if (thisEvent->getSelJets()->size() > 5) {
//        if (!(thisEvent->getSelJets()->at(5)->getp4()->Pt() > cut["6thJetsPT"])) {
//            return false; // Does not pass the cut
//        }
//    } else {
//        return false; // Not enough jets to apply the cut
//    }

//    cutflow["6thJetsPT>20"]+=1;
//    hCutFlow->Fill("6thJetsPT>20",1);
//    hCutFlow_w->Fill("6thJetsPT>20",_weight);

// Check if the number of selected leptons is equal to the expected number (cut["nLeptons"])
// If not, return false, meaning the event doesn't pass this selection.
// the selected leptons are with PT>(subleading_PT), so one must also require aditional selection criteria.
//my version [g. vian]

// Check if there are selected leptons
if (thisEvent->getSelLeptons()->size() == cut["nLeptons"] && thisEvent->getSelLeptons()->size() < 2) {
    cutflow["nlepton==1"] += 1;
    hCutFlow->Fill("nlepton==1", 1);
    hCutFlow_w->Fill("nlepton==1", _weight);

	    // Check if the leptons vector is not empty
	    if (!thisEvent->getSelLeptons()->empty()) {
	        // Access the first lepton in the list
	        objectLep* selectedLepton = thisEvent->getSelLeptons()->at(0);
	
	        // Check if the lepton is an electron and if pT is greater than leadElePt
	        if (selectedLepton->flavor == objectLep::kEle && selectedLepton->getp4()->Pt() > cut["leadElePt"] ) {
	            // Event accepted, lepton is an electron with pT above leadElePt
	            return true;
	        }
	        // Check if the lepton is a muon and if pT is greater than leadMuonPt
	        else if (selectedLepton->flavor == objectLep::kMuon && selectedLepton->getp4()->Pt() > cut["leadMuonPt"]) {
	            // Event accepted, lepton is a muon with pT above leadMuonPt
	            return true;
	        } else {
	            // If the lepton does not meet the pT criteria
	            return false;
	        }
	    } else {
	        // If there are no selected leptons
	        return false;
	    }
	} else {
	    // If the number of leptons is not equal to the expected value
	    return false;
	}


  /* (original)
  if(!(thisEvent->getnSelLepton() == cut["nLeptons"])){
        return false;
    }
    cutflow["nlepton==1"]+=1;                 
    hCutFlow->Fill("nlepton==1", 1);
    hCutFlow_w->Fill("nlepton==1", _weight);
*/
	
// Get the statistics for the combination of selected jets and selected leptons (ljetStat), 
// which may include various metrics like the number of jets, energy, etc.
    thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelLeptons(), ljetStat);
    thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);


    if(!(thisEvent->getSumSelJetScalarpT() > cut["HT"])){ 
      return false;
    }
    cutflow["HT>0"]+=1;
    hCutFlow->Fill("HT>0",1);
    hCutFlow_w->Fill("HT>0",_weight);

// This sequence retrieves the missing transverse energy (MET) of the event by accessing its 4-momentum 
//vector and then extracting the transverse momentum (Pt) from it.	
    if(!(thisEvent->getMET()->getp4()->Pt() > cut["MET"])){  
      return false;
    }
    cutflow["MET>20"]+=1;
    hCutFlow->Fill("MET>20",1);
    hCutFlow_w->Fill("MET>20",_weight);


 
    ////if(!(thisEvent->getnLightJet() >= cut["nlJets"])){
    ////        return false;
    ////}
    ////cutflow["nljets>=2"]+=1;
    ////hCutFlow->Fill("nljets>=2",1);
    ////hCutFlow_w->Fill("nljets>=2",_weight);    


    const float W_mass = 80.377f;
    float closest_mass_difference = std::numeric_limits<float>::max();
    float closest_pair_mass_sum = 0.0f;
    std::vector<float> jet_masses = thisEvent->getSelJetsMass();

    for (size_t i = 0; i < jet_masses.size(); ++i) {
        for (size_t j = i + 1; j < jet_masses.size(); ++j) {
            float mass_sum = jet_masses[i] + jet_masses[j];
            float mass_difference = std::fabs(mass_sum - W_mass);

            if (mass_difference < closest_mass_difference) {
                closest_mass_difference = mass_difference;
                closest_pair_mass_sum = mass_sum;
            }
        }
    }
    hInvMassHadW->Fill( closest_pair_mass_sum, _weight);

    if( closest_pair_mass_sum > 250.0 || closest_pair_mass_sum < 30.0 ){
             return false;
    }
    cutflow["30<ljetsM<250"]+=1;
    hCutFlow->Fill("30<ljetsM<250",1);
    hCutFlow_w->Fill("30<ljetsM<250",_weight);


    ////if(thisEvent->getSelLeptons()->at(0)->charge == thisEvent->getSelLeptons()->at(1)->charge){
    ////	return false;
    ////}
    ////cutflow["nOpositeChargedLep"]+=1;

    ////if(!(thisEvent->getnVetoLepton()  == cut["nVetoLeptons"])){
    ////	return false;
    ////}

    
    ////if(thisEvent->getnSelMuon()  == cut["nLeptons"]){
    ////	if(!((thisEvent->getSelMuonsMass() > 20) && (thisEvent->getSelMuonsMass() < 76 || thisEvent->getSelMuonsMass() > 106))){
    ////        return false;
    ////    }
    ////}
    ////
    ////if(thisEvent->getnSelElectron()  == cut["nLeptons"]){
    ////	if(!((thisEvent->getSelElectronsMass() > 20) && (thisEvent->getSelElectronsMass() < 76 || thisEvent->getSelElectronsMass() > 106))){
    ////	    return false;
    ////	}
    ////}
    ////cutflow["nMassCut"]+=1;

    if(cut["trigger"] > 0 && thisEvent->getHadTriggerAccept() == true)
    {
        cutflow["HadTrigger"]+=1;                 
        hCutFlow->Fill("HadTrigger",1);
        hCutFlow_w->Fill("HadTrigger",_weight);
    }

    cutflow["nTotal"]+=1;

    /*	std::cout << x.first  // string (key)
		  << ':' 
		  << x.second // string's value 
		  << std::endl;
		  } */
	
   
    return true;
}


void ttHHanalyzer::motherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4, const float mother1mass, float & _minChi2,float & _bbMassMin1){
    float bbMass1, chi2;
    bbMass1 = (dPar1p4+dPar2p4).M();
    chi2 = pow((bbMass1 - mother1mass),2)/pow((dPar1p4.Pt()+dPar2p4.Pt())/2.*0.02,0.5);
    if(_minChi2 > chi2){
	_minChi2      = chi2;
	_bbMassMin1   = bbMass1;
	_bpTHiggs1    = (dPar1p4+dPar2p4).Pt();
    }
} 


void ttHHanalyzer::diMotherReco(const TLorentzVector & dPar1p4,const TLorentzVector & dPar2p4,const TLorentzVector & dPar3p4,const TLorentzVector & dPar4p4, const float mother1mass, const float  mother2mass, float & _minChi2,float & _bbMassMin1, float & _bbMassMin2){
    float bbMass1, bbMass2, chi2;
    bbMass1 = (dPar1p4+dPar2p4).M();
    bbMass2 = (dPar3p4+dPar4p4).M();
    chi2 = pow((bbMass1 - mother1mass),2)/pow((dPar1p4.Pt()+dPar2p4.Pt())/2.*0.2,0.5) + pow((bbMass2 - mother2mass),2)/pow((dPar3p4.Pt()+dPar4p4.Pt())/2.*0.02,0.5);
    if(_minChi2 > chi2){
	_minChi2      = chi2;
	_bbMassMin1   = bbMass1;
	_bbMassMin2   = bbMass2;
	_bpTHiggs1    = (dPar1p4+dPar2p4).Pt();
	_bpTHiggs2    = (dPar3p4+dPar4p4).Pt();
    }
} 

void ttHHanalyzer::analyze(event *thisEvent){

    std::vector<objectJet*>* bJetsInv = thisEvent->getSelbJets(); 
    std::vector<objectJet*>* lbJetsInv = thisEvent->getLoosebJets(); 
    std::vector<objectJet*>* jetsInv = thisEvent->getSelJets(); 

    std::vector<TVector3> vectorsJet, vectorsBjet;
    // Event Shape Calculation & genbjet matching for mother particle
    for(int k = 0; k < jetsInv->size(); k++){
	 vectorsJet.push_back(jetsInv->at(k)->getp4()->Vect());
    }
    for(int m = 0; m < bJetsInv->size(); m++){
	vectorsBjet.push_back(bJetsInv->at(m)->getp4()->Vect());
	if(thisEvent->getnGenPart() < 1) continue;
	bJetsInv->at(m)->matchedtoHiggs = false;	    	
	for(auto genParticle: *thisEvent->getGenParts()){
	    float dR = bJetsInv->at(m)->getp4()->DeltaR( *genParticle->getp4());
	    if(genParticle->hasHiggsMother == true && dR < 0.8 && (fabs(bJetsInv->at(m)->getp4()->Pt() - (*genParticle->getp4()).Pt()) < bJetsInv->at(m)->getp4()->Pt()*0.4) ){
	      if(dR > genParticle->dRmatched) {
		  //std::cout << "this was matched to a closer particle before" << std::endl;
	      }else if( genParticle->matched){
		  //std::cout << "this was matched before" << std::endl;
	      }
	      bJetsInv->at(m)->matchedtoHiggs   = true;	
	      bJetsInv->at(m)->matchedtoHiggsdR = dR;	
	      genParticle->matched = true;
	      genParticle->dRmatched = dR;
	      break;
	    }
	} 
	//	std::cout << bJetsInv->at(m)->matchedtoHiggsb << std::endl;  
    }

    _minChi2Higgs  = cLargeValue;
    _minChi2Z      = cLargeValue;
    _minChi2HiggsZ = cLargeValue;
    _minChi2SHiggsNotMatched = cLargeValue;
    _minChi2SHiggsMatched = cLargeValue;
    _minChi2HHNotMatched = cLargeValue;
    _minChi2HHMatched = cLargeValue;
    _bbMassMinSHiggsMatched = -1;
    _bbMassMinSHiggsNotMatched = -1;
    _bbMassMinHH1Matched = -1;
    _bbMassMinHH1NotMatched = -1;
    _bbMassMinHH2Matched = -1;
    _bbMassMinHH2NotMatched = -1;

    float tempminChi2 = cLargeValue, tmpMassMin1HiggsZ = 0., tmpMassMin2HiggsZ = 0.;
    float tempMinChi2SHiggs = cLargeValue, tempMinChi2SHiggs_r = cLargeValue, tmpMassMinSHiggs = 0.;
    float tempMinChi2SHiggsMatched = cLargeValue, tempMinChi2SHiggsMatched_r = cLargeValue, tmpMassMinSHiggsMatched = 0.;
    float tempMinChi2SHiggsNotMatched = cLargeValue, tempMinChi2SHiggsNotMatched_r = cLargeValue, tmpMassMinSHiggsNotMatched = 0.;
    //extract H
    for( int ibjet1 = 0; ibjet1 < bJetsInv->size(); ibjet1++){
	tempMinChi2SHiggs = cLargeValue;
	bJetsInv->at(ibjet1)->minChiHiggsIndex = -1;
	for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
	    if( ibjet1 == ibjet2) continue;	   
	    tempMinChi2SHiggs_r = tempMinChi2SHiggs;
	    motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, tempMinChi2SHiggs, tmpMassMinSHiggs);
	    if(tempMinChi2SHiggs_r > tempMinChi2SHiggs){
		bJetsInv->at(ibjet1)->minChiHiggsIndex = ibjet2;
	    }
	    if(bJetsInv->at(ibjet1)->matchedtoHiggs == true && bJetsInv->at(ibjet2)->matchedtoHiggs == true){
		motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, _minChi2SHiggsMatched, _bbMassMinSHiggsMatched);		
	    } else if (bJetsInv->at(ibjet1)->matchedtoHiggs == false && bJetsInv->at(ibjet2)->matchedtoHiggs == false){
		motherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4(),cHiggsMass, _minChi2SHiggsNotMatched, _bbMassMinSHiggsNotMatched);		
	    }
	}
	bJetsInv->at(ibjet1)->minChiHiggs = tempMinChi2SHiggs;
    }


    // HH & ZZ reco : 4 medium b jet case

    if(thisEvent->getnbJet() >  3){
	for( int ibjet1 = 0; ibjet1 < bJetsInv->size(); ibjet1++){
	    for( int ibjet2 = ibjet1+1; ibjet2 < bJetsInv->size(); ibjet2++){
		if( ibjet1 == ibjet2) continue;
		for( int ibjet3 = 1; ibjet3 < bJetsInv->size(); ibjet3++){
		    if(ibjet1 == ibjet3 || ibjet2 == ibjet3) continue;
		    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
			if(ibjet1 == ibjet4 || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
			if(bJetsInv->at(ibjet1)->matchedtoHiggs == true && bJetsInv->at(ibjet2)->matchedtoHiggs == true && bJetsInv->at(ibjet3)->matchedtoHiggs == true && bJetsInv->at(ibjet4)->matchedtoHiggs == true){
			    diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
					 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
					 , cHiggsMass, cHiggsMass, _minChi2HHMatched, _bbMassMinHH1Matched, _bbMassMinHH2Matched);
			} else {
			    diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
					 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
					 , cHiggsMass, cHiggsMass, _minChi2HHNotMatched, _bbMassMinHH1NotMatched, _bbMassMinHH2NotMatched);
			}
			diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
				     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
				     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
			diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
				     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
				     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z);  
			diMotherReco(*bJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() //ZH 
				     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
				     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ); 
		    }
		}
	    }
	}
	// HH & ZZ reco : 3 medium + 1 loose b jet case
    }
    else if(thisEvent->getnbJet() == 3 && thisEvent->getnbLooseJet() > 3){
        for( int ibjet1 = 0; ibjet1 < lbJetsInv->size(); ibjet1++){
            for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
        	if( lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet2)) continue;
        	for( int ibjet3 = 0; ibjet3 < bJetsInv->size(); ibjet3++){
        	    if(lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet3) || ibjet2 == ibjet3) continue;
        	    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
        		if(lbJetsInv->at(ibjet1) == bJetsInv->at(ibjet4) || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
        		if( bJetsInv->at(ibjet2)->matchedtoHiggs == true && bJetsInv->at(ibjet3)->matchedtoHiggs == true && bJetsInv->at(ibjet4)->matchedtoHiggs == true){
        		    diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
        				 , cHiggsMass, cHiggsMass, _minChi2HHMatched, _bbMassMinHH1Matched, _bbMassMinHH2Matched);
        		} else {
        		    diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        				 , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4() 
        				 , cHiggsMass, cHiggsMass, _minChi2HHNotMatched, _bbMassMinHH1NotMatched, _bbMassMinHH2NotMatched);
        		}

        		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs); 
        		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z); 
        		// ZH combinatorics 
        		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH H(lbb)Z(bb)
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ);  
        		tempminChi2 = _minChi2HiggsZ; tmpMassMin1HiggsZ = _bbMassMin1HiggsZ; tmpMassMin2HiggsZ = _bbMassMin2HiggsZ;
        		diMotherReco(*lbJetsInv->at(ibjet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH Z(lbb)H(bb)
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cZMass, cHiggsMass, _minChi2HiggsZ, _bbMassMin2HiggsZ, _bbMassMin1HiggsZ); 
    		// pick the lowest minChi2 for the two combinatorics
        		if(tempminChi2 < _minChi2HiggsZ){
        		    _minChi2HiggsZ = tempminChi2; _bbMassMin1HiggsZ = tmpMassMin1HiggsZ; _bbMassMin2HiggsZ = tmpMassMin2HiggsZ;
        		}
        	    }
        	}
            }
        }
        // HH & ZZ reco : 3 medium b jet + 1 jet case 
    }
    else if(thisEvent->getnbJet() == 3){
        for( int ijet1 = 0; ijet1 < jetsInv->size(); ijet1++){
            for( int ibjet2 = 0; ibjet2 < bJetsInv->size(); ibjet2++){
        	if( jetsInv->at(ijet1) == bJetsInv->at(ibjet2)) continue;
        	for( int ibjet3 = 0; ibjet3 < bJetsInv->size(); ibjet3++){
        	    if(jetsInv->at(ijet1) == bJetsInv->at(ibjet3) || ibjet2 == ibjet3) continue;
        	    for( int ibjet4 = ibjet3+1; ibjet4 < bJetsInv->size(); ibjet4++){
        		if(jetsInv->at(ijet1) == bJetsInv->at(ibjet4) || ibjet2 == ibjet4 || ibjet3 == ibjet4 ) continue;		
        		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cHiggsMass, cHiggsMass, _minChi2Higgs, _bbMassMin1Higgs, _bbMassMin2Higgs);
        		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4()
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cZMass, cZMass, _minChi2Z, _bbMassMin1Z, _bbMassMin2Z);
        		// ZH combinatorics 
        		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH H(jb)Z(bb)
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cHiggsMass, cZMass, _minChi2HiggsZ, _bbMassMin1HiggsZ, _bbMassMin2HiggsZ);
        		tempminChi2 = _minChi2HiggsZ; tmpMassMin1HiggsZ = _bbMassMin1HiggsZ; tmpMassMin2HiggsZ = _bbMassMin2HiggsZ;
        		diMotherReco(*jetsInv->at(ijet1)->getp4(), *bJetsInv->at(ibjet2)->getp4() // ZH Z(jb)H(bb)
        			     , *bJetsInv->at(ibjet3)->getp4(), *bJetsInv->at(ibjet4)->getp4()
        			     , cZMass, cHiggsMass, _minChi2HiggsZ, _bbMassMin2HiggsZ, _bbMassMin1HiggsZ);
        		// pick the lowest minChi2 for the two combinatorics
        		if(tempminChi2 < _minChi2HiggsZ){
        		    _minChi2HiggsZ = tempminChi2; _bbMassMin1HiggsZ = tmpMassMin1HiggsZ; _bbMassMin2HiggsZ = tmpMassMin2HiggsZ;
        		}
        	    }
        	}
            }
        }
    }
    

    //    std::map<std::string, float> testVars = HypoComb->GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    HypoComb.GetBestPermutation(getLepP4(thisEvent),getJetP4(thisEvent),getJetCSV(thisEvent),*(thisEvent->getMET()->getp4()));
    //    std::cout<< "BLR: " << testVars["Evt_blr"] << std::endl;

    thisEvent->eventShapeJet  = new EventShape(vectorsJet);
    thisEvent->eventShapeBjet  = new EventShape(vectorsBjet);
}


void ttHHanalyzer::process(event* thisEvent, sysName sysType, bool up){
    createObjects(thisEvent, sysType, up);
    if(!selectObjects(thisEvent))  return;
    analyze(thisEvent);
    fillHistos(thisEvent);
    fillTree(thisEvent);
}


void ttHHanalyzer::fillHistos(event * thisEvent){

    //    for(int i=0; i < cutflow.size(); i++){
    //	std::cout << "cutflow size: " << cutflow[i] << std::endl; 
	//	hCutFlow->SetBinContent(1, cutflow[0]);
    //    }
    
    int i = 0; 
    for (const auto& x : cutflow){
	//hCutFlow->SetBinContent(i, x.second);
	i++;
    }
    
    thisEvent->getCentrality(thisEvent->getSelJets(), thisEvent->getSelbJets(), jbjetCent);
    thisEvent->getCentrality(thisEvent->getSelJets(), thisEvent->getSelLeptons(), jlepCent);
    thisEvent->getStats(thisEvent->getSelJets(), jetStat);
    thisEvent->getStats(thisEvent->getSelbJets(), bjetStat);
    thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelbJets(), bjStat);
    thisEvent->getMaxPTComb(thisEvent->getSelJets(), thisEvent->getSelbJets(), jbbMaxs);
    thisEvent->getMaxPTSame(thisEvent->getSelJets(), jjjMaxs);

    thisEvent->getStatsComb(thisEvent->getSelJets(), thisEvent->getSelLeptons(), ljetStat);
    thisEvent->getStatsComb(thisEvent->getSelbJets(), thisEvent->getSelLeptons(), lbjetStat);
    
    thisEvent->getFoxWolfram(thisEvent->getSelJets(), jetFoxWolfMom);
    thisEvent->getFoxWolfram(thisEvent->getSelbJets(), bjetFoxWolfMom);

    //    std::cout << "Number of Hadronic Higgs: " << thisEvent->getnHadronicHiggs() << std::endl;

    hjetNumber->Fill(thisEvent->getnSelJet(),_weight*thisEvent->getbTagSys());
    hHadronicHiggsNumber->Fill(thisEvent->getnHadronicHiggs(),_weight*thisEvent->getbTagSys());
    hBjetNumber->Fill(thisEvent->getnbJet(),_weight*thisEvent->getbTagSys());
    hLightJetNumber->Fill(thisEvent->getnLightJet(),_weight*thisEvent->getbTagSys());

    hjetAverageMass->Fill(thisEvent->getSumSelJetMass()/(float)thisEvent->getnSelJet(),_weight*thisEvent->getbTagSys());
    hHadronicHiggsAverageMass->Fill(thisEvent->getSumSelHadronicHiggsMass()/(float)thisEvent->getnHadronicHiggs(),_weight*thisEvent->getbTagSys());
    hBjetAverageMass->Fill(thisEvent->getSumSelbJetMass()/(float)thisEvent->getnbJet(),_weight*thisEvent->getbTagSys());
    hLightJetAverageMass->Fill(thisEvent->getSumSelLightJetMass()/(float)thisEvent->getnLightJet(),_weight*thisEvent->getbTagSys());
    hBjetAverageMassSqr->Fill((thisEvent->getSumSelbJetMass()*thisEvent->getSumSelbJetMass())/(float)thisEvent->getnbJet(), _weight*thisEvent->getbTagSys());

    if(thisEvent->getnHadronicHiggs() > 0){ 
	hHadronicHiggsSoftDropMass1->Fill(thisEvent->getSelHadronicHiggses()->at(0)->softDropMass,_weight*thisEvent->getbTagSys());
    }

    if(thisEvent->getnHadronicHiggs() > 1){ 
	hHadronicHiggsSoftDropMass2->Fill(thisEvent->getSelHadronicHiggses()->at(1)->softDropMass,_weight*thisEvent->getbTagSys());
    }


    hjetHT->Fill(thisEvent->getSumSelJetScalarpT(),_weight*thisEvent->getbTagSys());
    hBjetHT->Fill(thisEvent->getSumSelbJetScalarpT(),_weight*thisEvent->getbTagSys());
    hHadronicHiggsHT->Fill(thisEvent->getSumSelHadronicHiggsScalarpT(),_weight*thisEvent->getbTagSys());
    hLightJetHT->Fill(thisEvent->getSumSelLightJetScalarpT(),_weight*thisEvent->getbTagSys());
    hAvgDeltaRjj->Fill(jetStat.meandR,_weight*thisEvent->getbTagSys());
    hAvgDeltaEtajj->Fill(jetStat.meandEta,_weight*thisEvent->getbTagSys());
    hminDeltaRjj->Fill(jetStat.mindR,_weight*thisEvent->getbTagSys());
    hminDeltaRMassjj->Fill(jetStat.mindRMass,_weight*thisEvent->getbTagSys());
    hminDeltaRpTjj->Fill(jetStat.mindRpT,_weight*thisEvent->getbTagSys());
    hAvgDeltaRbb->Fill(bjetStat.meandR,_weight*thisEvent->getbTagSys());
    hAvgDeltaRbj->Fill(bjStat.meandR,_weight*thisEvent->getbTagSys());
    hAvgDeltaEtabj->Fill(bjStat.meandEta,_weight*thisEvent->getbTagSys());
    hminDeltaRbj->Fill(bjStat.mindR,_weight*thisEvent->getbTagSys());
    hminDeltaRMassbj->Fill(bjStat.mindRMass,_weight*thisEvent->getbTagSys());
    hminDeltaRpTbj->Fill(bjStat.mindRpT,_weight*thisEvent->getbTagSys());    
    hAvgDeltaEtabb->Fill(bjetStat.meandEta,_weight*thisEvent->getbTagSys());
    hminDeltaRbb->Fill(bjetStat.mindR,_weight*thisEvent->getbTagSys());
    hminDeltaRMassbb->Fill(bjetStat.mindRMass,_weight*thisEvent->getbTagSys());
    hminDeltaRpTbb->Fill(bjetStat.mindRpT,_weight*thisEvent->getbTagSys());
    hmaxDeltaEtabb->Fill(bjetStat.maxdEta,_weight*thisEvent->getbTagSys());
    hmaxDeltaEtajj->Fill(jetStat.maxdEta,_weight*thisEvent->getbTagSys());
    hAvgDeltaEtabj->Fill(bjStat.meandEta,_weight*thisEvent->getbTagSys());
    hmaxDeltaEtabj->Fill(bjStat.maxdEta,_weight*thisEvent->getbTagSys());
    hmaxPTmassjbb->Fill(jbbMaxs.maxPTmass, _weight*thisEvent->getbTagSys());
    hmaxPTmassjjj->Fill(jjjMaxs.maxPTmass, _weight*thisEvent->getbTagSys());

    hInvMassHSingleMatched->Fill(_bbMassMinSHiggsMatched,_weight*thisEvent->getbTagSys());
    hInvMassHSingleNotMatched->Fill(_bbMassMinSHiggsNotMatched,_weight*thisEvent->getbTagSys());
    hChi2HiggsSingleMatched->Fill(_minChi2SHiggsMatched,_weight*thisEvent->getbTagSys());
    hChi2HiggsSingleNotMatched->Fill(_minChi2SHiggsNotMatched,_weight*thisEvent->getbTagSys());

    hInvMassHH1Matched->Fill(_bbMassMinHH1Matched,_weight*thisEvent->getbTagSys());
    hInvMassHH1NotMatched->Fill(_bbMassMinHH1NotMatched,_weight*thisEvent->getbTagSys());
    hInvMassHH2Matched->Fill(_bbMassMinHH2Matched,_weight*thisEvent->getbTagSys());
    hInvMassHH2NotMatched->Fill(_bbMassMinHH2NotMatched,_weight*thisEvent->getbTagSys());
    hChi2HHMatched->Fill(_minChi2HHMatched,_weight*thisEvent->getbTagSys());
    hChi2HHNotMatched->Fill(_minChi2HHNotMatched,_weight*thisEvent->getbTagSys());


    hInvMassH1->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
    hInvMassH2->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
    hInvMassH1_zoomIn->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
    hInvMassH2_zoomIn->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
    hPTH1->Fill(_bpTHiggs1,_weight*thisEvent->getbTagSys());
    hPTH2->Fill(_bpTHiggs2,_weight*thisEvent->getbTagSys());
    hInvMassZ1->Fill(_bbMassMin1Z,_weight*thisEvent->getbTagSys());
    hInvMassZ2->Fill(_bbMassMin2Z,_weight*thisEvent->getbTagSys());
    hInvMassZ1_zoomIn->Fill(_bbMassMin1Z,_weight*thisEvent->getbTagSys());
    hInvMassZ2_zoomIn->Fill(_bbMassMin2Z,_weight*thisEvent->getbTagSys());
    hInvMassHZ1->Fill(_bbMassMin1HiggsZ,_weight*thisEvent->getbTagSys());
    hInvMassHZ2->Fill(_bbMassMin2HiggsZ,_weight*thisEvent->getbTagSys());
    hInvMassHZ1_zoomIn->Fill(_bbMassMin1HiggsZ,_weight*thisEvent->getbTagSys());
    hInvMassHZ2_zoomIn->Fill(_bbMassMin2HiggsZ,_weight*thisEvent->getbTagSys());

    if(fabs(_bbMassMin1Higgs-cHiggsMass) < fabs(_bbMassMin2Higgs-cHiggsMass)){
	hInvMassH1mChi->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
	hInvMassH2mChi->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
    } else  {
	hInvMassH2mChi->Fill(_bbMassMin1Higgs,_weight*thisEvent->getbTagSys());
	hInvMassH1mChi->Fill(_bbMassMin2Higgs,_weight*thisEvent->getbTagSys());
    }
    hChi2Higgs->Fill(_minChi2Higgs,_weight*thisEvent->getbTagSys());
    hChi2Z->Fill(_minChi2Z,_weight*thisEvent->getbTagSys());
    hChi2HiggsZ->Fill(_minChi2HiggsZ,_weight*thisEvent->getbTagSys());

    
    hmet->Fill(thisEvent->getMET()->getp4()->Pt(),_weight*thisEvent->getbTagSys());
    //    hmetPhi->Fill(thisEvent->getMET()->getp4()->Phi(),_weight*thisEvent->getbTagSys());
    // hmetEta->Fill(thisEvent->getMET()->getp4()->Eta(),_weight*thisEvent->getbTagSys());
    
    hAplanarity->Fill(thisEvent->eventShapeJet->getAplanarity(), _weight*thisEvent->getbTagSys());
    hSphericity->Fill(thisEvent->eventShapeJet->getSphericity(), _weight*thisEvent->getbTagSys());
    hTransSphericity->Fill(thisEvent->eventShapeJet->getTransSphericity(), _weight*thisEvent->getbTagSys());
    hCvalue->Fill(thisEvent->eventShapeJet->getC(), _weight*thisEvent->getbTagSys());
    hDvalue->Fill(thisEvent->eventShapeJet->getD(), _weight*thisEvent->getbTagSys());
    hCentralityjb->Fill(jbjetCent.centrality, _weight*thisEvent->getbTagSys());    
    hCentralityjl->Fill(jlepCent.centrality, _weight*thisEvent->getbTagSys());    

    hH0->Fill(jetFoxWolfMom.h0, _weight*thisEvent->getbTagSys());
    hH1->Fill(jetFoxWolfMom.h1, _weight*thisEvent->getbTagSys());
    hH2->Fill(jetFoxWolfMom.h2, _weight*thisEvent->getbTagSys());
    hH3->Fill(jetFoxWolfMom.h3, _weight*thisEvent->getbTagSys());
    hH4->Fill(jetFoxWolfMom.h4, _weight*thisEvent->getbTagSys());
    hR1->Fill(jetFoxWolfMom.r1, _weight*thisEvent->getbTagSys());
    hR2->Fill(jetFoxWolfMom.r2, _weight*thisEvent->getbTagSys());
    hR3->Fill(jetFoxWolfMom.r3, _weight*thisEvent->getbTagSys());
    hR4->Fill(jetFoxWolfMom.r4, _weight*thisEvent->getbTagSys()); 


    hBjetH0->Fill(bjetFoxWolfMom.h0, _weight*thisEvent->getbTagSys());
    hBjetH1->Fill(bjetFoxWolfMom.h1, _weight*thisEvent->getbTagSys());
    hBjetH2->Fill(bjetFoxWolfMom.h2, _weight*thisEvent->getbTagSys());
    hBjetH3->Fill(bjetFoxWolfMom.h3, _weight*thisEvent->getbTagSys());
    hBjetH4->Fill(bjetFoxWolfMom.h4, _weight*thisEvent->getbTagSys());
    hBjetR1->Fill(bjetFoxWolfMom.r1, _weight*thisEvent->getbTagSys());
    hBjetR2->Fill(bjetFoxWolfMom.r2, _weight*thisEvent->getbTagSys());
    hBjetR3->Fill(bjetFoxWolfMom.r3, _weight*thisEvent->getbTagSys());
    hBjetR4->Fill(bjetFoxWolfMom.r4, _weight*thisEvent->getbTagSys()); 


    hBjetAplanarity->Fill(thisEvent->eventShapeBjet->getAplanarity(), _weight*thisEvent->getbTagSys());
    hBjetSphericity->Fill(thisEvent->eventShapeBjet->getSphericity(), _weight*thisEvent->getbTagSys());
    hBjetTransSphericity->Fill(thisEvent->eventShapeBjet->getTransSphericity(), _weight*thisEvent->getbTagSys());
    hBjetCvalue->Fill(thisEvent->eventShapeBjet->getC(), _weight*thisEvent->getbTagSys());
    hBjetDvalue->Fill(thisEvent->eventShapeBjet->getD(), _weight*thisEvent->getbTagSys());


    for(int ih=0; ih < thisEvent->getnSelJet() && ih < nHistsJets; ih++){
	hjetsPTs.at(ih)->Fill(thisEvent->getSelJets()->at(ih)->getp4()->Pt(),_weight*thisEvent->getbTagSys());
	hjetsEtas.at(ih)->Fill(thisEvent->getSelJets()->at(ih)->getp4()->Eta(),_weight*thisEvent->getbTagSys());
	hjetsBTagDisc.at(ih)->Fill(getJetCSV(thisEvent).at(ih),_weight*thisEvent->getbTagSys());
    }

    for(int ih=0; ih < thisEvent->getnbJet() && ih < nHistsbJets; ih++){
	hbjetsPTs.at(ih)->Fill(thisEvent->getSelbJets()->at(ih)->getp4()->Pt(),_weight*thisEvent->getbTagSys());
	hbjetsEtas.at(ih)->Fill(thisEvent->getSelbJets()->at(ih)->getp4()->Eta(),_weight*thisEvent->getbTagSys());
	hbjetsBTagDisc.at(ih)->Fill(getbJetCSV(thisEvent).at(ih),_weight*thisEvent->getbTagSys());
    }


    for(int ih=0; ih < thisEvent->getnLightJet() && ih < nHistsLightJets; ih++){
	hLightJetsPTs.at(ih)->Fill(thisEvent->getSelLightJets()->at(ih)->getp4()->Pt(),_weight*thisEvent->getbTagSys());
	hLightJetsEtas.at(ih)->Fill(thisEvent->getSelLightJets()->at(ih)->getp4()->Eta(),_weight*thisEvent->getbTagSys());
	hLightJetsBTagDisc.at(ih)->Fill(getlightJetCSV(thisEvent).at(ih),_weight*thisEvent->getbTagSys());
    }

    hleptonNumber->Fill(thisEvent->getnSelLepton(),_weight*thisEvent->getbTagSys());
    hElecNumber->Fill(thisEvent->getnSelElectron(),_weight*thisEvent->getbTagSys());
    hMuonNumber->Fill(thisEvent->getnSelMuon(),_weight*thisEvent->getbTagSys());


    //if(thisEvent->getnSelMuon() == 2){
//	hDiMuonMass->Fill(thisEvent->getSelMuonsMass(),_weight*thisEvent->getbTagSys());
//	hDiMuonPT->Fill(thisEvent->getSelMuonsPT(),_weight*thisEvent->getbTagSys());
//	hDiMuonEta->Fill(thisEvent->getSelMuonsEta(),_weight*thisEvent->getbTagSys());
 //   }

//    if(thisEvent->getnSelElectron() == 2){
//	hDiElectronMass->Fill(thisEvent->getSelElectronsMass(),_weight*thisEvent->getbTagSys());
//	hDiElectronPT->Fill(thisEvent->getSelElectronsPT(),_weight*thisEvent->getbTagSys());
//	hDiElectronEta->Fill(thisEvent->getSelElectronsEta(),_weight*thisEvent->getbTagSys());
 //   }

    hleptonHT->Fill(thisEvent->getSelLeptonHT(),_weight*thisEvent->getbTagSys());
    hST->Fill(thisEvent->getSelLeptonST(),_weight*thisEvent->getbTagSys());
    hLeptonPT1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
    hLeptonEta1->Fill(thisEvent->getSelLeptons()->at(0)->getp4()->Eta(), _weight*thisEvent->getbTagSys());
//    hLeptonPT2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
//    hLeptonEta2->Fill(thisEvent->getSelLeptons()->at(1)->getp4()->Eta(), _weight*thisEvent->getbTagSys());


    if(thisEvent->getnSelMuon() > 0){
	hMuonPT1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
	hMuonEta1->Fill(thisEvent->getSelMuons()->at(0)->getp4()->Eta(), _weight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 0){
	hElePT1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
	hEleEta1->Fill(thisEvent->getSelElectrons()->at(0)->getp4()->Eta(), _weight*thisEvent->getbTagSys());
    }
    /*
    if(thisEvent->getnSelMuon() > 1){
	hMuonPT2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
	hMuonEta2->Fill(thisEvent->getSelMuons()->at(1)->getp4()->Eta(), _weight*thisEvent->getbTagSys());
    }
    
    if(thisEvent->getnSelElectron() > 1){
	hElePT2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Pt(), _weight*thisEvent->getbTagSys());
	hEleEta2->Fill(thisEvent->getSelElectrons()->at(1)->getp4()->Eta(), _weight*thisEvent->getbTagSys());
    } */

    hLepCharge1->Fill(thisEvent->getSelLeptons()->at(0)->charge, _weight*thisEvent->getbTagSys());
   // hLepCharge2->Fill(thisEvent->getSelLeptons()->at(1)->charge, _weight*thisEvent->getbTagSys());
    
}



void ttHHanalyzer::writeHistos(){
    _of->file->cd();
    _histoDirs.at(0)->cd();
    for(int ih=0; ih<nHistsJets; ih++){
	hjetsPTs.at(ih)->Write();
	hjetsEtas.at(ih)->Write();
	hjetsBTagDisc.at(ih)->Write();
    }
    for(int ih=0; ih<nHistsbJets; ih++){
	hbjetsPTs.at(ih)->Write();
	hbjetsEtas.at(ih)->Write();
	hbjetsBTagDisc.at(ih)->Write();
    }

    for(int ih=0; ih<nHistsLightJets; ih++){
	hLightJetsPTs.at(ih)->Write();
	hLightJetsEtas.at(ih)->Write();
	hLightJetsBTagDisc.at(ih)->Write();
    }

    hInvMassHadW->Write();

    hInvMassHSingleMatched->Write();
    hInvMassHSingleNotMatched->Write();
    hChi2HiggsSingleNotMatched->Write();
    hChi2HiggsSingleMatched->Write();
    hInvMassHH1Matched->Write();
    hInvMassHH1NotMatched->Write();
    hInvMassHH2Matched->Write();
    hInvMassHH2NotMatched->Write();
    hChi2HHNotMatched->Write();
    hChi2HHMatched->Write();

    hjetNumber->Write();
    hBjetNumber->Write();
    hHadronicHiggsNumber->Write();
    hLightJetNumber->Write();
    hjetAverageMass->Write();
    hBjetAverageMass->Write();
    hHadronicHiggsAverageMass->Write();
    hLightJetAverageMass->Write();
    hBjetAverageMassSqr->Write();
    hHadronicHiggsSoftDropMass1->Write();
    hHadronicHiggsSoftDropMass2->Write();
    hjetHT->Write();
    hBjetHT->Write();
    hHadronicHiggsHT->Write();
    hLightJetHT->Write();
    hAvgDeltaRjj->Write();
    hminDeltaRjj->Write();
    hminDeltaRMassjj->Write();
    hminDeltaRpTjj->Write();
    hAvgDeltaRbb->Write();
    hAvgDeltaEtajj->Write();
    hAvgDeltaEtabb->Write();
    hminDeltaRbb->Write();
    hminDeltaRMassbb->Write();
    hminDeltaRpTbb->Write();
    hmaxDeltaEtabb->Write();
    hmaxDeltaEtajj->Write();
    hAvgDeltaRbj->Write();
    hAvgDeltaEtabj->Write();
    hminDeltaRbj->Write();
    hminDeltaRMassbj->Write();
    hminDeltaRpTbj->Write();
    hmaxDeltaEtabj->Write();
    hmaxPTmassjbb->Write();
    hmaxPTmassjjj->Write();

    hPTH1->Write();
    hPTH2->Write();
    hInvMassH1->Write();
    hInvMassH2->Write();
    hInvMassH1_zoomIn->Write();
    hInvMassH2_zoomIn->Write();
    hInvMassH1mChi->Write();
    hInvMassH2mChi->Write();
    hInvMassHZ1->Write();
    hInvMassHZ2->Write();
    hInvMassHZ1_zoomIn->Write();
    hInvMassHZ2_zoomIn->Write();
    hInvMassZ1->Write();
    hInvMassZ2->Write();
    hInvMassZ1_zoomIn->Write();
    hInvMassZ2_zoomIn->Write();
    hChi2Higgs->Write();
    hChi2HiggsZ->Write();
    hChi2Z->Write();


    hmet->Write();
    // hmetPhi->Write();
    // hmetEta->Write();

    hAplanarity->Write();
    hSphericity->Write();
    hTransSphericity->Write();
    hCvalue->Write();
    hDvalue->Write();
    hCentralityjb->Write();
    hCentralityjl->Write();

    hH0->Write();
    hH1->Write();
    hH2->Write();
    hH3->Write();
    hH4->Write();
    hR1->Write();
    hR2->Write();
    hR3->Write();
    hR4->Write(); 

    hBjetH0->Write();
    hBjetH1->Write();
    hBjetH2->Write();
    hBjetH3->Write();
    hBjetH4->Write();
    hBjetR1->Write();
    hBjetR2->Write();
    hBjetR3->Write();
    hBjetR4->Write(); 

    hBjetAplanarity->Write();
    hBjetSphericity->Write();
    hBjetTransSphericity->Write();
    hBjetCvalue->Write();
    hBjetDvalue->Write();

    _histoDirs.at(1)->cd();
    
    hLepCharge1->Write();
    //hLepCharge2->Write();

    hleptonNumber->Write();
    hElecNumber->Write();
    hMuonNumber->Write();
   // hDiElectronMass->Write();
   // hDiMuonPT->Write();
  //  hDiElectronPT->Write();
   // hDiMuonEta->Write();
  //  hDiElectronEta->Write();
    hleptonHT->Write();
    hST->Write();
    hLeptonEta1->Write();
    hLeptonPT1->Write();
    hLeptonEta2->Write();
    hLeptonPT2->Write();
    
    hMuonEta1->Write();
    hMuonPT1->Write();
    hEleEta1->Write();
    hElePT1->Write();
//    hMuonEta2->Write();
//    hMuonPT2->Write();
//    hEleEta2->Write();
//   hElePT2->Write(); 
}
void ttHHanalyzer::fillTree(event * thisEvent){

////////////////////////////////////////////////////////////////////////////////////////
    // For Trigger Path
    passTrigger_HLT_IsoMu27 = _ev->HLT_IsoMu27; // Reference Muon Trigger
    passTrigger_HLT_PFHT1050 = _ev->HLT_PFHT1050;
    //passTrigger_HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59 = _ev->HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59;
    //passTrigger_HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94 = _ev->HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94;
    //passTrigger_HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5 = _ev->HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5;
    passTrigger_6J1T_B    = _ev->HLT_PFHT430_SixJet40_BTagCSV_p080;
    passTrigger_6J1T_CDEF = _ev->HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5;
    passTrigger_6J2T_B    = _ev->HLT_PFHT380_SixJet32_DoubleBTagCSV_p075;
    passTrigger_6J2T_CDEF = _ev->HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2;
    passTrigger_4J3T_B    = _ev->HLT_HT300PT30_QuadJet_75_60_45_40_TripeCSV_p07;
    passTrigger_4J3T_CDEF = _ev->HLT_PFHT300PT30_QuadPFJet_75_60_45_40_TriplePFBTagCSV_3p0;

    nMuons = thisEvent->getnSelMuon(); //maybe this is causing the problem
    nJets = thisEvent->getnSelJet();
    nbJets = thisEvent->getnbJet();
    HT = thisEvent->getSumSelJetScalarpT();

    // Fill the jet information [ It will fill the nJets && maximum 30th jets ]
    for (int i = 0; i < nJets && i < 30; ++i) {
        jetPt[i] = thisEvent->getSelJets()->at(i)->getp4()->Pt();
        jetEta[i] = thisEvent->getSelJets()->at(i)->getp4()->Eta();
        bTagScore[i] = thisEvent->getSelJets()->at(i)->bTagCSV;
    } 
    for (int i = nJets; i < 30; ++i) {
        jetPt[i] = -999;
        jetEta[i] = -999;
        bTagScore[i] = -999;
    }

    eventNumber = _ev->event;
    runNumber = _ev->run;

////////////////////////////////////////////////////////////////////////////////////////
   
    bjetPT1 = thisEvent->getSelJets()->at(0)->getp4()->Pt();
    bjetPT2 = thisEvent->getSelJets()->at(1)->getp4()->Pt();
    bjetPT3 = thisEvent->getSelJets()->at(2)->getp4()->Pt();
    bjetPT4 = thisEvent->getSelJets()->at(3)->getp4()->Pt();
    bjetEta1 = thisEvent->getSelJets()->at(0)->getp4()->Eta();
    bjetEta2 = thisEvent->getSelJets()->at(1)->getp4()->Eta();
    bjetEta3 = thisEvent->getSelJets()->at(2)->getp4()->Eta();
    bjetEta4 = thisEvent->getSelJets()->at(3)->getp4()->Eta();
    bjetBTagDisc1 = getJetCSV(thisEvent).at(0);
    bjetBTagDisc2 = getJetCSV(thisEvent).at(1);
    bjetBTagDisc3 = getJetCSV(thisEvent).at(2);
    bjetBTagDisc4 = getJetCSV(thisEvent).at(3);


    bbjetPT1 = thisEvent->getSelbJets()->at(0)->getp4()->Pt();
    bbjetPT2 = thisEvent->getSelbJets()->at(1)->getp4()->Pt();
    bbjetPT3 = thisEvent->getSelbJets()->at(2)->getp4()->Pt();
    bbjetEta1 = thisEvent->getSelbJets()->at(0)->getp4()->Eta();
    bbjetEta2 = thisEvent->getSelbJets()->at(1)->getp4()->Eta();
    bbjetEta3 = thisEvent->getSelbJets()->at(2)->getp4()->Eta();
    bbjetPhi1 = thisEvent->getSelbJets()->at(0)->getp4()->Phi();
    bbjetPhi2 = thisEvent->getSelbJets()->at(1)->getp4()->Phi();
    bbjetPhi3 = thisEvent->getSelbJets()->at(2)->getp4()->Phi();
    bbjetBTagDisc1 = getbJetCSV(thisEvent).at(0);
    bbjetBTagDisc2 = getbJetCSV(thisEvent).at(1);
    bbjetBTagDisc3 = getbJetCSV(thisEvent).at(2);
    bbjetHiggsMatched1 = thisEvent->getSelbJets()->at(0)->matchedtoHiggs;
    bbjetHiggsMatched2 = thisEvent->getSelbJets()->at(1)->matchedtoHiggs;
    bbjetHiggsMatched3 = thisEvent->getSelbJets()->at(2)->matchedtoHiggs;
    bbjetHiggsMatcheddR1 = thisEvent->getSelbJets()->at(0)->matchedtoHiggsdR;
    bbjetHiggsMatcheddR2 = thisEvent->getSelbJets()->at(1)->matchedtoHiggsdR;
    bbjetHiggsMatcheddR3 = thisEvent->getSelbJets()->at(2)->matchedtoHiggsdR;
    bbjetMinChiHiggsIndex1 = thisEvent->getSelbJets()->at(0)->minChiHiggsIndex;
    bbjetMinChiHiggsIndex2 = thisEvent->getSelbJets()->at(1)->minChiHiggsIndex;
    bbjetMinChiHiggsIndex3 = thisEvent->getSelbJets()->at(2)->minChiHiggsIndex;


    if(thisEvent->getnSelJet() > 4){
    	bjetPT5 = thisEvent->getSelJets()->at(4)->getp4()->Pt();
	bjetEta5 = thisEvent->getSelJets()->at(4)->getp4()->Eta();
	bjetBTagDisc5 = getJetCSV(thisEvent).at(4);
    } else{
	bjetPT5 = -6;
	bjetEta5 = -6;
	bjetBTagDisc5 = -6;
    }
    
    if(thisEvent->getnSelJet() > 5){
	bjetPT6 = thisEvent->getSelJets()->at(5)->getp4()->Pt();
	bjetEta6 = thisEvent->getSelJets()->at(5)->getp4()->Eta();
	bjetBTagDisc6 = getJetCSV(thisEvent).at(5);
    } else{
	bjetPT6 = -6;
	bjetEta6 = -6;
	bjetBTagDisc6 = -6;
    }


    if(thisEvent->getnSelJet() > 6){
	bjetPT7 = thisEvent->getSelJets()->at(6)->getp4()->Pt();
	bjetEta7 = thisEvent->getSelJets()->at(6)->getp4()->Eta();
	bjetBTagDisc7 = getJetCSV(thisEvent).at(6);
    } else{
	bjetPT7 = -6;
	bjetEta7 = -6;
	bjetBTagDisc7 = -6;
    }

    if(thisEvent->getnSelJet() > 7){
	bjetPT8 = thisEvent->getSelJets()->at(7)->getp4()->Pt();
	bjetEta8 = thisEvent->getSelJets()->at(7)->getp4()->Eta();
	bjetBTagDisc8 = getJetCSV(thisEvent).at(7);
    } else{
	bjetPT8 = -6;
	bjetEta8 = -6;
	bjetBTagDisc8 = -6;
    }

    if(thisEvent->getnSelJet() > 8){
	bjetPT9 = thisEvent->getSelJets()->at(8)->getp4()->Pt();
	bjetEta9 = thisEvent->getSelJets()->at(8)->getp4()->Eta();
	bjetBTagDisc9 = getJetCSV(thisEvent).at(8);
    } else{
	bjetPT9 = -6;
	bjetEta9 = -6;
	bjetBTagDisc9 = -6;
    }

    if(thisEvent->getnSelJet() > 9){
	bjetPT10 = thisEvent->getSelJets()->at(9)->getp4()->Pt();
	bjetEta10 = thisEvent->getSelJets()->at(9)->getp4()->Eta();
	bjetBTagDisc10 = getJetCSV(thisEvent).at(9);
    } else{
	bjetPT10 = -6;
	bjetEta10 = -6;
	bjetBTagDisc10 = -6;
    }

    if(thisEvent->getnSelJet() > 10){
	bjetPT11 = thisEvent->getSelJets()->at(10)->getp4()->Pt();
	bjetEta11 = thisEvent->getSelJets()->at(10)->getp4()->Eta();
	bjetBTagDisc11 = getJetCSV(thisEvent).at(10);
    } else{
	bjetPT11 = -6;
	bjetEta11 = -6;
	bjetBTagDisc11 = -6;
    }

    if(thisEvent->getnSelJet() > 11){
	bjetPT12 = thisEvent->getSelJets()->at(11)->getp4()->Pt();
	bjetEta12 = thisEvent->getSelJets()->at(11)->getp4()->Eta();
	bjetBTagDisc12 = getJetCSV(thisEvent).at(11);
    } else{
	bjetPT12 = -6;
	bjetEta12 = -6;
	bjetBTagDisc12 = -6;
    }

    // # b-jet
    if(thisEvent->getnbJet() > 3){
	bbjetPT4 = thisEvent->getSelbJets()->at(3)->getp4()->Pt();
	bbjetEta4 = thisEvent->getSelbJets()->at(3)->getp4()->Eta();
	bbjetPhi4 = thisEvent->getSelbJets()->at(3)->getp4()->Phi();
	bbjetBTagDisc4 = getbJetCSV(thisEvent).at(3);
	bbjetHiggsMatched4 = thisEvent->getSelbJets()->at(3)->matchedtoHiggs;
	bbjetHiggsMatcheddR4 = thisEvent->getSelbJets()->at(3)->matchedtoHiggsdR;
	bbjetMinChiHiggsIndex4 = thisEvent->getSelbJets()->at(3)->minChiHiggsIndex;
    } else {
	bbjetPT4 = -6;
	bbjetEta4 = -6;
	bbjetPhi4 = -6;
	bbjetBTagDisc4 = -6;
	bbjetHiggsMatched4 = 0;
	bbjetHiggsMatcheddR4 = -6;
	bbjetMinChiHiggsIndex4 = -6;
    }
    
    if(thisEvent->getnbJet() > 4){
	bbjetPT5 = thisEvent->getSelbJets()->at(4)->getp4()->Pt();
	bbjetEta5 = thisEvent->getSelbJets()->at(4)->getp4()->Eta();
	bbjetPhi5 = thisEvent->getSelbJets()->at(4)->getp4()->Phi();
	bbjetBTagDisc5 = getbJetCSV(thisEvent).at(4);
	bbjetHiggsMatched5 = thisEvent->getSelbJets()->at(4)->matchedtoHiggs;
	bbjetHiggsMatcheddR5 = thisEvent->getSelbJets()->at(4)->matchedtoHiggsdR;
	bbjetMinChiHiggsIndex5 = thisEvent->getSelbJets()->at(4)->minChiHiggsIndex;
    } else {
	bbjetPT5 = -6;
	bbjetEta5 = -6;
	bbjetPhi5 = -6;
	bbjetBTagDisc5 = -6;
	bbjetHiggsMatched5 = 0;
	bbjetHiggsMatcheddR5 = -6;
	bbjetMinChiHiggsIndex5 = -6;
    }

    if(thisEvent->getnbJet() > 5){
	bbjetPT6 = thisEvent->getSelbJets()->at(5)->getp4()->Pt();
	bbjetEta6 = thisEvent->getSelbJets()->at(5)->getp4()->Eta();
	bbjetPhi6 = thisEvent->getSelbJets()->at(5)->getp4()->Phi();
	bbjetBTagDisc6 = getbJetCSV(thisEvent).at(5);
	bbjetHiggsMatched6 = thisEvent->getSelbJets()->at(5)->matchedtoHiggs;
	bbjetHiggsMatcheddR6 = thisEvent->getSelbJets()->at(5)->matchedtoHiggsdR;
	bbjetMinChiHiggsIndex6 = thisEvent->getSelbJets()->at(5)->minChiHiggsIndex;
    } else {
	bbjetPT6 = -6;
	bbjetEta6 = -6;
	bbjetPhi6 = -6;
	bbjetBTagDisc6 = -6;
	bbjetHiggsMatched6 = 0;
	bbjetHiggsMatcheddR6 = -6;
	bbjetMinChiHiggsIndex6 = -6;
    }

    if(thisEvent->getnbJet() > 6){
	bbjetPT7 = thisEvent->getSelbJets()->at(6)->getp4()->Pt();
	bbjetEta7 = thisEvent->getSelbJets()->at(6)->getp4()->Eta();
	bbjetPhi7 = thisEvent->getSelbJets()->at(6)->getp4()->Phi();
	bbjetBTagDisc7 = getbJetCSV(thisEvent).at(6);
	bbjetHiggsMatched7 = thisEvent->getSelbJets()->at(6)->matchedtoHiggs;
	bbjetHiggsMatcheddR7 = thisEvent->getSelbJets()->at(6)->matchedtoHiggsdR;
    } else {
	bbjetPT7 = -6;
	bbjetEta7 = -6;
	bbjetPhi7 = -6;
	bbjetBTagDisc7 = -6;
	bbjetHiggsMatched7 = 0;
	bbjetHiggsMatcheddR7 = -6;
    }


    if(thisEvent->getnbJet() > 7){
	bbjetPT8 = thisEvent->getSelbJets()->at(7)->getp4()->Pt();
	bbjetEta8 = thisEvent->getSelbJets()->at(7)->getp4()->Eta();
	bbjetPhi8 = thisEvent->getSelbJets()->at(7)->getp4()->Phi();
	bbjetBTagDisc8 = getbJetCSV(thisEvent).at(7);
	bbjetHiggsMatched8 = thisEvent->getSelbJets()->at(7)->matchedtoHiggs;
	bbjetHiggsMatcheddR8 = thisEvent->getSelbJets()->at(7)->matchedtoHiggsdR;
    } else {
	bbjetPT8 = -6;
	bbjetEta8 = -6;
	bbjetPhi8 = -6;
	bbjetBTagDisc8 = -6;
	bbjetHiggsMatched8 = 0;
	bbjetHiggsMatcheddR8 = 0;
    }


    if(thisEvent->getnLightJet() > 0){
    	blightjetPT1 = thisEvent->getSelLightJets()->at(0)->getp4()->Pt();
	blightjetEta1 = thisEvent->getSelLightJets()->at(0)->getp4()->Eta();
	blightjetBTagDisc1 = getlightJetCSV(thisEvent).at(0);
    } else{
	blightjetPT1 = -6;
	blightjetEta1 = -6;
	blightjetBTagDisc1 = -6;
    }

    if(thisEvent->getnLightJet() > 1){
    	blightjetPT2 = thisEvent->getSelLightJets()->at(1)->getp4()->Pt();
	blightjetEta2 = thisEvent->getSelLightJets()->at(1)->getp4()->Eta();
	blightjetBTagDisc2 = getlightJetCSV(thisEvent).at(1);
    } else{
	blightjetPT2 = -6;
	blightjetEta2 = -6;
	blightjetBTagDisc2 = -6;
    }

    if(thisEvent->getnLightJet() > 2){
    	blightjetPT3 = thisEvent->getSelLightJets()->at(2)->getp4()->Pt();
	blightjetEta3 = thisEvent->getSelLightJets()->at(2)->getp4()->Eta();
	blightjetBTagDisc3 = getlightJetCSV(thisEvent).at(2);
    } else{
	blightjetPT3 = -6;
	blightjetEta3 = -6;
	blightjetBTagDisc3 = -6;
    }

    if(thisEvent->getnLightJet() > 3){
    	blightjetPT4 = thisEvent->getSelLightJets()->at(3)->getp4()->Pt();
	blightjetEta4 = thisEvent->getSelLightJets()->at(3)->getp4()->Eta();
	blightjetBTagDisc4 = getlightJetCSV(thisEvent).at(3);
    } else{
	blightjetPT4 = -6;
	blightjetEta4 = -6;
	blightjetBTagDisc4 = -6;
    }

    if(thisEvent->getnLightJet() > 4){
    	blightjetPT5 = thisEvent->getSelLightJets()->at(4)->getp4()->Pt();
	blightjetEta5 = thisEvent->getSelLightJets()->at(4)->getp4()->Eta();
	blightjetBTagDisc5 = getlightJetCSV(thisEvent).at(4);
    } else{
	blightjetPT5 = -6;
	blightjetEta5 = -6;
	blightjetBTagDisc5 = -6;
    }

    if(thisEvent->getnLightJet() > 5){
    	blightjetPT6 = thisEvent->getSelLightJets()->at(5)->getp4()->Pt();
	blightjetEta6 = thisEvent->getSelLightJets()->at(5)->getp4()->Eta();
	blightjetBTagDisc6 = getlightJetCSV(thisEvent).at(5);
    } else{
	blightjetPT6 = -6;
	blightjetEta6 = -6;
	blightjetBTagDisc6 = -6;
    }


    bweight= _weight;
    bjetAverageMass = thisEvent->getSumSelJetMass()/thisEvent->getnSelJet();
    bbJetAverageMass = thisEvent->getSumSelbJetMass()/thisEvent->getnbJet();
    blightJetAverageMass = thisEvent->getSumSelLightJetMass()/thisEvent->getnLightJet();
    bbJetAverageMassSqr = (thisEvent->getSumSelbJetMass()*thisEvent->getSumSelbJetMass())/thisEvent->getnbJet();
    bmet = thisEvent->getMET()->getp4()->Pt();
    baverageDeltaRjj = jetStat.meandR;
    baverageDeltaRbb = bjetStat.meandR;
    baverageDeltaRbj = bjStat.meandR;
    baverageDeltaEtajj = jetStat.meandEta;
    baverageDeltaEtabb = bjetStat.meandEta;
    baverageDeltaEtabj = bjStat.meandEta;
    bminDeltaRjj = jetStat.mindR;
    bminDeltaRbb = bjetStat.mindR;
    bminDeltaRbj = bjStat.mindR;
    bmaxDeltaEtabb = bjetStat.maxdEta;
    bmaxDeltaEtajj = jetStat.maxdEta;
    bmaxDeltaEtabj = bjStat.maxdEta;
    bminDeltaRMassjj = jetStat.mindRMass;
    bminDeltaRMassbb = bjetStat.mindRMass;
    bminDeltaRMassbj = bjStat.mindRMass;
    bminDeltaRpTjj = jetStat.mindRpT;
    bminDeltaRpTbb = bjetStat.mindRpT;
    bminDeltaRpTbj = bjStat.mindRpT;
    bmaxPTmassjjj = jjjMaxs.maxPTmass;
    bmaxPTmassjbb = jbbMaxs.maxPTmass;
    bH0 = jetFoxWolfMom.h0;
    bH1 = jetFoxWolfMom.h1;
    bH2 = jetFoxWolfMom.h2;
    bH3 = jetFoxWolfMom.h3;
    bH4 = jetFoxWolfMom.h4;
    bbH0 = bjetFoxWolfMom.h0;
    bbH1 = bjetFoxWolfMom.h1;
    bbH2 = bjetFoxWolfMom.h2;
    bbH3 = bjetFoxWolfMom.h3;
    bbH4 = bjetFoxWolfMom.h4;
    bR1 = jetFoxWolfMom.r1;
    bR2 = jetFoxWolfMom.r2;
    bR3 = jetFoxWolfMom.r3;
    bR4 = jetFoxWolfMom.r4;
    bbR1 = bjetFoxWolfMom.r1;
    bbR2 = bjetFoxWolfMom.r2;
    bbR3 = bjetFoxWolfMom.r3;
    bbR4 = bjetFoxWolfMom.r4;

    bjetHT = thisEvent->getSumSelJetScalarpT();  
    bbjetHT = thisEvent->getSumSelbJetScalarpT();
    blightjetHT = thisEvent->getSumSelLightJetScalarpT();
    bjetNumber = thisEvent->getnSelJet();
    bbjetNumber = thisEvent->getnbJet();
    blightjetNumber = thisEvent->getnLightJet();
    binvMassZ1 = _bbMassMin1Z; //ZZ
    binvMassZ2 = _bbMassMin2Z;
    bchi2Z = _minChi2Z;
    binvMassH1 = _bbMassMin1Higgs; //HH
    binvMassH2 = _bbMassMin2Higgs;
    bchi2Higgs = _minChi2Higgs;
    bchi2HiggsZ = _minChi2HiggsZ; //ZH
    binvMassHiggsZ1 = _bbMassMin1HiggsZ;
    binvMassHiggsZ2 = _bbMassMin2HiggsZ;
    bPTH1 = _bpTHiggs1;
    bPTH2 = _bpTHiggs2;


    bcentralityjb = jbjetCent.centrality; 
    bcentralityjl = jlepCent.centrality; 
    baplanarity = thisEvent->eventShapeJet->getAplanarity();
    bsphericity = thisEvent->eventShapeJet->getSphericity();
    btransSphericity = thisEvent->eventShapeJet->getTransSphericity();
    bcValue = thisEvent->eventShapeJet->getC();
    bdValue = thisEvent->eventShapeJet->getD();
    bbaplanarity = thisEvent->eventShapeBjet->getAplanarity();
    bbsphericity = thisEvent->eventShapeBjet->getSphericity();
    bbtransSphericity = thisEvent->eventShapeBjet->getTransSphericity();
    bbcValue = thisEvent->eventShapeBjet->getC();
    bbdValue = thisEvent->eventShapeBjet->getD();
    ////passHadTrig = thisEvent->getHadTriggerAccept();



    if(thisEvent->getnSelMuon() > 0 && thisEvent->getSelMuons()->at(0)->getp4()->Pt() > cut["leadMuonPt"]){
	bmuonPT1 = thisEvent->getSelMuons()->at(0)->getp4()->Pt();
	bmuonEta1 = thisEvent->getSelMuons()->at(0)->getp4()->Eta();
    } else {
	bmuonPT1 = -6;
	bmuonEta1 = -6;
    }



    if(thisEvent->getnSelElectron() > 0 && thisEvent->getSelElectrons()->at(0)->getp4()->Pt() > cut["leadElePt"]){
	belePT1 = thisEvent->getSelElectrons()->at(0)->getp4()->Pt();
	beleEta1 = thisEvent->getSelElectrons()->at(0)->getp4()->Eta();
    } else {
	belePT1 = -6;
	beleEta1 = -6;
    }
    bleptonPT1 = thisEvent->getSelLeptons()->at(0)->getp4()->Pt();
   // bleptonPT2 = thisEvent->getSelLeptons()->at(1)->getp4()->Pt();
    bleptonEta1 = thisEvent->getSelLeptons()->at(0)->getp4()->Eta();
 //   bleptonEta2 = thisEvent->getSelLeptons()->at(1)->getp4()->Eta();
    bleptonCharge1 = thisEvent->getSelLeptons()->at(0)->charge;
//    bleptonCharge2 = thisEvent->getSelLeptons()->at(1)->charge;
    bleptonHT = thisEvent->getSelLeptonHT();
    bST = thisEvent->getSelLeptonST();



/*    if(thisEvent->getnSelMuon() > 1){
	bmuonPT2 = thisEvent->getSelMuons()->at(1)->getp4()->Pt();
	bmuonEta2 = thisEvent->getSelMuons()->at(1)->getp4()->Eta();
	bdiMuonMass = thisEvent->getSelMuonsMass();
    } else {
	bmuonPT2 = -6;
	bmuonEta2 = -6;
	bdiMuonMass = -6;
    } */

														  
/*
    if(thisEvent->getnSelElectron() > 1){
	belePT2 = thisEvent->getSelElectrons()->at(1)->getp4()->Pt();
	beleEta2 = thisEvent->getSelElectrons()->at(1)->getp4()->Eta();
	bdiElectronMass = thisEvent->getSelElectronsMass();
    } else {
	belePT2 = -6;
	beleEta2 = -6;
	bdiElectronMass = -6;
	} */

    
    _inputTree->Fill();
}

void ttHHanalyzer::writeTree(){
    _of->file->cd();
    _treeDirs.at(0)->cd();
    _inputTree->Write();
    //    _inputTree->Delete();
    
}

//----------------------------------------------------------------------------
int main(int argc, char** argv){
    commandLine cl(argc, argv);
    vector<string> filenames = fileNames(cl.filelist);
    double weight = cl.externalweight;   // Get global weight 
 
    // Create tree reader
    itreestream stream(filenames, "Events");
    if ( !stream.good() ) error("can't read root input files");

    eventBuffer ev(stream);
    std::cout << " Output filename: " << cl.outputfilename << std::endl;
    ////ttHHanalyzer analysis(cl.outputfilename, &ev, weight, true)
  
    // If you want to check or modify arguments,
    // Please check the [ src/tnm.cc ]
    // Arguments structure --> filelist, outputDirName, weight, Year, Data or MC, sampleName
    ttHHanalyzer analysis(cl.outputfilename, &ev, weight, true, cl.runYear, cl.isData, cl.sampleName);
    analysis.performAnalysis();

    ev.close();
    //    of.close();
    return 0;
}
