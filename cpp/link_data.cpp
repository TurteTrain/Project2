#include "link_data.hpp"

uint24_t::operator uint32_t() const {
  return (static_cast<uint32_t>(bytes[0])) |
         (static_cast<uint32_t>(bytes[1]) << 8) |
         (static_cast<uint32_t>(bytes[2]) << 16);
}

void ffi::test() {
  GraphParents graph_parents =
      GraphParents("db/articles.bin", "db/article_offsets.bin");

  // The indexing functions all return ranges. The types get long, so use auto.

  auto test1 = graph_parents[34]; // We can input any uint24_t or any int type
                                  // of size >= 3 bytes safely
  auto test2 = graph_parents[test1]; // We can pass Article 34's parents in and
                                     // get all of their parents too
  auto test3 = graph_parents[test2]; // We can pass Articl34's parents' parents
                                     // and get all of their parents too!
  // We can do this indefinitely. You don't need to manually iterate through to
  // query for children and you don't need to copy any values

  // We can iterate over the results with a for-each loop
  //
  for (uint32_t id : test1) {
    std::cout << "id: " << id << '\n';
  }

  for (uint32_t id : test2) {
    std::cout << "id: " << id << '\n';
  }

  for (uint32_t id : test3) {
    std::cout << "id: " << id << '\n';
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
