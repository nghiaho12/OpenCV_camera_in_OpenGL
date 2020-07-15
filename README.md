# Example of using an OpenCV calibrated camera in OpenGL

![](https://github.com/nghiaho12/OpenCV_camera_in_OpenGL/blob/master/img.jpg)

This example shows how to project a virtual cuboid onto a checkerboard pattern in OpenGL given the following
- camera intrinsics (focal, center, skew)
- undistorted image
- checkerboard extrinsics/pose (rotation, translation)

Data used in this example was from opencv/samples/data/left*.jpg. Calibration was done using opencv/samples/cpp/calibration.cpp. I've embedded the data into the code so OpenCV is not required. This example is not exclusive to OpenCV, you can use any method to obtain the camera calibration and undistorted image.

## Build

You'll need the following libraries installed
- glfw3
- glm

On Ubuntu you can run (tested on 18.04)

```
sudo apt install libglfw3-dev
sudo apt install libglm-dev
```

To compile run
```
mkdir build
cd build
cmake ..
make
./main
```

Hit escape to quit.