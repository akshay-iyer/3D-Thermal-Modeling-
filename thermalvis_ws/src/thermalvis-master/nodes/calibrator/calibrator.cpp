#ifdef _BUILD_FOR_ROS_

#include "calibrator.hpp"

#define NODE_NAME "calibrator"

int main(int argc, char** argv) {
	
		
	ros::init(argc, argv, NODE_NAME);
	
	ros::NodeHandle private_node_handle("~");
	
	calibratorData startupData;
		
	bool inputIsValid = startupData.obtainStartingData(private_node_handle);
	
	startupData.read_addr = argv[0];
	ROS_INFO("<%s> Node launched from (%s)", NODE_NAME, startupData.read_addr.c_str());
	
	if (!inputIsValid) {
		ROS_ERROR("Configuration invalid.");
		return -1;
	}
		
	ROS_INFO("Startup data processed.");
	
	ros::NodeHandle nh;
	
	boost::shared_ptr < calibratorNode > calib_node (new calibratorNode (nh, startupData));
	
	globalNodePtr = &calib_node;
	
	signal(SIGINT, mySigintHandler);
	
	ROS_INFO("Node configured.");
	
	/*
	while (!::wantsToShutdown) {
		ros::spinOnce();
	}
	*/
	
    while (calib_node->isStillCollecting()) { ros::spinOnce(); }
	
	ROS_INFO("Patterns captured.");
	
    calib_node->getFrameTrackingTime();

    if (calib_node->wantsIntrinsics()) {
		
		ROS_INFO("Performing intrinsic calibration...");
		
        calib_node->preparePatternSubsets();
        calib_node->performIntrinsicCalibration();
		
		ROS_INFO("Intrinsic calibration completed.");

	} else {
        calib_node->assignIntrinsics();
	}
	
    if (calib_node->wantsExtrinsics()) {
		
		ROS_INFO("Performing extrinsic calibration...");

        calib_node->create_virtual_pairs();
        calib_node->prepareExtrinsicPatternSubsets();
        calib_node->performExtrinsicCalibration();
		
	}
	
    calib_node->writeResults();
	
    calib_node->set_ready_for_output();
	
    if (calib_node->wantsToUndistort() || (calib_node->wantsToRectify())) {
        calib_node->getAverageTime();
	}
	
    if (calib_node->wantsToUndistort()) {
		
        if (calib_node->wantsIntrinsics()) {
			ROS_INFO("Publishing undistorted images...");
		
            calib_node->startUndistortionPublishing();
			
            while (calib_node->wantsToUndistort()) {
				ros::spinOnce();
			}
			
			ROS_INFO("Publishing complete.");
		}
	}
	
    if (calib_node->wantsToRectify()) {
        if (calib_node->wantsExtrinsics()) {
			
			ROS_INFO("Publishing rectified images...");
		
            calib_node->startRectificationPublishing();
			
            while (calib_node->wantsToRectify()) {
				ros::spinOnce();
			}
			
			ROS_INFO("Publishing complete.");
			
		}
		
	}
	
	ROS_WARN("Operations complete. Node terminating.");
	
	return 0;
	
}

void mySigintHandler(int sig) {
	wantsToShutdown = true;
	ROS_WARN("Requested shutdown... terminating feeds...");
	
	(*globalNodePtr)->prepareForTermination();
}

bool calibratorData::obtainStartingData(ros::NodeHandle& nh) {
	
    numDetectors = 0;

    nh.param<string>("outputFolder", outputFolder, "outputFolder");
	
	nh.param<std::string>("topic", topic, "topic");
	
	if (topic != "topic") {
		ROS_INFO("<topic> (%s) selected.", topic.c_str());
	} else {
		topic = "/thermalvis/streamer/image_mono";
		ROS_WARN("No <topic> supplied; assuming (%s)", topic.c_str());
		//return false;
	}
	
	topicParent = topic.substr(0, topic.find_last_of("/"));
	
	ROS_INFO("topicParent = (%s)", topicParent.c_str());
	
	nh.param<string>("tracksOutputTopic", tracksOutputTopic, "tracksOutputTopic");
	
	if (tracksOutputTopic != "tracksOutputTopic") {
		ROS_INFO("<tracksOutputTopic> (%s) selected.", tracksOutputTopic.c_str());
	} else {
		ROS_WARN("No <tracksOutputTopic> supplied; will publish <tracks> at <%s>", topicParent.c_str());
		//return false;
	}
	
	//nh.param<int>("maxFeatures", maxFeatures, 100);
	
	nh.param<bool>("outputFeatureMotion", outputFeatureMotion, false);
	nh.param<bool>("normalizeFeatureVelocities", normalizeFeatureVelocities, true);

	
	//nh.param<bool>("debug_mode", debugMode, false);
	
	nh.param<bool>("outputTrackCount", outputTrackCount, false);
	
        //HGH/*
	
	for (int iii = 0; iii < MAX_DETECTORS; iii++) {
		
		char detector_tag[256], sensitivity_tag[256], method_tag[256], descriptor_tag[256];
		
		sprintf(detector_tag, "detector_%d", iii + 1);
		sprintf(sensitivity_tag, "sensitivity_%d", iii + 1);
		sprintf(method_tag, "method_%d", iii + 1);
		sprintf(descriptor_tag, "descriptor_%d", iii + 1);
		
		//printf("%s << detector_tag = %s\n", __FUNCTION__, detector_tag);
		
		
		if (iii == 0) {
			nh.param<std::string>(detector_tag, detector[iii], "GFTT");
		} else {
			nh.param<std::string>(detector_tag, detector[iii], "NONE");
		}
		
		//printf("%s << detector[%d] = %s\n", __FUNCTION__, iii, detector[iii].c_str());
		
		if ((detector[iii] == "SURF") || (detector[iii] == "FAST") || (detector[iii] == "GFTT") || (detector[iii] == "HARRIS") || (detector[iii] == "ORB") || (detector[iii] == "STAR")) {
			ROS_INFO("detector [%d]: %s", iii, detector[iii].c_str());
		} else if (detector[iii] == "NONE"){
			// printf("%s << No [%d] detector requested.\n", __FUNCTION__, iii);
			break;
		} else {
			ROS_INFO("ERROR! Detector (%s) not recognized.", detector[iii].c_str());
			return false;
		}
		
		nh.param<double>(sensitivity_tag, sensitivity[iii], 0.2);
		
		ROS_INFO("detector [%d] sensitivity: %f", iii, sensitivity[iii]);
		
		nh.param<std::string>(method_tag, method[iii], "match");
		
		if ((method[iii] == "match") || (method[iii] == "track")) {
			ROS_INFO("detector [%d] matching method: %s", iii, method[iii].c_str());
			if (method[iii] == "match") {
				method_match[iii] = true;
			} else {
				method_match[iii] = false;
			}
		} else {
			ROS_INFO("ERROR! detector [%d] matching method (%s) not recognized.", iii, method[iii].c_str());
			return false;
		}
		
		nh.param<std::string>(descriptor_tag, descriptor[iii], "BRIEF");
		
		if (method_match[iii]) {
			if ((descriptor[iii] == "SURF") || (descriptor[iii] == "ORB") || (descriptor[iii] == "BRIEF") || (descriptor[iii] == "SIFT")) {
				ROS_INFO("descriptor [%d]: %s", iii, descriptor[iii].c_str());
			} else {
				ROS_INFO("ERROR! descriptor [%d] (%s) not recognized.", iii, descriptor[iii].c_str());
				return false;
			}
		}
		
		numDetectors++;
	}
	
	
	
	ROS_INFO("[%d] detectors to be implemented.", numDetectors);
	
        //*/
	
	return true;
}

#endif
