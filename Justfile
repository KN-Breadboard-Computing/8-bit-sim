BUILD_DIR := "build"
SIMULATOR_SRC_DIR := "simulator"
GPU_SRC_DIR := "src"
TEST_DIR := "tests"
EXEC_NAME := "simulator"

project_dir := justfile_directory()
build_dir := project_dir + "/" + BUILD_DIR

num_cpus := `nproc`

default: (run "Debug")

build BUILD_TYPE="Debug":
    git submodule update --init --recursive && \
    mkdir -p {{BUILD_DIR}} && cd {{BUILD_DIR}} && \
    cmake -DCMAKE_BUILD_TYPE={{BUILD_TYPE}} -DCMAKE_POLICY_VERSION_MINIMUM=3.5 .. && \
    cmake --build . --config {{BUILD_TYPE}} -j{{num_cpus}}

test *ARGS: (build)
    cd {{build_dir}} && ctest -j{{num_cpus}}

run BUILD_TYPE *ARGS: (build BUILD_TYPE)
    ./build/{{SIMULATOR_SRC_DIR}}/{{EXEC_NAME}} {{ARGS}}

clean:
    rm -rf {{build_dir}}
