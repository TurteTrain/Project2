#include "../target/cxxbridge/rust/cxx.h"
#include <array>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ranges>
#include <span>
#include <string_view>

using std::unique_ptr;

struct __attribute__((__packed__)) uint24_t;

namespace ffi {
struct LinkData;

rust::Box<LinkData> from_path(rust::Str data_path,
                              rust::Str offsets_path) noexcept;
rust::slice<std::array<uint8_t, 3>> index(LinkData *link_data, size_t index);

void test();
}; // namespace ffi

using namespace ffi;

class GraphParents {
  rust::box<LinkData> link_data;

  GraphParents() = delete; // Constructing the class with no files to open
                           // doesn't make sense.
  GraphParents(const GraphParents &other) = delete;
  GraphParents &operator=(const GraphParents &other) = delete;

public:
  GraphParents(rust::Str data_path, rust::Str offsets_path);
  std::span<uint24_t> operator[](size_t index);
};
