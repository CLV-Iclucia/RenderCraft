# for frame in range [0, 180)
# execute: ~/SimCraft/build/FluidSim/gpu-smoke -o ./scenes/media -e {frame}
# then execute: ./cmake-build-release/Core/rdcraft ~/frames/frame-{frame}.exr

for frame in (seq 180 360)
    ~/SimCraft/build/FluidSim/gpu-smoke -o ./scenes/media -e $frame
    ./cmake-build-release/Core/rdcraft ~/frames/frame-$frame.exr
end
