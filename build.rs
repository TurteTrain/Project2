fn main() {
    cxx_build::bridge("src/data/file_data.rs") // returns a cc::Build
        .file("cpp/link_data.cpp")
        .std("c++20")
        .compile("wiki-solve");

    println!("cargo:rerun-if-changed=cpp/link_data.cpp");
    println!("cargo:rerun-if-changed=cpp/link_data.hpp");
}
