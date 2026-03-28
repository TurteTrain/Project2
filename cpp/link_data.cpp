#include "../cpp/searchTree.h"
#include "../cpp/timer.h"


uint24_t::operator uint32_t() const {
  return (static_cast<uint32_t>(bytes[0])) |
         (static_cast<uint32_t>(bytes[1]) << 8) |
         (static_cast<uint32_t>(bytes[2]) << 16);
}

void ffi::maincpp() {
    searchTree searcher = searchTree();
    bool run = true;
    std::string input;
    std::string input2;
    std::string input3;
    while (run) {
        std::cout << "0: Exit - 1: Search\n";
        std::cin >> input;
        if (std::stoi(input) == 0) {
            run = false;
        }
        else if (std::stoi(input) == 1){
            std::cout << "Input first ID:\n";
            std::cin >> input;
            std::cout << "Input second IDL \n";
            std::cin >> input2;
            std::cout << "0: Depth - 1: Breadth\n";
            std::cin >> input3;
            timer Timer;
            Timer.startTimer();
            std::vector<std::vector<int>> paths;
            if (std::stoi(input3) == 0) {
                paths = searcher.depth_search(std::stoi(input), std::stoi(input2));
            }
            else if (std::stoi(input3) == 1) {
                paths = searcher.reverse_list(searcher.breadth_search(std::stoi(input), std::stoi(input2)));
            }
            Timer.printTime();
            for (int i = 0; i < paths.size(); i++) {
                std::cout << "Path #" << i + 1 << ": " << std::endl;
                for (int num: paths.at(i)) {
                    std::cout << num << " ";
                }
                std::cout << "\n";
            }
        }
        std::cout << "Cycle complete \n\n";
    }
}

GraphParents::GraphParents(rust::Str data_path, rust::Str offsets_path)
    : link_data(from_path(data_path, offsets_path)) {}

std::span<uint24_t> GraphParents::operator[](size_t index) {
  rust::Slice<std::array<uint8_t, 3> const> byte_array_slice =
      ffi::index(link_data.into_raw(), index);
  rust::Slice<uint24_t> &slice =
      reinterpret_cast<rust::Slice<uint24_t> &>(byte_array_slice);

  return std::span<uint24_t>(slice.data(), slice.size());
}

std::span<uint24_t> GraphParents::operator[](uint24_t index) {
  return operator[](static_cast<size_t>(
      index)); // Cast happens implicitly, but defining for clarity
}
