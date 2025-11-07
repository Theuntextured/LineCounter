#pragma once
#include "AppFramework/tree_node.h"

class folder_node : public tree_node
{
public:
    explicit folder_node(const std::filesystem::path& in_path);
    ~folder_node() override;
    
    counters get_and_cache_counters() override;
    
    sf::Color get_fill_color() const override;

private:
    std::vector<tree_node*> children;
};
