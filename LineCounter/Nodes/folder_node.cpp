#include "folder_node.h"
#include "file_node.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

folder_node::folder_node(const fs::path& in_path)
    : tree_node(in_path)
{
    try
    {
        if (!fs::exists(in_path) || !fs::is_directory(in_path))
            return;

        for (const auto& entry : fs::directory_iterator(in_path))
        {
            const fs::path& childPath = entry.path();

            try
            {
                if (entry.is_directory())
                {
                    // Create subfolder node
                    folder_node* childFolder = new folder_node(childPath);
                    children.push_back(childFolder);
                }
                else if (entry.is_regular_file())
                {
                    // Create file node
                    file_node* childFile = new file_node(childPath);
                    children.push_back(childFile);
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error processing " << childPath << ": " << e.what() << '\n';
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error reading folder " << in_path << ": " << e.what() << '\n';
    }
}

folder_node::~folder_node()
{
    for (const tree_node* node : children)
        delete node;
    children.clear();
}
