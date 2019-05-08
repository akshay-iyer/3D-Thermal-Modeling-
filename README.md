# 3D-Thermal-Modeling-
3D thermal models are much more effective in thermal fault diagnosis than conventional 2D thermal images. However due to limited work in this domain, there is no standard pipeline of creating such a model using just a pair of images. In this work, we explore a variety of techniques and propose a pipeline of creating a model from a pair of images. It gives satisfactory results even with limited calibration accuracy due to hardware constraints. For the same we create a stereo rig using Orbbec Astra depth camera and a FLIR E40 thermal camera in order to create a 3D thermal model of the indoor environment. We explore the intrinsic and the cross calibration of the trimodal setup in depth and present the challenges and solutions one would face in such a calibration process. We then explore multiple approaches for creation of a 3D thermal model. We find that the method of fusing thermal and depth pointclouds from pairs of corresponding images is the method which is successful in creation of a 3D thermal model.

## Problem Statement
The primary motive of this project is twofold: 

1. Explore in depth, the process of cross calibration of a trimodal setup of a combination of RGB, depth and thermal camera 

2. Explore the feasibility of several different approaches to establish a pipeline for constructing a 3D thermal model of indoor environment by utilizing the trimodal setup. The RGBD camera that will be used is an ORBBEC Astra 3D camera while the thermal camera is a FLIR E40 Handheld system. There will be several different methods explored where the discussion of the most promising approach will be detailed with good analysis and results with respect to creation of a 3D thermal model

## Methodology
The methodology we followed is as follows: we create a stereo rig of a an RGBD and a thermal camera mentioned in the next section. We then perform the intrinsic calibration of the individual cameras of the stereo rig and perform the cross calibration between RGB and thermal and between the depth and the thermal cameras. Then we explore various approaches to create a 3D thermal model and conclude with our analysis.

## Camera Specifications
The  two  cameras  used  in  this  paper  are  the  Orbbec  Astra 3D  RGB  camera  and  the  FLIR  E40  Thermal  Imaging Camera. The Orbbec Astra was used to capture RGB-D data at 640 x 480 at 30fps and has a field of view of 60◦H x49.5◦V x 73◦D. The FLIR E40 is a handheld thermal camera that records data at 160 x 120 at 60fps and has a field of view of 25◦H x 19◦V

## Stereo Rig
To calibrate the cameras before they were ready to be used for capturing the image data, we needed a stereo setup of FLIR E40 thermal camera and the Astra RGBD camera. The cameras were mounted on a sheet made from a cardboard and a wooden base was created to stabilize the camera setup. To ensure that the later process of creating point clouds is robust, we ensured that the focal lengths of the cameras remain fixed i.e. the camera is not in auto-focus mode and that the lens cannot be easily moved. Also we ensured that there is no relative motion between the two cameras in the rig so that the extrinsic parameters between them remain constant. 

## Intrinsic Calibration - thermal camera
We used a thermal board with black rubber blocks that have different thermal conductivity than the metallic blocks. The following figure shows the image of the calibration sheet used. On heating the sheet with a thermal source (like a flood lamp or exposed to sun light), there is a distinct thermal gradient developed on the sheet which produces checkerboard patterns on the thermal sheet due to absorption of heat by the black strips. We then used the MATLAB camera calibration toolbox to perform the calibration. 

## Intrinsic Calibration - depth camera
We prepared a Hermann grid of dimensions of 24x36 inches as shown in the figure. We used a Full spectrum P-series laser cutter to cut square holes of 5 inch side. For intrinsic calibration of the same, we initially used the MATLAB Camera Calibration Toolbox but it does not have the feature of calibration using Hermann grid. It only works on calibration using the standard chessboard corners. We then tried using BoofCV which is an open source library written for computer vision and had the feature of square grid calibration.

## Cross Calibration
For the purpose of Stereo Calibration (Cross calibration of RGB-Thermal cameras), MATLAB Computer Vision Toolbox was used. We used the calibration method outlined by Zhang. The board was held in variety of positions and images were simultaneously taken from the two cameras. Thus we got around 25 image pairs from the stereo rig. The chessboard corners computed from the reference stereo images were then used to compute the camera parameters. The same was repeated using the Hermann board for the depth-thermal cross calibration.

## Creating a 3D thermal model using PointCloud Fusion
This approach involves fusing a depth point cloud and a thermal point cloud for every pair of image which generates a thermal model. Multiple thermal models can then be incrementally registered using Iterative Closest Point algorithm to form a global thermal map which has been left for future work. This process involves the following steps assuming that the depth and the thermal cameras are already intrinsically and cross calibrated:
1. Load a pair of thermal and depth image of the same scene 
2. Create a depth point cloud using the depth image and its intrinsic parameters
3. Register the depth image and the thermal image
4. Create a thermal point cloud using the registered thermal image and the intrinsic parameters of the thermal camera
5. Fuse thermal and depth point cloud to create a 3D thermal model

For a detailed explanation, kindly refer to https://drive.google.com/file/d/1nJGxJrqAVkLG7GgIozAzmcEhZ3U6ULhN/view?usp=sharing

## Sample Results
The follwoing image is the screenshot of the 3D pointcloud created. PCL viewer can be used to view the 3D model. As we can see, that the created 3D thermal model from the dataset contains clearly discernible features. We can identify hot objects like the jug and the ventilator at the wall. As also once can clearly identify the people as high temperature entities. One is able to discern the relatively cold chair, tables and window frame despite it not being very clear in the depth image.

## Useful Tools and datasets for thermal modeling 
We have explored a variety of tools for camera calibration as our setup is a multi-modal setup and requires different types of calibration process. They have been specified below alongwith their significance in the project so it can help the readers interested in working in this domain:
### MATLAB Camera Calibrator
MATLAB camera calibrator is an app that is part of the Matlab Computer Vision Toolbox. It has the options for both mono camera calibration and stereo camera calibration. This is the most simple and easy to use application for both mono/stereo camera calibrations. 
### Thermalvis
Thermalvis is a opensource software written by Stephen Vidas. It is written in c++ and has many prebuilt applications related to thermal vision. One of the applications we were interested in our project was the mmcamera calibrator app that has multi-modal camera calibration. 
### BoofCV 
BoofCV is a opensource library similar to opencv but for real-time computer vision and is based on java virtual machine. The library is written in java but there is also support for python. This library is used to calibrate the thermal camera and the depth camera using the hermann grid. 
### Astra SDK and OpenNI
The AstraSDK and Openni for linux both create the driver support to use the Astra Camera in linux through USB connectivity. Besides the SDK provided by Astra, there is also a ROS package available \textit{(ros\_astra)} which can be directly used if ROS is pre-installed in the computer.  
###  AAU VAP Trimodal People segmentation dataset 
https://www.kaggle.com/aalborguniversity/trimodal-people-segmentation
We used this dataset which provides 3 different scenes of a trimodal video capture recorded in visible, depth and thermal cameras. This dataset also comes with the calibration parameters of the cameras used in their recording.

## Code Usage:
To run the following code, ensure you have PointCloudLibrary (http://pointclouds.org/) installed on your machine.
1. Clone the repo
2. Open a new terminal
3. Type the following:

```console
mkdir build
cd build
cmake ..
make
./model
```
This will create a 3D pointcloud file 00000.pcd for the first pair of images.

To view pointcloud file, go to the folder where it is saved and open a terminal there and run:
```console
pcl_viewer 00000.pcd
```
