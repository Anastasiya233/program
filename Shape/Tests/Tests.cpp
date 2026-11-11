#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <boost/signals2.hpp>

#include "../Shape/editor/EditorController.cpp"
#include "../Shape/editor/EditorController.h"
#include "../Shape/editor/EditorState.h"
#include "../Shape/editor/Logger.h"
#include "../Shape/editor/commands/AddShapeCommand.cpp"
#include "../Shape/editor/commands/AddShapeCommand.h"
#include "../Shape/editor/commands/ChangeColorCommand.cpp"
#include "../Shape/editor/commands/ChangeColorCommand.h"
#include "../Shape/editor/commands/DeleteShapesCommand.cpp"
#include "../Shape/editor/commands/DeleteShapesCommand.h"
#include "../Shape/editor/commands/MoveShapesCommand.cpp"
#include "../Shape/editor/commands/MoveShapesCommand.h"
#include "../Shape/editor/commands/ReorderShapeCommand.cpp"
#include "../Shape/editor/commands/ReorderShapeCommand.h"
#include "../Shape/editor/commands/ResizeShapesCommand.cpp"
#include "../Shape/editor/commands/ResizeShapesCommand.h"
#include "../Shape/editor/history/CommandHistory.cpp"
#include "../Shape/editor/history/CommandHistory.h"
#include "../Shape/editor/serializer/DocumentSerializer.cpp"
#include "../Shape/editor/serializer/DocumentSerializer.h"
#include "../Shape/model/Color.cpp"
#include "../Shape/model/Color.h"
#include "../Shape/model/Picture.cpp"
#include "../Shape/model/Picture.h"
#include "../Shape/model/Shape.cpp"
#include "../Shape/model/Shape.h"
#include "../Shape/model/observer/ConsolePictureObserver.cpp"
#include "../Shape/model/observer/ConsolePictureObserver.h"
#include "../Shape/model/shapes/EllipseStrategy.cpp"
#include "../Shape/model/shapes/EllipseStrategy.h"
#include "../Shape/model/shapes/PentagonStrategy.cpp"
#include "../Shape/model/shapes/PentagonStrategy.h"
#include "../Shape/model/shapes/RectangleStrategy.cpp"
#include "../Shape/model/shapes/RectangleStrategy.h"
#include "../Shape/model/shapes/ShapeStrategyFactory.cpp"
#include "../Shape/model/shapes/ShapeStrategyFactory.h"
#include "../Shape/model/shapes/StarStrategy.cpp"
#include "../Shape/model/shapes/StarStrategy.h"
#include "../Shape/model/shapes/TriangleStrategy.cpp"
#include "../Shape/model/shapes/TriangleStrategy.h"
#include "../Shape/visitor/IShapeVisitor.h"
#include "../Shape/visitor/SaveVisitor.cpp"
#include "../Shape/visitor/SaveVisitor.h"
#include <sstream>
#include <string>

TEST_CASE("AddShapeCommand: Undo removes, Redo restores")
{
	Picture picture;
	CommandHistory history;

	auto shape = std::make_unique<Shape>("r1", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(10, 10, 100, 50));

	history.Execute(std::make_unique<AddShapeCommand>(picture, std::move(shape), 0));
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("r1") != nullptr);

	history.Undo();
	CHECK(picture.GetShapes().empty());

	history.Redo();
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("r1") != nullptr);
}

TEST_CASE("DeleteShapesCommand: Undo restores z-order")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));
	picture.AddShape(std::make_unique<Shape>("b", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));
	picture.AddShape(std::make_unique<Shape>("c", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));

	history.Execute(std::make_unique<DeleteShapesCommand>(picture, std::vector<std::string>{ "b" }));
	CHECK(picture.GetShapes().size() == 2);
	CHECK(picture.GetShape("b") == nullptr);

	history.Undo();
	CHECK(picture.GetShapes().size() == 3);
	CHECK(picture.GetShapeIndex("a") == 0);
	CHECK(picture.GetShapeIndex("b") == 1);
	CHECK(picture.GetShapeIndex("c") == 2);
}

TEST_CASE("DeleteShapesCommand: multi-delete one Undo restores all")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));
	picture.AddShape(std::make_unique<Shape>("b", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));
	picture.AddShape(std::make_unique<Shape>("c", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)));

	history.Execute(std::make_unique<DeleteShapesCommand>(
		picture, std::vector<std::string>{ "a", "c" }));
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("b") != nullptr);

	history.Undo();
	CHECK(picture.GetShapes().size() == 3);
	CHECK(picture.GetShapeIndex("a") == 0);
	CHECK(picture.GetShapeIndex("b") == 1);
	CHECK(picture.GetShapeIndex("c") == 2);
}

TEST_CASE("MoveShapesCommand: Undo/Redo restores position")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(100, 100, 50, 50)));

	picture.GetShape("a")->Move(30, 20);
	history.PushExecuted(std::make_unique<MoveShapesCommand>(
		picture, std::vector<std::string>{ "a" }, 30, 20));

	CHECK(picture.GetShape("a")->GetBounds().left == 130.0);
	CHECK(picture.GetShape("a")->GetBounds().top == 120.0);

	history.Undo();
	CHECK(picture.GetShape("a")->GetBounds().left == 100.0);
	CHECK(picture.GetShape("a")->GetBounds().top == 100.0);

	history.Redo();
	CHECK(picture.GetShape("a")->GetBounds().left == 130.0);
	CHECK(picture.GetShape("a")->GetBounds().top == 120.0);
}

TEST_CASE("ResizeShapesCommand: Undo/Redo restores bounds")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(100, 100, 50, 50)));

	Bounds before = picture.GetShape("a")->GetBounds();
	Bounds after{ 100, 100, 200, 150 };

	picture.GetShape("a")->SetBounds(after);
	history.PushExecuted(std::make_unique<ResizeShapesCommand>(
		picture, std::vector<ResizeShapesCommand::Change>{ { "a", before, after } }));

	CHECK(picture.GetShape("a")->GetBounds().width == 200.0);

	history.Undo();
	CHECK(picture.GetShape("a")->GetBounds().width == 50.0);

	history.Redo();
	CHECK(picture.GetShape("a")->GetBounds().width == 200.0);
}

TEST_CASE("New command after Undo kills Redo")
{
	Picture picture;
	CommandHistory history;

	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 0));
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("b", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 1));

	CHECK(picture.GetShapes().size() == 2);

	history.Undo();
	CHECK(picture.GetShapes().size() == 1);
	CHECK(history.CanRedo());

	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("c", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 1));

	REQUIRE_FALSE(history.CanRedo());

	history.Redo();
	CHECK(picture.GetShapes().size() == 2);
	CHECK(picture.GetShape("c") != nullptr);
	CHECK(picture.GetShape("b") == nullptr);
}

TEST_CASE("Undo at start / Redo at end are no-op")
{
	Picture picture;
	CommandHistory history;

	REQUIRE_FALSE(history.CanUndo());
	REQUIRE_FALSE(history.CanRedo());

	history.Undo();
	history.Redo();

	CHECK(picture.GetShapes().empty());
}

TEST_CASE("Delete -> Undo -> Undo Move uses id, not address")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(100, 100, 50, 50)));

	picture.GetShape("a")->Move(20, 0);
	history.PushExecuted(std::make_unique<MoveShapesCommand>(
		picture, std::vector<std::string>{ "a" }, 20, 0));

	history.Execute(std::make_unique<DeleteShapesCommand>(
		picture, std::vector<std::string>{ "a" }));
	CHECK(picture.GetShape("a") == nullptr);

	history.Undo();
	CHECK(picture.GetShape("a") != nullptr);

	history.Undo();
	CHECK(picture.GetShape("a")->GetBounds().left == 100.0);
}

TEST_CASE("Undo x N restores state step by step")
{
	Picture picture;
	CommandHistory history;

	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 0));
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("b", Color(0, 255, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 1));
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("c", Color(0, 0, 255), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 2));

	CHECK(picture.GetShapes().size() == 3);

	history.Undo();
	CHECK(picture.GetShapes().size() == 2);
	CHECK(picture.GetShape("a") != nullptr);
	CHECK(picture.GetShape("b") != nullptr);
	CHECK(picture.GetShape("c") == nullptr);

	history.Undo();
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("b") == nullptr);

	history.Undo();
	CHECK(picture.GetShapes().empty());

	history.Undo();
	CHECK(picture.GetShapes().empty());
	REQUIRE_FALSE(history.CanUndo());
}

TEST_CASE("Redo x N restores state step by step")
{
	Picture picture;
	CommandHistory history;

	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 0));
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("b", Color(0, 255, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 1));
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("c", Color(0, 0, 255), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 2));

	history.Undo();
	history.Undo();
	history.Undo();
	CHECK(picture.GetShapes().empty());

	history.Redo();
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("a") != nullptr);

	history.Redo();
	CHECK(picture.GetShapes().size() == 2);

	history.Redo();
	CHECK(picture.GetShapes().size() == 3);
	CHECK(picture.GetShape("c") != nullptr);

	history.Redo();
	CHECK(picture.GetShapes().size() == 3);
	REQUIRE_FALSE(history.CanRedo());
}

TEST_CASE("Load clears history")
{
	Picture source;
	source.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(10, 20, 30, 40)));
	DocumentSerializer::Save(source, "test_load.tmp");

	Picture picture;
	CommandHistory history;
	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("temp", Color(0, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10)), 0));
	CHECK(history.CanUndo());

	auto loaded = DocumentSerializer::Load("test_load.tmp");
	picture.Clear();
	for (const auto* s : loaded->GetShapes())
	{
		picture.AddShape(s->Clone(s->GetId()));
	}
	history.Clear();

	REQUIRE_FALSE(history.CanUndo());
	REQUIRE_FALSE(history.CanRedo());
	CHECK(picture.GetShapes().size() == 1);
	CHECK(picture.GetShape("a") != nullptr);

	std::remove("test_load.tmp");
}

TEST_CASE("Save does not affect history")
{
	Picture picture;
	CommandHistory history;

	history.Execute(std::make_unique<AddShapeCommand>(
		picture, std::make_unique<Shape>("a", Color(255, 0, 0), std::make_unique<RectangleStrategy>(10, 20, 30, 40)), 0));
	CHECK(history.CanUndo());

	DocumentSerializer::Save(picture, "test_save.tmp");

	CHECK(history.CanUndo());
	REQUIRE_FALSE(history.CanRedo());

	history.Undo();
	CHECK(picture.GetShapes().empty());

	std::remove("test_save.tmp");
}

TEST_CASE("Empty drag creates no history entry")
{
	Picture picture;
	EditorState state;
	CommandHistory history;
	EditorController controller(picture, state, history);

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(100, 100, 50, 50)));

	state.SelectedIds = { "a" };
	state.drag.isDrag = true;
	state.drag.startX = 120;
	state.drag.startY = 120;

	controller.OnMouseMoved(120, 120);
	controller.OnMouseReleased(120, 120);

	REQUIRE_FALSE(history.CanUndo());
}

TEST_CASE("Save/Load round-trip preserves shapes")
{
	Picture picture;
	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(10, 20, 30, 40)));
	picture.AddShape(std::make_unique<Shape>("b", Color(0, 0, 255),
		std::make_unique<EllipseStrategy>(100, 100, 50, 50)));
	picture.AddShape(std::make_unique<Shape>("c", Color(0, 255, 0),
		std::make_unique<TriangleStrategy>(0, 100, 100, 100, 50, 0)));

	DocumentSerializer::Save(picture, "test_roundtrip.tmp");
	auto loaded = DocumentSerializer::Load("test_roundtrip.tmp");

	CHECK(loaded->GetShapes().size() == 3);

	CHECK(loaded->GetShapeIndex("a") == 0);
	CHECK(loaded->GetShapeIndex("b") == 1);
	CHECK(loaded->GetShapeIndex("c") == 2);

	CHECK(loaded->GetShape("a") != nullptr);
	CHECK(loaded->GetShape("a")->GetTypeName() == "rectangle");
	CHECK(loaded->GetShape("b")->GetTypeName() == "circle");
	CHECK(loaded->GetShape("c")->GetTypeName() == "triangle");

	CHECK(loaded->GetShape("a")->GetColor().GetHexStr() == "#ff0000");

	Bounds b = loaded->GetShape("a")->GetBounds();
	CHECK(b.left == 10.0);
	CHECK(b.top == 20.0);
	CHECK(b.width == 30.0);
	CHECK(b.height == 40.0);

	std::remove("test_roundtrip.tmp");
}

TEST_CASE("Visitor dispatches to correct strategy")
{
	struct CountingVisitor : IShapeVisitor
	{
		int rect = 0, ellipse = 0, triangle = 0, pentagon = 0, star = 0;

		void Visit(const Shape&, const RectangleStrategy&) override { ++rect; }
		void Visit(const Shape&, const EllipseStrategy&) override { ++ellipse; }
		void Visit(const Shape&, const TriangleStrategy&) override { ++triangle; }
		void Visit(const Shape&, const PentagonStrategy&) override { ++pentagon; }
		void Visit(const Shape&, const StarStrategy&) override { ++star; }
	};

	Shape r("r", Color(0, 0, 0), std::make_unique<RectangleStrategy>(0, 0, 10, 10));
	Shape e("e", Color(0, 0, 0), std::make_unique<EllipseStrategy>(0, 0, 10, 10));
	Shape t("t", Color(0, 0, 0), std::make_unique<TriangleStrategy>(0, 0, 10, 0, 0, 10));
	Shape p("p", Color(0, 0, 0), std::make_unique<PentagonStrategy>(0, 0, 10, 10));
	Shape s("s", Color(0, 0, 0), std::make_unique<StarStrategy>(0, 0, 10, 10));

	CountingVisitor v;
	r.Accept(v);
	e.Accept(v);
	t.Accept(v);
	p.Accept(v);
	s.Accept(v);

	CHECK(v.rect == 1);
	CHECK(v.ellipse == 1);
	CHECK(v.triangle == 1);
	CHECK(v.pentagon == 1);
	CHECK(v.star == 1);
}

TEST_CASE("Save/Load round-trip preserves 5 shape types")
{
	Picture picture;
	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(10, 20, 30, 40)));
	picture.AddShape(std::make_unique<Shape>("b", Color(0, 0, 255),
		std::make_unique<EllipseStrategy>(100, 100, 50, 50)));
	picture.AddShape(std::make_unique<Shape>("c", Color(0, 255, 0),
		std::make_unique<TriangleStrategy>(0, 100, 100, 100, 50, 0)));
	picture.AddShape(std::make_unique<Shape>("d", Color(255, 165, 0),
		std::make_unique<PentagonStrategy>(200, 200, 50, 50)));
	picture.AddShape(std::make_unique<Shape>("e", Color(255, 215, 0),
		std::make_unique<StarStrategy>(400, 400, 60, 60)));

	DocumentSerializer::Save(picture, "test_5shapes.tmp");
	auto loaded = DocumentSerializer::Load("test_5shapes.tmp");

	CHECK(loaded->GetShapes().size() == 5);
	CHECK(loaded->GetShape("d")->GetTypeName() == "pentagon");
	CHECK(loaded->GetShape("e")->GetTypeName() == "star");

	std::remove("test_5shapes.tmp");
}

TEST_CASE("ChangeColorCommand: Undo/Redo restores color")
{
	Picture picture;
	CommandHistory history;

	picture.AddShape(std::make_unique<Shape>("a", Color(255, 0, 0),
		std::make_unique<RectangleStrategy>(0, 0, 10, 10)));

	Color oldColor = picture.GetShape("a")->GetColor();
	Color newColor(0, 255, 0);

	std::vector<ChangeColorCommand::Change> changes = { { "a", oldColor, newColor } };

	history.Execute(std::make_unique<ChangeColorCommand>(picture, changes));

	CHECK(picture.GetShape("a")->GetColor().GetHexStr() == "#00ff00");

	history.Undo();
	CHECK(picture.GetShape("a")->GetColor().GetHexStr() == "#ff0000");

	history.Redo();
	CHECK(picture.GetShape("a")->GetColor().GetHexStr() == "#00ff00");
}
#include "../Shape/editor/commands/PictureSnapshotCommand.h"
TEST_CASE("Nested groups move, scale, ungroup, undo and serialization")
{
    Picture picture;
    picture.AddShape(std::make_unique<Shape>("a",Color(255,0,0),
        std::make_unique<RectangleStrategy>(10, 10, 20, 20)));
    picture.AddShape(std::make_unique<Shape>("b",Color(0,255,0),
        std::make_unique<RectangleStrategy>(40, 10, 20, 20)));
    picture.AddShape(std::make_unique<Shape>("c",Color(0,0,255),
        std::make_unique<RectangleStrategy>(80, 10, 20, 20)));
    auto before = picture.Clone();
    REQUIRE(picture.GroupShapes({"a","b"}, "inner"));
    REQUIRE(picture.GroupShapes({"inner","c"}, "outer"));
    auto* outer = picture.GetShape("outer");
    REQUIRE(outer != nullptr);
    REQUIRE(outer->GetChildren().size() == 2);
    const auto old = outer->GetBounds();
    outer->Move(5,6);
    REQUIRE(outer->GetBounds().left == Approx(old.left+5));
    const auto moved = outer->GetBounds();
    outer->SetBounds({moved.left,moved.top,moved.width*2,moved.height*2});
    REQUIRE(outer->GetBounds().width == Approx(old.width*2));
    const auto after = picture.Clone();
    CommandHistory history;
    history.PushExecuted(std::make_unique<PictureSnapshotCommand>(
        picture, std::move(before), std::move(after),
        std::vector<std::string>{"a","b","c","outer"}));
    REQUIRE(history.CanUndo());
    history.Undo();
    REQUIRE(picture.GetShape("a") != nullptr);
    REQUIRE(picture.GetShape("outer") == nullptr);
    history.Redo();
    REQUIRE(picture.GetShape("outer") != nullptr);
    const std::string filename = "groups_roundtrip.txt";
    DocumentSerializer::Save(picture,filename);
    auto loaded = DocumentSerializer::Load(filename);
    REQUIRE(loaded->GetShape("outer") != nullptr);
    REQUIRE(loaded->GetShape("outer")->GetChildren().size() == 2);
    REQUIRE(loaded->UngroupShape("outer").size() == 2);
    REQUIRE(loaded->GetShape("inner") != nullptr);
}
