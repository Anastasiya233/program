#include "EditorController.h"
#include "../editor/serializer/DocumentSerializer.h"
#include "../model/shapes/ShapeStrategyFactory.h"
#include "commands/AddShapeCommand.h"
#include "commands/PictureSnapshotCommand.h"
#include "commands/MacroCommand.h"
#include "commands/ChangeColorCommand.h"
#include "commands/DeleteShapesCommand.h"
#include "commands/MoveShapesCommand.h"
#include "commands/ResizeShapesCommand.h"
#include "history/CommandHistory.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include "commands/ReorderShapeCommand.h"

//TODO: лучше через стате(EditorState) делать и перемещение тоже(сейчас перемещение через паттерн комады, а изменение размеров фигур через макро команды)

const double minScale = 1e-3;
const double floatEps = 1e-9;

EditorController::EditorController(Picture& picture, EditorState& state, CommandHistory& history)
	: m_picture(picture)
	, m_state(state)
	, m_history(history)
{
}

void EditorController::OnMousePressed(double x, double y, bool isCtrlPressed)
{
	if (IsDrawingMode())
	{
		StartDraft(x, y);
		return;
	}
	HandleSelection(x, y, isCtrlPressed);
}

void EditorController::OnMouseMoved(double x, double y)
{
	if (m_state.draft.isDrafting)
	{
		UpdateDraft(x, y);
		return;
	}

	if (m_state.SelectedIds.empty())
	{
		return;
	}

	if (m_state.resize.isResize)
	{
		UpdateResizePreview(x, y);
		return;
	}

	if (m_state.drag.isDrag)
	{
		UpdateMovePreview(x, y);
	}
}

void EditorController::UpdateMovePreview(double x, double y)
{
	if (!m_state.drag.isDrag || m_state.SelectedIds.empty())
	{
		return;
	}

	const double rawDx = x - m_state.drag.startX;
	const double rawDy = y - m_state.drag.startY;
	const auto [dx, dy] = ClampDragDelta(rawDx, rawDy);

	m_state.preview.hasMovePreview = true;
	m_state.preview.movePreviewDx = dx;
	m_state.preview.movePreviewDy = dy;
}

void EditorController::OnMouseReleased(double x, double y)
{
	if (m_state.draft.isDrafting)
	{
		FinishDraft();
		return;
	}

	if (m_state.drag.isDrag)
	{
		CommitMove();
		return;
	}

	if (m_state.resize.isResize)
	{
		CommitResize();
		return;
	}
}

void EditorController::CommitResize()
{
	if (!m_state.resize.isResize)
	{
		return;
	}

	std::vector<ResizeShapesCommand::Change> changes;

	if (m_state.preview.hasResizePreview)
	{
		for (const auto& [id, after] : m_state.preview.resizePreviewBounds)
		{
			auto itBefore = m_state.resize.originalBounds.find(id);
			if (itBefore == m_state.resize.originalBounds.end())
			{
				continue;
			}

			const Bounds& before = itBefore->second;
			const bool changed = before.left != after.left || before.top != after.top || before.width != after.width || before.height != after.height;

			if (!changed)
			{
				continue;
			}

			changes.push_back({ id, before, after });
		}
	}

	m_state.ClearPreview();
	m_state.resize.isResize = false;
	m_state.resize.handle = Handle::None;
	m_state.resize.originalBounds.clear();

	if (changes.empty())
	{
		return;
	}

	for (const auto& c : changes)
	{
		if (auto* shape = m_picture.GetShape(c.id))
		{
			shape->SetBounds(c.after);
		}
	}

	std::vector<std::unique_ptr<ICommand>> steps;
	for (const auto& c : changes)
		steps.push_back(std::make_unique<ResizeOneShapeCommand>(
			m_picture, c.id, c.before, c.after));
	m_history.PushExecuted(std::make_unique<MacroCommand>(std::move(steps)));
}

bool EditorController::IsDrawingMode() const
{
	return m_state.activeTool != Tool::Select;
}

void EditorController::StartDraft(double x, double y)
{
	m_state.draft.isDrafting = true;
	m_state.draft.startX = x;
	m_state.draft.startY = y;
	m_draftShape.reset();
}

void EditorController::UpdateDraft(double x, double y)
{
	const double left = std::min(m_state.draft.startX, x);
	const double top = std::min(m_state.draft.startY, y);
	const double width = std::abs(x - m_state.draft.startX);
	const double height = std::abs(y - m_state.draft.startY);

	if (width < floatEps || height < floatEps)
	{
		m_draftShape.reset();
		return;
	}

	const Bounds clamped = AdjustBasedCanvas({ left, top, width, height });
	m_draftShape = ShapeStrategyFactory::CreateDraftShape(
		m_state.activeTool,
		clamped.left, clamped.top, clamped.width, clamped.height);
}

void EditorController::FinishDraft()
{
	if (m_draftShape && m_draftShape->GetBounds().width >= m_minSizeShape)
	{
		const std::string newId = GenerateNewShapeId();
		auto shape = m_draftShape->Clone(newId);
		const int index = static_cast<int>(m_picture.GetShapes().size());

		m_history.Execute(std::make_unique<AddShapeCommand>(
			m_picture, std::move(shape), index));

		m_state.ClearSelection();
		m_state.AddToSelection(newId);
	}

	CancelDraft();
	m_state.activeTool = Tool::Select;
}

std::string EditorController::GenerateNewShapeId()
{
	std::string id;
	do
	{
		id = "shape" + std::to_string(m_nextId++);
	} while ([&]() {
		std::function<bool(const Shape*)> contains = [&](const Shape* s) {
			if (s->GetId() == id) return true;
			for (const Shape* child : s->GetChildren())
				if (contains(child)) return true;
			return false;
		};
		for (const Shape* s : m_picture.GetShapes())
			if (contains(s)) return true;
		return false;
	}());
	return id;
}

void EditorController::SetTool(Tool tool)
{
	m_state.activeTool = tool;
	m_state.ClearSelection();
	CancelDraft();
	m_state.ClearPreview();
}

void EditorController::CancelDraft()
{
	m_draftShape.reset();
	m_state.draft.isDrafting = false;
}

Shape* EditorController::GetDraftShape() const
{
	return m_draftShape.get();
}

void EditorController::ToggleSelection(const std::string& id)
{
	auto it = std::find(m_state.SelectedIds.begin(), m_state.SelectedIds.end(), id);
	if (it != m_state.SelectedIds.end())
	{
		m_state.SelectedIds.erase(it);
	}
	else
	{
		m_state.SelectedIds.push_back(id);
	}
}

void EditorController::ResetSelectionTo(const std::string& id)
{
	m_state.ClearSelection();
	m_state.SelectedIds.push_back(id);
}

void EditorController::PressCtl(Shape* shape, bool isCtrlPressed)
{
	const std::string& id = shape->GetId();

	if (isCtrlPressed)
	{
		ToggleSelection(id);
	}
	else if (!m_state.IsSelected(id))
	{
		ResetSelectionTo(id);
	}

	RememberDragStartPositions();
}

void EditorController::HandleSelection(double x, double y, bool isCtrlPressed)
{
	if (SwitchModeResize(x, y))
	{
		return;
	}

	auto* shape = m_picture.GetTopShapeAt(x, y);

	if (shape)
	{
		PressCtl(shape, isCtrlPressed);
		m_state.drag.isDrag = true;
		m_state.drag.startX = x;
		m_state.drag.startY = y;
	}
	else
	{
		if (!isCtrlPressed)
		{
			m_state.ClearSelection();
		}
		m_state.drag.isDrag = false;
	}
	m_state.resize.isResize = false;
	m_state.ClearPreview();
}

void EditorController::RememberDragStartPositions()
{
	m_state.drag.originalBounds.clear();
	for (const auto& selId : m_state.SelectedIds)
	{
		if (auto* selShape = m_picture.GetShape(selId))
		{
			m_state.drag.originalBounds[selId] = selShape->GetBounds();
		}
	}
}

void EditorController::UpdateResizePreview(double x, double y)
{
	if (m_state.SelectedIds.empty() || !m_state.resize.isResize)
	{
		return;
	}

	const std::string& mainId = m_state.resize.mainId;
	auto itOldMain = m_state.resize.originalBounds.find(mainId);
	if (itOldMain == m_state.resize.originalBounds.end())
	{
		return;
	}
	const Bounds oldMain = itOldMain->second;

	const Bounds desiredMain = ComputeDesiredMainBounds(oldMain, x, y);

	double scaleX = (oldMain.width > 0.0) ? desiredMain.width / oldMain.width : 1.0;
	double scaleY = (oldMain.height > 0.0) ? desiredMain.height / oldMain.height : 1.0;
	if (scaleX < minScale)
	{
		scaleX = minScale;
	}
	if (scaleY < minScale)
	{
		scaleY = minScale;
	}

	Bounds anchored = ComputeAnchoredMainBounds(oldMain, scaleX, scaleY);

	const Bounds groupBounds = ComputeGroupBounds(scaleX, scaleY, anchored, oldMain);
	ClampGroupScale(groupBounds, scaleX, scaleY);

	anchored = ComputeAnchoredMainBounds(oldMain, scaleX, scaleY);
	anchored = ShiftMainIntoCanvas(anchored, scaleX, scaleY, oldMain);

	m_state.preview.hasResizePreview = true;
	m_state.preview.resizePreviewBounds = ComputeResizePreview(scaleX, scaleY, anchored, oldMain);
}

std::map<std::string, Bounds> EditorController::ComputeResizePreview(
	double scaleX, double scaleY,
	const Bounds& newMain, const Bounds& oldMain) const
{
	std::map<std::string, Bounds> result;
	for (const auto& [id, oldB] : m_state.resize.originalBounds)
	{
		Bounds nb;
		nb.left = newMain.left + (oldB.left - oldMain.left) * scaleX;
		nb.top = newMain.top + (oldB.top - oldMain.top) * scaleY;
		nb.width = oldB.width * scaleX;
		nb.height = oldB.height * scaleY;
		result[id] = nb;
	}
	return result;
}

void EditorController::ReorderShape(const std::string& id, int newIndex)
{
	int oldIndex = m_picture.GetShapeIndex(id);
	if (oldIndex < 0 || oldIndex == newIndex)
		return;

	m_history.Execute(std::make_unique<ReorderShapeCommand>(
		m_picture, id, oldIndex, newIndex));
}

void EditorController::CommitMove()
{
	const double dx = m_state.preview.movePreviewDx;
	const double dy = m_state.preview.movePreviewDy;
	const bool changed = (dx != 0.0 || dy != 0.0);

	m_state.ClearPreview();
	m_state.drag.isDrag = false;

	if (!changed)
	{
		return;
	}

	const std::vector<std::string> ids = m_state.SelectedIds;

	for (const auto& id : ids)
	{
		if (auto* shape = m_picture.GetShape(id))
		{
			shape->Move(dx, dy);
		}
	}

	std::vector<std::unique_ptr<ICommand>> steps;
	for (const auto& id : ids)
		steps.push_back(std::make_unique<MoveOneShapeCommand>(m_picture,id,dx,dy));
	m_history.PushExecuted(std::make_unique<MacroCommand>(std::move(steps)));
}

std::pair<double, double> EditorController::ClampDragDelta(double dx, double dy) const
{
	double minLeft = std::numeric_limits<double>::max();
	double minTop = std::numeric_limits<double>::max();
	double maxRight = std::numeric_limits<double>::lowest();
	double maxBottom = std::numeric_limits<double>::lowest();

	for (const auto& [id, bounds] : m_state.drag.originalBounds)
	{
		minLeft = std::min(minLeft, bounds.left);
		minTop = std::min(minTop, bounds.top);
		maxRight = std::max(maxRight, bounds.Right());
		maxBottom = std::max(maxBottom, bounds.Bottom());
	}

	if (minLeft + dx < 0)
	{
		dx = -minLeft;
	}
	if (maxRight + dx > m_state.GetUsableCanvasWidth())
	{
		dx = m_state.GetUsableCanvasWidth() - maxRight;
	}
	if (minTop + dy < m_state.toolbarHeight)
	{
		dy = m_state.toolbarHeight - minTop;
	}
	if (maxBottom + dy > m_state.canvasHeight)
	{
		dy = m_state.canvasHeight - maxBottom;
	}
	return { dx, dy };
}

bool EditorController::SwitchModeResize(double x, double y)
{
	if (m_state.SelectedIds.empty())
	{
		return false;
	}

	auto* shape = m_picture.GetShape(m_state.SelectedIds[0]);
	if (!shape)
	{
		return false;
	}

	const Handle handle = HitHandle(shape->GetBounds(), x, y);
	if (handle == Handle::None)
	{
		return false;
	}

	m_state.resize.isResize = true;
	m_state.drag.isDrag = false;
	m_state.resize.handle = handle;
	m_state.resize.mainId = m_state.SelectedIds[0];
	m_state.resize.originalBounds.clear();

	for (const auto& id : m_state.SelectedIds)
	{
		if (auto* s = m_picture.GetShape(id))
		{
			m_state.resize.originalBounds[id] = s->GetBounds();
		}
	}
	return true;
}

Bounds EditorController::ComputeDesiredMainBounds(const Bounds& oldMain,
	double x, double y) const
{
	Bounds desired = oldMain;

	switch (m_state.resize.handle)
	{
	case Handle::TL:
		desired.left = std::min(x, oldMain.Right() - m_minSizeShape);
		desired.top = std::min(y, oldMain.Bottom() - m_minSizeShape);
		desired.width = oldMain.Right() - desired.left;
		desired.height = oldMain.Bottom() - desired.top;
		break;
	case Handle::TR:
		desired.top = std::min(y, oldMain.Bottom() - m_minSizeShape);
		desired.width = std::max(x - oldMain.left, m_minSizeShape);
		desired.height = oldMain.Bottom() - desired.top;
		break;
	case Handle::BR:
		desired.width = std::max(x - oldMain.left, m_minSizeShape);
		desired.height = std::max(y - oldMain.top, m_minSizeShape);
		break;
	case Handle::BL:
		desired.left = std::min(x, oldMain.Right() - m_minSizeShape);
		desired.width = oldMain.Right() - desired.left;
		desired.height = std::max(y - oldMain.top, m_minSizeShape);
		break;
	case Handle::None:
	default:
		break;
	}
	return AdjustBasedCanvas(desired);
}

Bounds EditorController::ComputeAnchoredMainBounds(const Bounds& oldMain,
	double scaleX, double scaleY) const
{
	const double w = oldMain.width * scaleX;
	const double h = oldMain.height * scaleY;
	Bounds nb{ 0.0, 0.0, w, h };

	switch (m_state.resize.handle)
	{
	case Handle::TL:
		nb.left = oldMain.Right() - w;
		nb.top = oldMain.Bottom() - h;
		break;
	case Handle::TR:
		nb.left = oldMain.left;
		nb.top = oldMain.Bottom() - h;
		break;
	case Handle::BR:
		nb.left = oldMain.left;
		nb.top = oldMain.top;
		break;
	case Handle::BL:
		nb.left = oldMain.Right() - w;
		nb.top = oldMain.top;
		break;
	case Handle::None:
	default:
		nb.left = oldMain.left;
		nb.top = oldMain.top;
		break;
	}
	return nb;
}

Bounds EditorController::ComputeGroupBounds(double scaleX, double scaleY,
	const Bounds& newMain, const Bounds& oldMain) const
{
	double minL = std::numeric_limits<double>::max();
	double minT = std::numeric_limits<double>::max();
	double maxR = std::numeric_limits<double>::lowest();
	double maxB = std::numeric_limits<double>::lowest();

	for (const auto& [id, oldB] : m_state.resize.originalBounds)
	{
		const double nl = newMain.left + (oldB.left - oldMain.left) * scaleX;
		const double nt = newMain.top + (oldB.top - oldMain.top) * scaleY;
		const double nr = nl + oldB.width * scaleX;
		const double nb = nt + oldB.height * scaleY;

		minL = std::min(minL, nl);
		minT = std::min(minT, nt);
		maxR = std::max(maxR, nr);
		maxB = std::max(maxB, nb);
	}
	return { minL, minT, maxR - minL, maxB - minT };
}

void EditorController::ClampGroupScale(const Bounds& groupBounds,
	double& scaleX, double& scaleY) const
{
	const double maxX = m_state.GetUsableCanvasWidth();
	const double minY = m_state.toolbarHeight;
	const double maxY = m_state.canvasHeight;

	if (groupBounds.width > floatEps
		&& (groupBounds.left < 0.0 || groupBounds.Right() > maxX))
	{
		const double allowed = std::min(groupBounds.Right(), maxX)
			- std::max(groupBounds.left, 0.0);
		if (allowed > 0.0 && allowed < groupBounds.width)
		{
			scaleX *= allowed / groupBounds.width;
		}
	}

	if (groupBounds.height > floatEps
		&& (groupBounds.top < minY || groupBounds.Bottom() > maxY))
	{
		const double allowed = std::min(groupBounds.Bottom(), maxY)
			- std::max(groupBounds.top, minY);
		if (allowed > 0.0 && allowed < groupBounds.height)
		{
			scaleY *= allowed / groupBounds.height;
		}
	}

	if (scaleX < minScale)
	{
		scaleX = minScale;
	}
	if (scaleY < minScale)
	{
		scaleY = minScale;
	}
}

Bounds EditorController::ShiftMainIntoCanvas(const Bounds& main,
	double scaleX, double scaleY, const Bounds& oldMain) const
{
	const double maxX = m_state.GetUsableCanvasWidth();
	const double minY = m_state.toolbarHeight;
	const double maxY = m_state.canvasHeight;

	Bounds shifted = main;
	const Bounds g = ComputeGroupBounds(scaleX, scaleY, shifted, oldMain);

	if (g.Right() > maxX)
	{
		shifted.left -= (g.Right() - maxX);
	}
	if (g.left < 0.0)
	{
		shifted.left -= g.left;
	}
	if (g.Bottom() > maxY)
	{
		shifted.top -= (g.Bottom() - maxY);
	}
	if (g.top < minY)
	{
		shifted.top -= (g.top - minY);
	}
	return shifted;
}

Handle EditorController::HitHandle(const Bounds& b, double x, double y) const
{
	const double hitHandleSize = 6.0;

	bool hitLeft = std::abs(x - b.left) <= hitHandleSize;
	bool hitRight = std::abs(x - b.Right()) <= hitHandleSize;
	bool hitTop = std::abs(y - b.top) <= hitHandleSize;
	bool hitBottom = std::abs(y - b.Bottom()) <= hitHandleSize;

	if (hitLeft && hitTop)
	{
		return Handle::TL;
	}
	if (hitRight && hitTop)
	{
		return Handle::TR;
	}
	if (hitRight && hitBottom)
	{
		return Handle::BR;
	}
	if (hitLeft && hitBottom)
	{
		return Handle::BL;
	}
	return Handle::None;
}

Bounds EditorController::AdjustBasedCanvas(Bounds newBounds) const
{
	if (newBounds.left < 0)
	{
		newBounds.width += newBounds.left;
		newBounds.left = 0;
	}
	if (newBounds.top < m_state.toolbarHeight)
	{
		newBounds.height -= m_state.toolbarHeight - newBounds.top;
		newBounds.top = m_state.toolbarHeight;
	}
	if (newBounds.Right() > m_state.GetUsableCanvasWidth())
	{
		newBounds.width = m_state.GetUsableCanvasWidth() - newBounds.left;
	}
	if (newBounds.Bottom() > m_state.canvasHeight)
	{
		newBounds.height = m_state.canvasHeight - newBounds.top;
	}
	if (newBounds.width < m_minSizeShape)
	{
		newBounds.width = m_minSizeShape;
	}
	if (newBounds.height < m_minSizeShape)
	{
		newBounds.height = m_minSizeShape;
	}
	return newBounds;
}

void EditorController::GroupSelection()
{
    if (m_state.SelectedIds.size() < 2) return;
    auto before = m_picture.Clone();
    auto affected = m_state.SelectedIds;
    const std::string id = GenerateNewShapeId();
    affected.push_back(id);
    if (!m_picture.GroupShapes(m_state.SelectedIds, id)) return;
    auto after = m_picture.Clone();
    m_history.PushExecuted(std::make_unique<PictureSnapshotCommand>(
        m_picture, std::move(before), std::move(after), affected));
    m_state.ClearSelection();
    m_state.AddToSelection(id);
    m_state.ClearPreview();
}

void EditorController::UngroupSelection()
{
    if (m_state.SelectedIds.size() != 1) return;
    auto before = m_picture.Clone();
    const std::string originalGroupId = m_state.SelectedIds.front();
    auto ids = m_picture.UngroupShape(originalGroupId);
    if (ids.empty()) return;
    auto affected = ids;
    affected.push_back(originalGroupId);
    auto after = m_picture.Clone();
    m_history.PushExecuted(std::make_unique<PictureSnapshotCommand>(
        m_picture, std::move(before), std::move(after), affected));
    m_state.SelectedIds = std::move(ids);
    m_state.ClearPreview();
}

void EditorController::DeleteShape()
{
	if (m_state.SelectedIds.empty())
	{
		return;
	}

	const std::vector<std::string> ids = m_state.SelectedIds;

	m_history.Execute(std::make_unique<DeleteShapesCommand>(m_picture, ids));

	m_state.ClearSelection();
	m_state.drag.isDrag = false;
	m_state.resize.isResize = false;
	m_state.ClearPreview();
}

void EditorController::SaveDocument(const std::string& filename)
{
	try
	{
		DocumentSerializer::Save(m_picture, filename);
	}
	catch (const std::exception& e)
	{
		m_state.currentFileName = "";
		m_state.lastError = "Error saving: " + std::string(e.what());
	}
}

void EditorController::LoadDocument(const std::string& filename)
{
	try
	{
		auto newPicture = DocumentSerializer::Load(filename);
		m_picture.Clear();
		for (const auto* shape : newPicture->GetShapes())
		{
			m_picture.AddShape(shape->Clone(shape->GetId()));
		}
		m_state.ClearSelection();
		m_state.ClearPreview();
		m_history.Clear();
	}
	catch (const std::exception& e)
	{
		m_state.currentFileName = "";
		m_state.lastError = "Error loading: " + std::string(e.what());
	}
}

void EditorController::Undo()
{
	try
	{
		const auto ids = m_history.Undo();
		m_state.ClearSelection();
		for (const auto& id : ids)
			if (m_picture.GetShape(id)) m_state.AddToSelection(id);
		m_state.ClearPreview();
	}
	catch (const std::exception& e)
	{
		m_state.lastError = std::string("Undo failed: ") + e.what();
	}
}

void EditorController::Redo()
{
	try
	{
		const auto ids = m_history.Redo();
		m_state.ClearSelection();
		for (const auto& id : ids)
			if (m_picture.GetShape(id)) m_state.AddToSelection(id);
		m_state.ClearPreview();
	}
	catch (const std::exception& e)
	{
		m_state.lastError = std::string("Redo failed: ") + e.what();
	}
}

void EditorController::SetSelectedShapesColor(const Color& color)
{
	if (m_state.SelectedIds.empty())
	{
		return;
	}

	// Groups require a snapshot to restore all descendants' individual colors.
	bool includesGroup = false;
	for (const auto& id : m_state.SelectedIds)
		if (auto* s = m_picture.GetShape(id))
			includesGroup |= !s->GetChildren().empty();
	if (includesGroup)
	{
		auto before = m_picture.Clone();
		std::function<bool(const Shape*)> differs = [&](const Shape* shape) {
			if (shape->GetColor().GetHexStr() != color.GetHexStr()) return true;
			for (const Shape* child : shape->GetChildren())
				if (differs(child)) return true;
			return false;
		};
		bool changed = false;
		for (const auto& id : m_state.SelectedIds)
		{
			if (auto* shape = m_picture.GetShape(id); shape && differs(shape))
			{
				changed = true;
				shape->SetColorRecursive(color);
			}
		}
		if (changed)
		{
			auto after = m_picture.Clone();
			m_history.PushExecuted(std::make_unique<PictureSnapshotCommand>(
				m_picture, std::move(before), std::move(after), m_state.SelectedIds));
		}
		return;
	}
	std::vector<ChangeColorCommand::Change> changes;

	for (const auto& id : m_state.SelectedIds)
	{
		if (auto* shape = m_picture.GetShape(id))
		{
			Color oldColor = shape->GetColor();
			if (oldColor.GetHexStr() != color.GetHexStr())
			{
				changes.push_back({ id, oldColor, color });
			}
		}
	}

	if (changes.empty())
	{
		return;
	}

	m_history.Execute(std::make_unique<ChangeColorCommand>(m_picture, std::move(changes)));
}