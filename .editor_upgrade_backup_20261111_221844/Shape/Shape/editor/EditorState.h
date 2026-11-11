#pragma once

#include "../model/Bounds.h"
#include "../model/Color.h"
#include <algorithm>
#include <map>
#include <optional>
#include <string>
#include <vector>

enum class Handle
{
	None,
	TL,
	TR,
	BR,
	BL
};

enum class Tool
{
	Select,
	DrawRect,
	DrawEllipse,
	DrawTriangle,
	DrawPentagon,
	DrawStar
};

class EditorState
{
public:
	const double canvasWidth = 1000.0;
	const double canvasHeight = 600.0;
	const double toolbarHeight = 40.0;
	const double listPanelWidth = 200.0;

	std::string currentFileName;
	std::string lastError;
	int draggedListIndex = -1;
	int dropListIndex = -1;
	std::string draggedShapeId;

	std::vector<std::string> SelectedIds;
	struct DragState
	{
		bool isDrag = false;
		double startX = 0.0;
		double startY = 0.0;
		std::map<std::string, Bounds> originalBounds;
	};
	DragState drag;

	struct ResizeState
	{
		bool isResize = false;
		Handle handle = Handle::None;
		std::string mainId;
		std::map<std::string, Bounds> originalBounds;
	};
	ResizeState resize;

	Tool activeTool = Tool::Select;
	struct DraftState
	{
		bool isDrafting = false;
		double startX = 0.0;
		double startY = 0.0;
	};
	DraftState draft;

	struct PreviewState
	{
		bool hasMovePreview = false;
		double movePreviewDx = 0.0;
		double movePreviewDy = 0.0;

		bool hasResizePreview = false;
		std::map<std::string, Bounds> resizePreviewBounds;
	};
	PreviewState preview;

	void ClearPreview()
	{
		preview.hasMovePreview = false;
		preview.movePreviewDx = 0.0;
		preview.movePreviewDy = 0.0;

		preview.hasResizePreview = false;
		preview.resizePreviewBounds.clear();
	}

	bool IsSelected(const std::string& id) const
	{
		return std::find(SelectedIds.begin(), SelectedIds.end(), id) != SelectedIds.end();
	}
	void ClearSelection()
	{
		SelectedIds.clear();
	}

	void AddToSelection(const std::string& id)
	{
		SelectedIds.push_back(id);
	}

	double GetUsableCanvasWidth() const
	{
		return canvasWidth - listPanelWidth;
	}
};