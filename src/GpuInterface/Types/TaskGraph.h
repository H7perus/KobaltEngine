#pragma once

#include "TaskNode.h"

#include <memory>
#include <vector>


namespace KE::VK
{

class TaskGraph
{
  public:
    TaskGraph()  = default;
    ~TaskGraph() = default;

    template <std::derived_from<ITaskNode> T> void AddTask(auto&&... args)
    {
        tasks_.emplace_back(std::make_unique<T>(std::forward<decltype(args)>(args)...));
    }

    void Compile()
    {
        //RESOLVE DEPENDENCIES
        for (auto& node : tasks_)
        {
            for (auto& entry : node->reads_)
            {
                if (namedResources_.contains(entry.first))
                {
                    entry.second = namedResources_[entry.first];
                }
            }
            for (auto& entry : node->writes_)
            {
                if (namedResources_.contains(entry.first))
                {
                    entry.second = namedResources_[entry.first];
                }
            }
        }

        //INSERT BARRIERS
        std::vector<std::string> resourcesWritten;
        for (int i = 0; i < tasks_.size(); i++)
        {
            std::unique_ptr<ITaskNode>& node = tasks_[i];
        }
    }

    void Execute(vk::CommandBuffer cmd)
    {
        for (auto& task : tasks_)
        {
            task->Execute(cmd);
        }
    };

    void AddNamedResource(std::string name, ResourceHandle resource)
    {
        if (!namedResources_.contains(name))
        {
            namedResources_.insert({name, resource});
        }
        else
        {
            throw std::logic_error(std::format("Resource with name \"{}\" already exists!", name));
        }
    }

  private:
    std::vector<std::unique_ptr<ITaskNode>> tasks_;

    std::map<std::string, ResourceHandle> namedResources_;
};

} // namespace KE::VK