#pragma once
#include "AppFramework/tree_node.h"

class file_node : public tree_node
{
public:
    explicit file_node(const std::filesystem::path& in_path);
    
    [[nodiscard]] sf::Color get_fill_color() const override;
    ~file_node() override;
    counters get_and_cache_counters() override;
};
