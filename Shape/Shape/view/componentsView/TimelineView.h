#pragma once
#include "../../model/Picture.h"
#include "../../editor/EditorState.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <string>

// Separate document view, same selection state as Canvas. Scrubbing is
// window state; vertical layer moves use the editor's ReorderShapeCommand.
class TimelineView
{
public:
    bool Contains(sf::Vector2f pt, const EditorState&) const
    {
        return pt.x >= 0 && pt.x < 800 && pt.y >= 470 && pt.y <= 600;
    }

    void UpdateTime(float x, EditorState& state) const
    {
        const double maxTime = 634.0 / (40.0 * state.timelineZoom);
        state.currentTime = std::clamp((double(x) - 165.0) /
            (40.0 * state.timelineZoom), 0.0, maxTime);
    }

    int ShapeRowAt(sf::Vector2f pt, const Picture& picture, const EditorState& state) const
    {
        if (!Contains(pt, state)) return -1;
        const int row = static_cast<int>(std::floor((pt.y - 496.f)/21.f));
        const int index = static_cast<int>(picture.GetShapes().size()) - 1 - row;
        return row >= 0 && row < 4 && index >= 0 && index < static_cast<int>(picture.GetShapes().size())
            ? index : -1;
    }

    std::string SelectAt(sf::Vector2f pt, const Picture& picture, EditorState& state) const
    {
        UpdateTime(pt.x, state);
        const int row = ShapeRowAt(pt, picture, state);
        if (row < 0) return {};
        const auto& id = picture.GetShapes()[row]->GetId();
        state.ClearSelection();
        state.AddToSelection(id);
        return id;
    }

    void Draw(sf::RenderWindow& window, const Picture& picture,
        const EditorState& state, const sf::Font& font) const
    {
        sf::RectangleShape panel({800.f,130.f});
        panel.setPosition({0.f,470.f});
        panel.setFillColor(sf::Color(236,239,244));
        window.draw(panel);
        sf::Text heading(font);
        heading.setCharacterSize(13);
        heading.setFillColor(sf::Color::Black);
        heading.setString("Timeline   t="+std::to_string(state.currentTime).substr(0,5));
        heading.setPosition({9.f,474.f});
        window.draw(heading);

        const auto shapes = picture.GetShapes();
        const int rows = std::min(4,static_cast<int>(shapes.size()));
        for (int row=0; row<rows; ++row)
        {
            const Shape* shape = shapes[shapes.size()-1-row];
            sf::RectangleShape item({796.f,20.f});
            item.setPosition({2.f,496.f+row*21.f});
            item.setFillColor(state.IsSelected(shape->GetId())
                ? sf::Color(170,196,246) : sf::Color(251,251,251));
            window.draw(item);
            sf::Text name(font);
            name.setCharacterSize(12);
            name.setFillColor(sf::Color::Black);
            name.setString(shape->GetTypeName()+"  "+shape->GetId());
            name.setPosition({6.f,498.f+row*21.f});
            window.draw(name);
        }
        // Time cursor and grid are display-only (not persisted in Drawing).
        for (int tick=0; tick<=15; ++tick)
        {
            const float x=165.f+tick*40.f*static_cast<float>(state.timelineZoom);
            if (x > 799.f) break;
            sf::RectangleShape mark({1.f,5.f});
            mark.setPosition({x,488.f});
            mark.setFillColor(sf::Color(90,100,118));
            window.draw(mark);
        }
        const float cursor=165.f+static_cast<float>(state.currentTime
            * 40.0 * state.timelineZoom);
        if (cursor >= 165.f && cursor < 799.f)
        {
            sf::RectangleShape line({2.f,111.f});
            line.setPosition({cursor,489.f});
            line.setFillColor(sf::Color(232,72,61));
            window.draw(line);
        }
    }
};
