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



