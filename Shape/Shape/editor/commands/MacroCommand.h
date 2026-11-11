#pragma once
#include "../history/CommandHistory.h"
#include "../../model/Picture.h"
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

class MacroCommand final : public ICommand
{
public:
    explicit MacroCommand(std::vector<std::unique_ptr<ICommand>> commands)
        : m_commands(std::move(commands)) {}
    void Execute() override
    {
        for (auto& command : m_commands) command->Execute();
    }
    void Unexecute() override
    {
        for (auto it=m_commands.rbegin(); it!=m_commands.rend(); ++it)
            (*it)->Unexecute();
    }
    std::vector<std::string> AffectedIds() const override
    {
        std::vector<std::string> out;
        for (const auto& c : m_commands)
            for (const auto& id : c->AffectedIds())
                if (std::find(out.begin(),out.end(),id)==out.end()) out.push_back(id);
        return out;
    }
private:
    std::vector<std::unique_ptr<ICommand>> m_commands;
};

class MoveOneShapeCommand final : public ICommand
{
public:
    MoveOneShapeCommand(Picture& p, std::string id, double dx, double dy)
        : picture(p), id(std::move(id)), dx(dx), dy(dy) {}
    void Execute() override { if (auto* s=picture.GetShape(id)) s->Move(dx,dy); }
    void Unexecute() override { if (auto* s=picture.GetShape(id)) s->Move(-dx,-dy); }
    std::vector<std::string> AffectedIds() const override { return {id}; }
private:
    Picture& picture;
    std::string id;
    double dx,dy;
};

class ResizeOneShapeCommand final : public ICommand
{
public:
    ResizeOneShapeCommand(Picture& p, std::string id, Bounds before, Bounds after)
        : picture(p), id(std::move(id)), before(before), after(after) {}
    void Execute() override { if (auto* s=picture.GetShape(id)) s->SetBounds(after); }
    void Unexecute() override { if (auto* s=picture.GetShape(id)) s->SetBounds(before); }
    std::vector<std::string> AffectedIds() const override { return {id}; }
private:
    Picture& picture;
    std::string id;
    Bounds before,after;
};
