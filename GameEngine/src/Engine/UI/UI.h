#pragma once

#include <imgui/imgui.h>

namespace Engine::UI
{
	struct ScopedStyleColor
	{
		ScopedStyleColor(ImGuiCol idx, const ImVec4& col, bool predicate = true)
			:m_Set(predicate)
		{
			if (m_Set)
				ImGui::PushStyleColor(idx, col);
		}

		ScopedStyleColor(ImGuiCol idx, ImU32 col, bool predicate = true)
			:m_Set(predicate)
		{
			if (m_Set)
				ImGui::PushStyleColor(idx, col);
		}

		~ScopedStyleColor()
		{
			if (m_Set)
				ImGui::PopStyleColor();
		}

	private:
		bool m_Set = false;
	};
}
