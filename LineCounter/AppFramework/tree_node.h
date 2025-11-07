#pragma once
#include "common.h"
#include "SFML/Graphics.hpp"

class tree_node
{
public:
  
  struct counters
  {
    int file,
    line,
    lines_of_code,
    comment_lines,
    characters = 0;
    bool is_valid = false;
    FORCEINLINE void invalidate() { is_valid = false; }
  };

  explicit tree_node(const std::filesystem::path& in_path){}
  virtual ~tree_node() = default;
  _NODISCARD virtual sf::Color get_fill_color() const = 0;
  virtual counters get_and_cache_counters() = 0;

  counters cached_counters{};
};
