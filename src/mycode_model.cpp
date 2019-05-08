// ================ it is the first combo which is right ================

#include <pcl/io/pcd_io.h>

//#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp> // imread

#include <boost/filesystem.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

using namespace pcl;
using namespace std;
using namespace cv;



typedef struct Intr
{
    int width;
    int height;
    float fx;
    float fy;
    float cx;
    float cy;
    float scale_factor;
} Intr;



// instrinsic camera matrix of depth camera
// old one 
// const Intr DEFAULT_CAM_PARAMS   =   {
//
//                                     640,
//                                     480,
//                                     489.1f,
//                                     488.5f,
//                                     334.9f,
//                                     231.6f,
//                                     (0.001f)
//                                 };

// second combo
// 0 = thermal; 1=depth
//const Intr DEFAULT_CAM_PARAMS   =   {
//
//                                    640,
//                                    480,
//                                    1487.3f,
//                                    1371.63f,
//                                    312.61f,
//                                    234.21f,
//                                    (0.001f)
//                                };

// ================ it is the first combo which is right ================

// // first combo
// //consider: 0=depth; 1=thermal
 const Intr DEFAULT_CAM_PARAMS   =   {

                                     640,
                                     480,
                                     1241.6f,
                                     1130.35f,
                                     345.26f,
                                     207.61f,
                                     (0.001f)
                                 };


const string DEFAULT_CFG_FILE   =   "cam_params.cfg";

bool
load_camera_intrinsics ( const string & cam_param_file_name, Intr & param )
{
    fstream cam_param; 
    cam_param.open ( cam_param_file_name.c_str(), std::ios::in );
    if ( !cam_param.is_open() ) {
       
        param = DEFAULT_CAM_PARAMS;
        return false;
    }
    else
        cam_param >> param.width >> param.height >> param.fx >> param.fy >> param.cx >> param.cy >> param.scale_factor;

    return true;
}

void
load_names_of_all_files_from_dir ( const char * dir_name, vector < string > & vector_with_names )
{
    const boost::filesystem::path base_dir ( dir_name );
    string extension (".png");
    
    for (boost::filesystem::directory_iterator it (base_dir); it != boost::filesystem::directory_iterator (); ++it)
    {      
        boost::filesystem::path p = it->path ();
        if ( boost::filesystem::is_regular_file ( it->status () ) && boost::filesystem::extension ( it->path () ) == extension )
            {//cout<<it->path().string()<<endl;
            vector_with_names.push_back ( it->path().string() );}

    }
}

template < class T >
void
set_pixel ( T & pcl_pixel, cv::Mat & src, int x, int y, Intr& cam_params )
{
    cerr << "set_pixel: Error - do not have proper specification for type: " << typeid(T).name() << endl;
    throw;
}

template <>
void
set_pixel ( RGB & pcl_color_pixel, cv::Mat & src, int x, int y, Intr& cam_params )
{
    uint32_t rgb;
    cv::Vec3b cur_rgb = src.at<cv::Vec3b>(y,x);// b,g,r
    rgb =   ( static_cast<int> ( cur_rgb [ 2 ] ) ) << 16 |
            ( static_cast<int> ( cur_rgb [ 1 ] ) ) << 8 |
            ( static_cast<int> ( cur_rgb [ 0 ] ) );
    
    pcl_color_pixel.rgba = static_cast < uint32_t > ( rgb );
}

template <>
void
set_pixel ( pcl::PointXYZ & xyz_pcl_pixel, cv::Mat & src, int x, int y, Intr& cam_params )
{
    //calc world z coord
    xyz_pcl_pixel.z = src.at<unsigned short>( y * cam_params.width + x ) * cam_params.scale_factor;
    //xyz_pcl_pixel.z = src.at<unsigned short>(y,x)* cam_params.scale_factor;
    // since x = fX/Z, X = Z*x/f; just its x-cx instead of x || similarly for y
    xyz_pcl_pixel.x = xyz_pcl_pixel.z * ( x - cam_params.cx ) / cam_params.fx;
    xyz_pcl_pixel.y = xyz_pcl_pixel.z * ( y - cam_params.cy ) / cam_params.fy;
}

template < class T>
bool
load_cloud ( const string & file_name, PointCloud<T> & pcl_cloud , Intr& cam_params )
{
    //cout<<"file name: "<<file_name.c_str()<<endl;
    //file_name = ""
     cv::Mat cur_mat =   cv::imread ( file_name.c_str(), -1 );
     // cv::imshow("a", cur_mat);
     // cv::waitKey(0);
	 cv::Size s      =   cur_mat.size();
	 int width       =   s.width;
	 int height      =   s.height;
     int nchannels   =   cur_mat.channels();
     int step        =   cur_mat.step;

     //width =640 , height = 480 for rgb and depth
     pcl_cloud.width   = width;
     pcl_cloud.height  = height;
     pcl_cloud.is_dense = true;
     pcl_cloud.points.resize ( width * height );

     for ( int y = 0; y < height; y++ )
        for ( int x = 0; x < width; x++ )
        {
            T   current_pixel;

            set_pixel <T> ( current_pixel, cur_mat, x, y, cam_params );
                  
            pcl_cloud (x, y ) = current_pixel;

        }

}


string new_rgb_path =  "//home/akshay/Documents/Books/ARN/datasets/vap/rgbdt-stereo/Scene1/new_rgb";

string register_images (Mat& color_image, Mat& depth_image,  pcl::PointCloud<pcl::PointXYZ>::Ptr depth_cloud_ptr, int& count){


// rotn between thermal and depth
float R_depth2color_arr [9] =


// rot will be same for both combo
{
    9.9980985906928577e-01, 1.8012299704400637e-02,
       7.4701249797968095e-03, -1.7704505435978152e-02,
       9.9906653390187838e-01, -3.9403189268817869e-02,
       -8.1728939257985998e-03, 3.9263442241424938e-02,
       9.9919546931930792e-01
   };

//old one - this one is rot between rgb and depth within kinect itself
// { 9.9984628826577793e-01, 1.2635359098409581e-03,-1.7487233004436643e-02,
//  -1.4779096108364480e-03, 9.9992385683542895e-01,-1.2251380107679535e-02,
//   1.7470421412464927e-02, 1.2275341476520762e-02, 9.9977202419716948e-01 };

Mat R_depth2color = Mat(3,3, CV_32F, R_depth2color_arr);
Mat rvec_depth2color;
Rodrigues(R_depth2color, rvec_depth2color); //R_depth2color is a 3x3 rotation matrix

//old one - between rgb and depth within kinect
//float t_depth2color_arr [3] = { 1.9985242312092553e-02, -7.4423738761617583e-04,-1.0916736334336222e-02};

// transl will be same for both combos
float t_depth2color_arr [9] = { -4.8890479500973520e-03, 4.8693501501806867e-02, 2.8147391520221020e-02};
Mat tvec_depth2color = Mat(1,3, CV_32F, t_depth2color_arr);


//old one
// float camera_mat_arr [9]= {4.8906692124429253e+002, 0.,                         3.3485218422573155e+002,
//                        0.,                      4.8848093316478855e+002,    2.3160073976994627e+002,
//                        0.,                      0.,                         1.};
//

// camera mat of thermal
// first combo
//from calibdata.yml
// float camera_mat_arr [9]= {1.4867282604016025e+03, 0., 3.1261186928169531e+02, 0.,
//        1.3716348151844629e+03, 2.3421540858396753e+02, 0., 0., 1.};
// Mat camera_mat = Mat(3,3, CV_32F, camera_mat_arr);
//
 float camera_mat_arr [9]= {1.2541762569013270e+003, 0., 3.1940523093512138e+002, 0.,
                            1.2369262330051283e+003, 2.3798718330767593e+002, 0., 0., 1.};
 Mat camera_mat = Mat(3,3, CV_32F, camera_mat_arr);

// second combo
//float camera_mat_arr [9]= {1.2416470380123314e+03, 0., 3.4526088422371339e+02, 0.,
//       1.1303508827451676e+03, 2.0761471425186008e+02, 0., 0., 1.};
//Mat camera_mat = Mat(3,3, CV_32F, camera_mat_arr);


//distortion of thermal
//old one 
//float distortion[5] = {1.5011536247461052e-001, -4.5887485657496629e-001,1.7153860191987405e-002, 3.1920401962601407e-003, 3.6254346234629548e-001};

// first combo
//from calibdata.yml
// float distortion[5] ={-3.9943018224099482e+00, 6.8261548735274474e+01,
//        2.9261719351995969e-02, 6.8077204932989787e-03,
//        -6.4058615426672668e+02};

    float distortion[5] ={-1.1149577425916057e+000, -5.1687917030033468e+000,
                          9.5693871380802455e-003, -5.0628328008900355e-003,
                          6.1926563729200794e+001};
 Mat distortioncoeff = Mat(1,5, CV_32F, distortion);

//second combo
//float distortion[5] ={4.3353334684082723e-01, 2.5481939218623442e+01,
//       8.7201851523402596e-02, 2.0883619235959907e-02,
//       -2.7488360624767233e+02};
//Mat distortioncoeff = Mat(1,5, CV_32F, distortion);



std::vector<cv::Point3d> src(1);
std::vector<cv::Point2d> dst;
cv::Mat color_image_aligned = cv::Mat::zeros(color_image.size(), color_image.type());

for (int i = 0; i < depth_image.rows; i++) {
  for (int j = 0; j < depth_image.cols; j++) {
    //cout<<"depth points\n";
    //cout<<(*depth_cloud_ptr)(j,i).z<<endl;
    //Check if the current point is valid (correct depth)
    if ((*depth_cloud_ptr)(j,i).z > 0) { //pointcloud is a pcl::PointCloud<pcl::PointXYZ>::Ptr in this example
      //XYZ 3D depth coordinate at the current [u,v] 2D depth image coordinate
      cv::Point3d xyz_depth( (*depth_cloud_ptr)(j,i).x, (*depth_cloud_ptr)(j,i).y, (*depth_cloud_ptr)(j,i).z );
      src[0] = xyz_depth;
      //cout<<src[0].x<<" "<<src[0].y<<" "<<src[0].z<<endl;

      //Transform the 3D depth coordinate to the color frame and project it in the color image plane
      projectPoints(src, rvec_depth2color, tvec_depth2color, camera_mat, distortioncoeff, dst);
      cv::Point2d uv_color = dst.front();
      //cout<<uv_color.x<<" "<<uv_color.y<<endl;      

      //Clamp pixel coordinate
      int u_color = std::max( std::min(color_image.cols-1, cvRound(uv_color.x)), 0 );
      int v_color = std::max( std::min(color_image.rows-1, cvRound(uv_color.y)), 0 );

      //Copy pixel
      color_image_aligned.at<cv::Vec3b>(i,j) = color_image.at<cv::Vec3b>(v_color, u_color);
      
      //cout<<"return: "<<file_name<<endl;
      
    }
  }
}
string file_name = new_rgb_path+ to_string(count) +"new.png";
count++;
imwrite(file_name, color_image_aligned);
return file_name;
}

int main ( int argc, char* argv[] )
{
  
    vector < string > depth_names;
    vector < string > rgb_names;
    vector < string > thermal_names;
    vector < string > pcd_file_names;
    int count = 0;
   
    char* depth_path = "/home/akshay/Documents/Books/ARN/datasets/vap/rgbdt-stereo/Scene1/depth_test";
    char* rgb_path = "/home/akshay/Documents/Books/ARN/datasets/vap/rgbdt-stereo/Scene1/rgb_test";
    char* thermal_path = "/home/akshay/Documents/Books/ARN/datasets/vap/rgbdt-stereo/Scene1/thermal_test";

    // loads all depth iamge names in the vector
    load_names_of_all_files_from_dir ( depth_path, depth_names );
    // loads all rgb image names in the vector
    load_names_of_all_files_from_dir ( rgb_path, rgb_names );
    // load names of all thermal iamegs in vector
    load_names_of_all_files_from_dir ( thermal_path, thermal_names );

    // ********remeber to change extension of thermal image to png
    //cout<<depth_names[0]<<endl<<"hello\n"<<thermal_names[0]<<endl;
    for(int i=0; i<depth_names.size(); i++){
        char tmp_str[128];
        memset ( tmp_str, 0, 128 );
        //sprintf is like printf but instead of terminal, it saves to string
        sprintf ( tmp_str, "/home/akshay/Documents/Books/ARN/datasets/vap/rgbdt-stereo/Scene1/latest/%05d.pcd", i);
        // creating pcd files with names 00000.pcd, 00001.pcd,...
        pcd_file_names.push_back(tmp_str);
    }

    //cout<<depth_names[0]<<endl<<thermal_names[0]<<endl<<pcd_file_names[0]<<endl;
    //cout<<depth_names.size()<<endl<<rgb_names.size()<<endl<<pcd_file_names.size()<<endl;
    // do we have data for continue ?
    if ( depth_names.empty() || depth_names.size() != thermal_names.size() ){
        cout<<"either folders are empty or of unequal sizes";
        exit(0);
    }

    // have to resize the thermal images to match the depth/rgb images
    //resize()
    Intr cam_params; 
    load_camera_intrinsics ( DEFAULT_CFG_FILE, cam_params );
    // Intr astra_cam_params;
    // Intr flir_cam_params; 
    // load_camera_intrinsics ( "astra_calib.yaml", astra_cam_params );
    // load_camera_intrinsics ( "flir_calib.yaml", flir_cam_params );

    
    for ( int i = 0; i < pcd_file_names.size(); i++)
    {
        cout<<depth_names[i]<<endl<<"hello\n"<<thermal_names[i]<<endl;
        // create a simple rgb cloud
        PointCloud < RGB >          current_color_cloud;
        PointCloud < PointXYZ >     current_xyz_cloud;
        PointCloud < PointXYZRGBA > current_xyzrgb_cloud;
    
        
        load_cloud < PointXYZ > ( depth_names[i], current_xyz_cloud, cam_params );

        //pcl::io::savePCDFileBinaryCompressed<pcl::PointXYZ> ( pcd_file_names[i], current_xyz_cloud );
        //cout<<"hello1\n";
        cout<<"depth cloud size: "<<current_xyz_cloud.size()<<endl;
        pcl::PointCloud<pcl::PointXYZ>::Ptr depth_cloud_ptr (new pcl::PointCloud<PointXYZ> (current_xyz_cloud));

        //cout<<"rchdd here\n";
        Mat color_image = imread(thermal_names[i]);
        //Mat color_image = imread(rgb_names[i]);   
        Mat depth_image = imread(depth_names[i]);
        //cout<<"rchf here\n";
        string rgb_file_name = register_images(color_image, depth_image, depth_cloud_ptr, count);
        //cout<<"registered\n";
        //load_cloud < RGB > ( rgb_file_name, current_color_cloud, cam_params );
        load_cloud < RGB > ( thermal_names[i], current_color_cloud, cam_params );
        //cout<<"hello "<<rgb_names[0]<<endl;
        //cout<<"color cloud size: "<<current_color_cloud.size()<<endl;


        copyPointCloud ( current_xyz_cloud, current_xyzrgb_cloud );
        for ( size_t ii = 0; ii < current_color_cloud.size (); ++ii )
            current_xyzrgb_cloud.points[ii].rgba = current_color_cloud.points[ii].rgba;
        cout<<"composite cloud size: "<<current_xyzrgb_cloud.size()<<endl; 
        pcl::io::savePCDFileBinaryCompressed<pcl::PointXYZRGBA> ( pcd_file_names[i], current_xyzrgb_cloud );
        cout<<"===========================================================\n";
    }
    return 0;
}




