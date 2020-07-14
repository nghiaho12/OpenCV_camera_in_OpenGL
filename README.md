# An example of using an OpenCV calibrated camera in OpenGL

This example shows how to project a virtual cuboid onto a checkerboard pattern in OpenGL given the following
- calibrated camera intrinsics
- checkerboard extrinsics (rotation + translation)
- undistorted image

Data used in this example was from opencv/samples/data/left*.jpg. Calibration was done using opencv/samples/cpp/calibration.cpp. I've embedded the data into the code so OpenCV is not required.

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