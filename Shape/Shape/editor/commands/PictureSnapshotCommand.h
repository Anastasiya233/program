#pragma once
#include "../history/CommandHistory.h"
#include "../../model/Picture.h"
#include <memory>
#include <utility>

// One composite edit is one history entry. Snapshots preserve exact top-level
// order and nested children when grouping/ungrouping is undone or redone.
class PictureSnapshotCommand final : public ICommand
{
public:
    PictureSnapshotCommand(Picture& picture,
        std::unique_ptr<Picture> before, std::unique_ptr<Picture> after,
        std::vector<std::string> ids = {})
      : m_picture(picture), m_before(std::move(before)), m_after(std::move(after)), m_ids(std::move(ids)) {}

    void Execute() override { Restore(*m_after); }
    void Unexecute() override { Restore(*m_before); }
    std::vector<std::string> AffectedIds() const override { return m_ids; }
private:
    void Restore(const Picture& snapshot)
    {
        // Copy BEFORE clearing so a cloning failure does not destroy the model.
        auto safeCopy = snapshot.Clone();
        m_picture.Clear();
        for (const auto* s : safeCopy->GetShapes())
            m_picture.AddShape(s->Clone(s->GetId()));
    }
    Picture& m_picture;
    std::unique_ptr<Picture> m_before, m_after;
    std::vector<std::string> m_ids;
};
