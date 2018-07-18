//============================================================================
// Name        : vsumm-cpu.cpp
// Author      : Suellen Almeida / based on Sandra Avila code
// Version     :
// Copyright   : Your copyright notice
// Description : Video sumarization
//============================================================================

#include <iostream>
#include <vector>
#include <iomanip>

#include "FileOperations.h"
#include "VideoSegmentationCPU.h"
#include "HistogramCPU.h"
#include "ClusteringCPU.h"
#include "Results.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <string>
#include <chrono>

using namespace std;

#define BINS 16 //camibio original 16

int main(int argc, char **argv) {

	Results *result;
	result = Results::getInstance();
	result->setArch("cpu");

	cv::TickMeter timeGlobal, timeLocal;

	string video_path(argv[1]);
	vector<string> videoNames = FileOperations::listFiles(video_path, ".mp4");
	sort(videoNames.begin(), videoNames.end());

	// -------------------- TIME ---------------------------//
	auto start = std::chrono::high_resolution_clock::now();

	for(int i=0; i<(int)videoNames.size(); i++)
	{
		cout << "Processing video " << videoNames[i] << "... ";

		//-----------------------CREATE DIRECTORIES------------------------------//
		string frameDir = "frames-"+FileOperations::getSimpleName(videoNames[i])+"/";
		string summaryDir = "summary-"+FileOperations::getSimpleName(videoNames[i])+"/";

		if(FileOperations::createDir(frameDir) == 0)
		{
			FileOperations::deleteDir(frameDir);
			FileOperations::createDir(frameDir);
		}
		if(FileOperations::createDir(summaryDir) == 0)
		{
			vector<string> frame = FileOperations::listFiles(summaryDir, ".jpg");
			if(frame.size() > 0)
			{
				cout<<"Continua"<<endl;
				continue;
			}
		}
		//-----------------------------------------------------------------------//


		//-----------------------GET VIDEO INFORMATION---------------------------//

		result->setVideoName(FileOperations::getSimpleName(videoNames[i]));

		vector<string> splitStr = FileOperations::split(FileOperations::getSimpleName(videoNames[i]), '_');

		result->setLength(atoi(splitStr[0].c_str()));// nose xq ponia 1 aqui incluso proban con 4 videos no funcionaba
		if(atoi(splitStr[0].c_str()) == 30)
			result->setResolution(splitStr[0]);// aqui tambien ponia 2
		else
			result->setResolution(splitStr[0]); // y aqui XD
		//-----------------------------------------------------------------------//



		timeGlobal.reset(); timeGlobal.start();
		timeLocal.reset(); timeLocal.start();
		//------------------------VIDEO SEGMENTATION-----------------------------//



		VideoSegmentationCPU segmentation(videoNames[i]);
		segmentation.readSaveFrames(frameDir);
		//-----------------------------------------------------------------------//
		timeLocal.stop();
		result->setDecode(timeLocal.getTimeSec());
		cout<<"fff"<<endl;


		timeLocal.reset(); timeLocal.start();
		//------------------------FEATURE EXTRACTION-----------------------------//
		vector<string> frameNames = FileOperations::listFiles(frameDir, ".jpg");
		sort(frameNames.begin(), frameNames.end());

		vector<HistogramCPU> frameHistograms = FeaturesCPU::computeAllHist(frameNames);
		//-----------------------------------------------------------------------//
		timeLocal.stop();
		result->setFeatExtraction(timeLocal.getTimeSec());




		cout<<"uuuu"<<endl;
		timeLocal.reset(); timeLocal.start();
		//------------------------FRAMES CLUSTERING------------------------------//
		ClusteringCPU clust(frameHistograms);
		clust.kmeans();
		cout<<"Termino Kmeans"<<endl;
		//-----------------------------------------------------------------------//
		timeLocal.stop();
		result->setClustering(timeLocal.getTimeSec());



		timeLocal.reset(); timeLocal.start();
		//------------------------KEYFRAME EXTRACTION----------------------------//
		clust.findKeyframes();
		cout<<"0000000"<<endl;
		//-----------------------------------------------------------------------//
		timeLocal.stop();
		result->setKeyframeExtraction(timeLocal.getTimeSec());



		timeLocal.reset(); timeLocal.start();
		cout<<"8888888"<<endl;
		//-------------------ELIMINATION OF SIMILAR KEYFRAMES--------------------//
		clust.removeSimilarKeyframes();//funcion con problemas
		cout<<"----12"<<endl;
		//-----------------------------------------------------------------------//
		timeLocal.stop();
		result->setEliminateSimilar(timeLocal.getTimeSec());
        cout<<"1111"<<endl;



		//---------------------GENERATE SUMMARY----------------------------------//
		vector<HistogramCPU> finalKeyframes = clust.getKeyframes();
		cout<<"tam : "<<finalKeyframes.size()<<endl;

		for(int j=0; j<(int)finalKeyframes.size(); j++)
		{
			if(finalKeyframes[j].getIdFrame() >= frameNames.size())
				continue;
			stringstream out;
			out.fill('0');
			out << std::right << std::setw(6) << finalKeyframes[j].getIdFrame();
			string name = frameDir+"/frame-"+out.str()+".jpg";

			FileOperations::copyFile(name, summaryDir);
		}
		// FileOperations::deleteDir(frameDir);
		//-----------------------------------------------------------------------//


		timeGlobal.stop();
		result->setTotal(timeGlobal.getTimeSec());
		result->print();
		cout <<"Elapsed time: "<< timeGlobal.getTimeSec()<<"segundos" << endl;
		result->save();
		cout << endl;

	}

	return 0;
}
