#pragma once

#include "../editor/commands/ChangeColorCommand.h"
#include "../editor/history/CommandHistory.h"
#include "../model/Picture.h"
#include "EditorState.h"
#include <boost/signals2.hpp>
#include <memory>
#include <string>
#include <utility>

class EditorController
{
public:
	EditorController(Picture& picture, EditorState& state, CommandHistory& history);

	void OnMousePressed(double x, double y, bool isCtrlPressed);
	void OnMouseMoved(double x, double y);
	void OnMouseReleased(double x, double y);

	void SetTool(Tool tool);
	void CancelDraft();
	Shape* GetDraftShape() const;

	void DeleteShape();
	void SaveDocument(const std::string& filename);
	void LoadDocument(const std::string& filename);

	void Undo();
	void Redo();

	void SetSelectedShapesColor(const Color& color);
	void ReorderShape(const std::string& id, int newIndex);

private:
	Picture& m_picture;
	EditorState& m_state;
	CommandHistory& m_history;
	int m_nextId = 1;
	const double m_minSizeShape = 16.0;

	std::unique_ptr<Shape> m_draftShape;

	bool IsDrawingMode() const;
	void StartDraft(double x, double y);
	void UpdateDraft(double x, double y);
	void FinishDraft();
	std::string GenerateNewShapeId();

	void HandleSelection(double x, double y, bool isCtrlPressed);
	void ToggleSelection(const std::string& id);
	void ResetSelectionTo(const std::string& id);
	void PressCtl(Shape* shape, bool isCtrlPressed);

	void RememberDragStartPositions();

	void UpdateMovePreview(double x, double y);
	void CommitMove();

	std::pair<double, double> ClampDragDelta(double dx, double dy) const;
	void UpdateResizePreview(double x, double y);
	bool SwitchModeResize(double x, double y);

	Bounds ComputeDesiredMainBounds(const Bounds& oldMain, double x, double y) const;
	Bounds ComputeAnchoredMainBounds(const Bounds& oldMain, double scaleX, double scaleY) const;
	Bounds ComputeGroupBounds(double scaleX, double scaleY, const Bounds& newMain, const Bounds& oldMain) const;
	void ClampGroupScale(const Bounds& groupBounds, double& scaleX, double& scaleY) const;
	Bounds ShiftMainIntoCanvas(const Bounds& main, double scaleX, double scaleY,
		const Bounds& oldMain) const;

	Handle HitHandle(const Bounds& b, double x, double y) const;
	Bounds AdjustBasedCanvas(Bounds newBounds) const;

	void CommitResize();
	std::map<std::string, Bounds> ComputeResizePreview(double scaleX, double scaleY, const Bounds& newMain, const Bounds& oldMain) const;
};