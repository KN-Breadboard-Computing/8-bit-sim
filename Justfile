project_dir := justfile_directory()
build_dir := project_dir + "/build"

num_cpus := `nproc`

clean:
    rm -rf {{build_dir}}

configure build_type="Debug":
    mkdir -p {{build_dir}} && cd {{build_dir}} && cmake -DCMAKE_BUILD_TYPE={{build_type}} {{project_dir}}

build build_type="Debug": (configure build_type)
     cd {{build_dir}} && cmake --build . -j{{num_cpus}}

test: (build)
    cd {{build_dir}} && ctest -j{{num_cpus}}
