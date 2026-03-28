#include "link_data.hpp"

struct uint24_t {
  std::array<uint8_t, 3> bytes;

  operator uint32_t() const {
    return (static_cast<uint32_t>(bytes[0])) |
           (static_cast<uint32_t>(bytes[1]) << 8) |
           (static_cast<uint32_t>(bytes[2]) << 16);
  }
};

void ffi::test() {
  std::cout << "hi" << std::endl;
  GraphParents graph_parents =
      GraphParents("db/articles.bin", "db/article_offsets.bin");

  for (uint32_t id : graph_parents[0]) {
    std::cout << "id: " << id << '\n';
  }
}

GraphParents::GraphParents(rust::Str data_path, rust::Str offsets_path)
    : link_data(from_path(data_path, offsets_path)) {}

std::span<uint24_t> GraphParents::operator[](size_t index) {
  rust::Slice<std::array<uint8_t, 3>> byte_array_slice =
      ffi::index(link_data.into_raw(), index);
  rust::Slice<uint24_t> &slice =
      reinterpret_cast<rust::Slice<uint24_t> &>(byte_array_slice);

  return std::span<uint24_t>(slice.data(), slice.size());
}
