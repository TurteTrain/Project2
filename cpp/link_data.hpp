#include "../target/cxxbridge/rust/cxx.h"
#include <array>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ranges>
#include <span>
#include <string_view>
#include <vector>

using std::unique_ptr;

struct __attribute__((__packed__)) uint24_t {
  std::array<uint8_t, 3> bytes;

  operator uint32_t() const;
};

namespace ffi {
struct LinkData;

rust::Box<LinkData> from_path(rust::Str data_path,
                              rust::Str offsets_path) noexcept;
rust::slice<std::array<uint8_t, 3> const> index(const LinkData *link_data,
                                                size_t index) noexcept;

void test();
}; // namespace ffi

using namespace ffi;

template <typename R, typename RetType>
concept forward_range_of = std::ranges::forward_range<R> &&
                           std::same_as<RetType, std::ranges::range_value_t<R>>;

class GraphParents {
  rust::box<LinkData> link_data;

  GraphParents() = delete; // Constructing the class with no files to open
                           // doesn't make sense.
  GraphParents(const GraphParents &other) = delete;
  GraphParents &operator=(const GraphParents &other) = delete;

public:
  GraphParents(rust::Str data_path, rust::Str offsets_path);
  std::span<uint24_t> operator[](size_t index);

  std::span<uint24_t> operator[](uint24_t index);

  template <forward_range_of<uint24_t> R>
  auto /*join_view<...> of uint24_t*/ operator[](R indices) {
    std::vector<std::span<uint24_t>> ranges;

    for (uint24_t id : indices) {
      ranges.push_back(operator[](id));
    }

    return std::ranges::join_view(ranges);
  }
};
