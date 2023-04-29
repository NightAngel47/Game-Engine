#include "SceneHierarchyPanel.h"

#include "Engine/ImGui/ImGuiUI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Engine
{
#define SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields)								\
	ScriptFieldInstance& scriptField = fieldExists ? entityFields.at(name) : entityFields[name];				\
	if (!sceneRunning && !fieldExists)																			\
		scriptField.Field = field;																				\

#define GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, className, Type)	\
	if (sceneRunning)																							\
	{																											\
		data = scriptInstance->GetFieldValue<Type>(name);														\
	}																											\
	else if (fieldExists)																						\
	{																											\
		data = scriptField.GetValue<Type>();																	\
	}																											\
	else 																										\
	{																											\
		data = ScriptEngine::GetDefaultScriptFieldMap(component.ClassName).at(name).GetValue<Type>();			\
		scriptField.SetValue(data);																				\
	}																											\

	static void FieldTypeUnsupported(const ScriptFieldType& fieldType)
	{
		const char* typeName = Utils::ScriptFieldTypeToString(fieldType);
		ImGui::Text(typeName);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		:m_SelectionContext(UUID::INVALID())
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = UUID::INVALID();
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				if (entity.GetComponent<RelationshipComponent>().Parent.IsValid())
					return;

				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = UUID::INVALID();

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Entity"))
					m_Context->CreateEntity();
				else if (ImGui::MenuItem("Create Sprite"))
					m_Context->CreateEntity("Sprite").AddComponent<SpriteRendererComponent>();
				else if (ImGui::MenuItem("Create Circle"))
					m_Context->CreateEntity("Circle").AddComponent<CircleRendererComponent>();
				else if (ImGui::MenuItem("Create Camera"))
					m_Context->CreateEntity("Camera").AddComponent<CameraComponent>();
				else if (ImGui::MenuItem("Create from Prefab"))
					CreateFromPrefab();

				ImGui::EndPopup();
			}

			glm::vec2 DDTASize = glm::vec2( 1.0f, 1.0f );
			ImVec2 ContentRegionAvailable = ImGui::GetContentRegionAvail();
			DDTASize += glm::vec2(ContentRegionAvailable.x, ContentRegionAvailable.y);
			ImGui::InvisibleButton("##DragDropTargetArea", ImVec2(DDTASize.x, DDTASize.y));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					const wchar_t* fileExtension = std::wcsrchr(path, '.');

					if (std::wcscmp(fileExtension, L".prefab") == 0)
					{
						CreateFromPrefab(path);
					}
					else
					{
						std::wstring ws(fileExtension);
						ENGINE_CORE_WARN("File type is not supported by drag and drop in the Scene Hierarchy Panel: " + std::string(ws.begin(), ws.end()));
					}
				}

				ImGui::EndDragDropTarget();
			}
		}
		
		ImGui::End();

		ImGui::Begin("Properties");
		if(IsSelectedEntityValid())
		{
			Entity selectedEntity = GetSelectedEntity();
			DrawComponents(selectedEntity);
		}
		
		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity.GetUUID();
	}

	void SceneHierarchyPanel::SetSelectedEntity(UUID entityID)
	{
		m_SelectionContext = entityID;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetName();

		ImGuiTreeNodeFlags flags = (IsSelectedEntityValid() && (GetSelectedEntity() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= (entity.GetComponent<RelationshipComponent>().HasChildren()) ? 0 : ImGuiTreeNodeFlags_Leaf;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::BeginDragDropSource())
		{
			const UUID* entityItem = &entity.GetUUID();
			ImGui::SetDragDropPayload("SCENE_HIERARCHY_ENTITY_ITEM", entityItem, sizeof(UUID*), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY_ITEM"))
			{
				const UUID* entityItemID = (const UUID*)payload->Data;
				Entity entityItem = m_Context->GetEntityWithUUID(*entityItemID);
				entity.AddChild(entityItem);
			}

			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered() && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			m_SelectionContext = entity.GetUUID();
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			else if (ImGui::MenuItem("Create Prefab"))
				SavePrefabAs();
			else if (ImGui::MenuItem("Create Child Entity"))
				CreateChildEntity();

			ImGui::EndPopup();
		}

		if(opened)
		{
			for (Entity childEntity : entity.Children())
			{
				DrawEntityNode(childEntity);
			}

			ImGui::TreePop();
		}

		if(entityDeleted)
		{
			if (IsSelectedEntityValid() && GetSelectedEntity() == entity)
				m_SelectionContext = UUID::INVALID();
			m_Context->DestroyEntity(entity);
		}
	}

	static void DrawVec3Control(const:: std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		const auto boldFont = io.Fonts->Fonts[0];
		
		ImGui::PushID(label.c_str());
		
		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		const ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if(ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		
		ImGui::Columns(1);
		
		ImGui::PopID();
	}

	template <typename T, typename UIFunction>
	static void DrawComponent(const:: std::string& label, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;
		
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
			const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removeComponent = true;
				
				ImGui::EndPopup();
			}
			
			if (open)
			{
				uiFunction(component);
				
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if(entity.HasComponent<TagComponent>())
		{
			auto& component = entity.GetComponent<TagComponent>();
			ImGui::InputText("##Tag", &component.Tag);

		}

		// Draw Add Component Button at the top right next to the tag input field.
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DisplayAddComponentEntry<TextRendererComponent>("Text Renderer");
			DisplayAddComponentEntry<UILayoutComponent>("UI Layout Component");
			DisplayAddComponentEntry<UIButtonComponent>("UI Button Component");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			DisplayAddComponentEntry<ScriptComponent>("Script Component");
			
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Position", component.Position);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<UILayoutComponent>("UI Layout", entity, [](auto& component)
		{
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size), 1.0f);
			ImGui::DragFloat2("Anchor Min", glm::value_ptr(component.AnchorMin), 0.1f, -1.0f, 1.0f);
			ImGui::DragFloat2("Anchor Max", glm::value_ptr(component.AnchorMax), 0.1f, -1.0f, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::Checkbox("Primary", &component.Primary);
			
			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) 
			{
				for (int i = 0; i < 2; ++i)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						camera.SetProjectionType((SceneCamera::ProjectionType)i);

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				
				ImGui::EndCombo();
			}

			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.SetPerspectiveFarClip(perspectiveFar);
			}
			
			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.SetOrthographicSize(orthoSize);
				
				float orthoNear = camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.SetOrthographicNearClip(orthoNear);
				
				float orthoFar = camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.SetOrthographicFarClip(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			}
		});
		
		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			
			const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			const ImVec2 buttonSize = {0.0f, lineHeight};
			ImGui::Text("Texture");
			ImGui::SameLine();

			std::string textureName = component.Path.empty() ? "None" : component.Path.string();
			ImGui::Button(textureName.c_str(), buttonSize);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					const wchar_t* fileExtension = std::wcsrchr(path, '.');
				
					if (std::wcscmp(fileExtension, L".png") == 0)
					{
						component.LoadTexture(path);
					}
					else
					{
						std::wstring ws(fileExtension);
						ENGINE_CORE_WARN("File type is not supported by drag and drop in the Sprite Renderer Texture Slot: " + std::string(ws.begin(), ws.end()));
					}
				}
			
				ImGui::EndDragDropTarget();
			}
			
			ImGui::DragFloat("Tiling", &component.Tiling, 0.1f);

			ImGui::Separator();
			bool subtextureInvalidated = false;
			ImGui::Text("Sub-Texture Settings");

			if (ImGui::Checkbox("Is Sub-Texture", &component.IsSubTexture))
				subtextureInvalidated = true;
			if (ImGui::DragFloat2("Sub-Coords", glm::value_ptr(component.SubCoords), 1.0f, 0.0f, std::numeric_limits<float>().max(), "%.0f"))
				subtextureInvalidated = true;
			if (ImGui::DragFloat2("Sub-CellSize", glm::value_ptr(component.SubCellSize), 1.0f, 0.0f, std::numeric_limits<float>().max(), "%.0f"))
				subtextureInvalidated = true;
			if (ImGui::DragFloat2("Sub-SpriteSize", glm::value_ptr(component.SubSpriteSize), 1.0f, 1.0f, std::numeric_limits<float>().max(), "%.0f"))
				subtextureInvalidated = true;

			if (subtextureInvalidated)
			{
				component.GenerateSubTexture();
			}
		});
		
		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.0001f, 0.0f, 1.0f);
		});

		DrawComponent<TextRendererComponent>("Text Renderer", entity, [](auto& component)
		{
			ImGui::InputTextMultiline("Text String", &component.TextString);

			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::DragFloat("Kerning", &component.Kerning, 0.01f);
			ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.01f);
		});

		DrawComponent<UIButtonComponent>("UI Button", entity, [&](auto& component)
		{
			ImGui::Checkbox("Intractable", &component.ButtonState.Interactable);

			ImGui::ColorEdit4("Normal Color", glm::value_ptr(component.NormalColor));
			ImGui::ColorEdit4("Hover Color", glm::value_ptr(component.HoverColor));
			ImGui::ColorEdit4("Pressed Color", glm::value_ptr(component.PressedColor));
			ImGui::ColorEdit4("Disabled Color", glm::value_ptr(component.DisabledColor));

			ImGui::Text("Pressed Event");
			{
				bool isValid = component.PressedEvent.InteractedEntityID.IsValid();
				std::string entityName = "None";

				Entity eventEntity;
				if (isValid)
				{
					eventEntity = m_Context->GetEntityWithUUID(component.PressedEvent.InteractedEntityID);
					entityName = eventEntity.GetName();
				}

				const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				const ImVec2 buttonSize = { 0.0f, lineHeight };
				ImGui::Text("Entity");
				ImGui::SameLine();
				ImGui::Button(entityName.c_str(), buttonSize);

				// get entity target for interaction
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY_ITEM"))
					{
						const UUID* entityItemID = (const UUID*)payload->Data;
						component.PressedEvent.InteractedEntityID = *entityItemID;
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::Text("Function");
				ImGui::SameLine();

				if (isValid && eventEntity.HasComponent<ScriptComponent>())
				{
					// get script on entity target and select function
					ScriptComponent sc = eventEntity.GetComponent<ScriptComponent>();
					if (!ScriptEngine::EntityClassExists(sc.ClassName)) return;
					bool sceneRunning = m_Context->IsRunning();

					if (ImGui::BeginCombo("##PressedMethod", component.PressedEvent.InteractedFunction.c_str()))
					{
						for (const auto& [name, scriptMethod] : ScriptEngine::GetScriptMethodMap(sc.ClassName))
						{
							bool isSelected = component.PressedEvent.InteractedFunction == name;
							if (ImGui::Selectable(name.c_str(), isSelected))
							{
								component.PressedEvent.InteractedFunction = name;
								component.PressedEvent.SetupParams(scriptMethod);
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					// if function selected get/set params
					if (!component.PressedEvent.InteractedFunction.empty())
					{
						int i = 0;
						auto paramType = component.PressedEvent.Params[i]->Field.Type;
						while (i < 8 && paramType != ScriptFieldType::None)
						{
							auto& param = component.PressedEvent.Params[i];
							switch (paramType)
							{
								case ScriptFieldType::Float:
								{
									float data = 0.0f;
									data = param->GetValue<float>();
									if (ImGui::DragFloat(("##PressedEventParam" + std::to_string(i)).c_str(), &data, 0.1f))
										param->SetValue(data); 
									break;
								}
								case ScriptFieldType::Double:
								{
									double data = 0.0;
									data = param->GetValue<double>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_Double, &data, 0.1))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Bool:
								{
									bool data = false;
									data = param->GetValue<bool>();
									if (ImGui::Checkbox(("##PressedEventParam" + std::to_string(i)).c_str(), &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Char:
								{
									char data[2];
									memset(data, 0, sizeof(data));
									data[0] = param->GetValue<char>();
									if (ImGui::InputText(("##PressedEventParam" + std::to_string(i)).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
										param->SetValue(data[0]);
									break;
								}
								case ScriptFieldType::String:
								{
									char data[64];
									memset(data, 0, sizeof(data));
									strcpy_s(data, sizeof(data), param->GetValue<std::string>().c_str());
									if (ImGui::InputText(("##PressedEventParam" + std::to_string(i)).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
										param->SetValue(std::string(data));
									break;
								}
								case ScriptFieldType::SByte:
								{
									int8_t data = 0;
									data = param->GetValue<int8_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S8, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Short:
								{
									int16_t data = 0;
									data = param->GetValue<int16_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S16, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Int:
								{
									int32_t data = 0;
									data = param->GetValue<int32_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S32, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Long:
								{
									int64_t data = 0;
									data = param->GetValue<int64_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S64, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Byte:
								{
									uint8_t data = 0;
									data = param->GetValue<uint8_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U8, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::UShort:
								{
									uint16_t data = 0;
									data = param->GetValue<uint16_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U16, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::UInt:
								{
									uint32_t data = 0;
									data = param->GetValue<uint32_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U32, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::ULong:
								{
									uint64_t data = 0;
									data = param->GetValue<uint64_t>();
									if (ImGui::DragScalar(("##PressedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U64, &data))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Vector2:
								{
									glm::vec2 data = {};
									data = param->GetValue<glm::vec2>();
									if (ImGui::DragFloat2(("##PressedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Vector3:
								{
									glm::vec3 data = {};
									data = param->GetValue<glm::vec3>();
									if (ImGui::DragFloat3(("##PressedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Vector4:
								{
									glm::vec4 data = {};
									data = param->GetValue<glm::vec4>();
									if (ImGui::DragFloat4(("##PressedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
										param->SetValue(data);
									break;
								}
								case ScriptFieldType::Entity:
								case ScriptFieldType::Void:
								case ScriptFieldType::None:
								default:
									FieldTypeUnsupported(paramType);
									break;
							}

							++i;
							paramType = component.PressedEvent.Params[i]->Field.Type;
						}
					}
				}
			}

			ImGui::Text("Released Event");
			{
				bool isValid = component.ReleasedEvent.InteractedEntityID.IsValid();
				std::string entityName = "None";

				Entity eventEntity;
				if (isValid)
				{
					eventEntity = m_Context->GetEntityWithUUID(component.ReleasedEvent.InteractedEntityID);
					entityName = eventEntity.GetName();
				}

				const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				const ImVec2 buttonSize = { 0.0f, lineHeight };
				ImGui::Text("Entity");
				ImGui::SameLine();
				ImGui::Button(entityName.c_str(), buttonSize);

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_HIERARCHY_ENTITY_ITEM"))
					{
						const UUID* entityItemID = (const UUID*)payload->Data;
						component.ReleasedEvent.InteractedEntityID = *entityItemID;
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::Text("Function");
				ImGui::SameLine();

				if (isValid && eventEntity.HasComponent<ScriptComponent>())
				{
					ScriptComponent sc = eventEntity.GetComponent<ScriptComponent>();

					if (ImGui::BeginCombo("##ReleasedMethod", component.ReleasedEvent.InteractedFunction.c_str()))
					{
						for (const auto& [name, scriptMethod] : ScriptEngine::GetScriptMethodMap(sc.ClassName))
						{
							bool isSelected = component.ReleasedEvent.InteractedFunction == name;
							if (ImGui::Selectable(name.c_str(), isSelected))
							{
								component.ReleasedEvent.InteractedFunction = name;
								component.ReleasedEvent.SetupParams(scriptMethod);
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}
				}


				// if function selected get/set params
				if (!component.ReleasedEvent.InteractedFunction.empty())
				{
					int i = 0;
					auto paramType = component.ReleasedEvent.Params[i]->Field.Type;
					while (i < 8 && paramType != ScriptFieldType::None)
					{
						auto& param = component.ReleasedEvent.Params[i];
						switch (paramType)
						{
						case ScriptFieldType::Float:
						{
							float data = 0.0f;
							data = param->GetValue<float>();
							if (ImGui::DragFloat(("##ReleasedEventParam" + std::to_string(i)).c_str(), &data, 0.1f))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Double:
						{
							double data = 0.0;
							data = param->GetValue<double>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_Double, &data, 0.1))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Bool:
						{
							bool data = false;
							data = param->GetValue<bool>();
							if (ImGui::Checkbox(("##ReleasedEventParam" + std::to_string(i)).c_str(), &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Char:
						{
							char data[2];
							memset(data, 0, sizeof(data));
							data[0] = param->GetValue<char>();
							if (ImGui::InputText(("##ReleasedEventParam" + std::to_string(i)).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
								param->SetValue(data[0]);
							break;
						}
						case ScriptFieldType::String:
						{
							char data[64];
							memset(data, 0, sizeof(data));
							strcpy_s(data, sizeof(data), param->GetValue<std::string>().c_str());
							if (ImGui::InputText(("##ReleasedEventParam" + std::to_string(i)).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
								param->SetValue(std::string(data));
							break;
						}
						case ScriptFieldType::SByte:
						{
							int8_t data = 0;
							data = param->GetValue<int8_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S8, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Short:
						{
							int16_t data = 0;
							data = param->GetValue<int16_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S16, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Int:
						{
							int32_t data = 0;
							data = param->GetValue<int32_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S32, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Long:
						{
							int64_t data = 0;
							data = param->GetValue<int64_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_S64, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Byte:
						{
							uint8_t data = 0;
							data = param->GetValue<uint8_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U8, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::UShort:
						{
							uint16_t data = 0;
							data = param->GetValue<uint16_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U16, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::UInt:
						{
							uint32_t data = 0;
							data = param->GetValue<uint32_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U32, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::ULong:
						{
							uint64_t data = 0;
							data = param->GetValue<uint64_t>();
							if (ImGui::DragScalar(("##ReleasedEventParam" + std::to_string(i)).c_str(), ImGuiDataType_U64, &data))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Vector2:
						{
							glm::vec2 data = {};
							data = param->GetValue<glm::vec2>();
							if (ImGui::DragFloat2(("##ReleasedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Vector3:
						{
							glm::vec3 data = {};
							data = param->GetValue<glm::vec3>();
							if (ImGui::DragFloat3(("##ReleasedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Vector4:
						{
							glm::vec4 data = {};
							data = param->GetValue<glm::vec4>();
							if (ImGui::DragFloat4(("##ReleasedEventParam" + std::to_string(i)).c_str(), glm::value_ptr(data), 0.1f))
								param->SetValue(data);
							break;
						}
						case ScriptFieldType::Entity:
						case ScriptFieldType::Void:
						case ScriptFieldType::None:
						default:
							FieldTypeUnsupported(paramType);
							break;
						}

						++i;
						paramType = component.ReleasedEvent.Params[i]->Field.Type;
					}
				}
			}
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [&](auto& component)
		{
			bool sceneRunning = m_Context->IsRunning();
			auto body = (b2Body*)component.RuntimeBody;

			const char* bodyTypeStrings[3] = { "Static", "Dynamic", "Kinematic" };
			int bodyTypeIndex = sceneRunning ? (int)Engine::Utils::Box2DBodyTypeToRigidbody2DType(body->GetType()) : (int)component.Type;
			const char* currentBodyTypeString = bodyTypeStrings[bodyTypeIndex];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; ++i)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						Rigidbody2DComponent::BodyType selectedBody = (Rigidbody2DComponent::BodyType)i;
						if (sceneRunning)
							body->SetType(Engine::Utils::Rigidbody2DTypeToBox2DBodyType(selectedBody));
						else
							component.Type = selectedBody;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);

			const char* smoothTypeStrings[3] = { "None", "Interpolation", "Extrapolation" };
			const char* currentsmoothTypeString = smoothTypeStrings[(int)component.Smoothing];

			if (ImGui::BeginCombo("Smoothing", currentsmoothTypeString))
			{
				for (int i = 0; i < 3; ++i)
				{
					bool isSelected = currentsmoothTypeString == smoothTypeStrings[i];
					if (ImGui::Selectable(smoothTypeStrings[i], isSelected))
						component.Smoothing = (Rigidbody2DComponent::SmoothingType)i;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		});
		
		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f);
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size), 0.1f);

			ImGui::Checkbox("Trigger", &component.Sensor);

			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, std::numeric_limits<float>::infinity());
		});
		
		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f);
			ImGui::DragFloat("Radius", &component.Radius, 0.1f);

			ImGui::Checkbox("Trigger", &component.Sensor);

			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, std::numeric_limits<float>::infinity());
		});

		DrawComponent<ScriptComponent>("Script Component", entity, [&](auto& component)
		{
			bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);
			bool sceneRunning = m_Context->IsRunning();

			ImGui::Text("Script");
			ImGui::SameLine();

			if (sceneRunning)
			{
				ImGui::Text(component.ClassName.c_str());
			}
			else
			{
				const std::string& currentScriptClassName = scriptClassExists ? component.ClassName : "";
				if (ImGui::BeginCombo("##Script", currentScriptClassName.c_str()))
				{
					for (const auto& [name, scriptClass] : ScriptEngine::GetEntityClasses())
					{
						bool isSelected = currentScriptClassName == name;
						if (ImGui::Selectable(name.c_str(), isSelected))
							component.ClassName = name;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			}
			
			if (!scriptClassExists) return;

			// Fields
			Ref<ScriptInstance> scriptInstance = sceneRunning ? ScriptEngine::GetEntityInstance(entity) : nullptr;
			auto& fields = sceneRunning ? scriptInstance->GetScriptClass()->GetScriptFields() : ScriptEngine::GetEntityClass(component.ClassName)->GetScriptFields();
			auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

			for (auto& [name, field] : fields)
			{
				if (!field.IsPublic())
					continue;

				ImGui::Text(name.c_str());
				ImGui::SameLine();

				bool fieldExists = entityFields.find(name) != entityFields.end(); // TODO make entity fields exists func

				SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

				switch (field.Type)
				{
					case ScriptFieldType::Float:
					{
						float data = 0.0f;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, float);
						if (ImGui::DragFloat(("##" + name).c_str(), &data, 0.1f))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Double:
					{
						double data = 0.0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, double);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_Double, &data, 0.1))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Bool:
					{
						bool data = false;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, bool);
						if (ImGui::Checkbox(("##" + name).c_str(), &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Char:
					{
						char data[2];
						memset(data, 0, sizeof(data));
						if (sceneRunning)
						{
							data[0] = scriptInstance->GetFieldValue<char>(name);
						}
						else if (fieldExists)
						{
							data[0] = scriptField.GetValue<char>();
						}
						else
						{
							data[0] = ScriptEngine::GetDefaultScriptFieldMap(component.ClassName).at(name).GetValue<char>();
							scriptField.SetValue(data[0]);
						}

						if (ImGui::InputText(("##" + name).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data[0]) : scriptField.SetValue(data[0]);
						break;
					}
					case ScriptFieldType::String:
					{
						char data[64];
						memset(data, 0, sizeof(data));
						if (sceneRunning)
						{
							strcpy_s(data, sizeof(data), scriptInstance->GetFieldValue<std::string>(name).c_str());
						}
						else if (fieldExists)
						{
							strcpy_s(data, sizeof(data), scriptField.GetValue<std::string>().c_str());
						}
						else
						{
							std::string strVal = ScriptEngine::GetDefaultScriptFieldMap(component.ClassName).at(name).GetValue<std::string>();
							strcpy_s(data, sizeof(data), strVal.c_str());
							scriptField.SetValue(strVal);
						}

						if (ImGui::InputText(("##" + name).c_str(), data, sizeof(data), ImGuiInputTextFlags_EnterReturnsTrue))
							sceneRunning ? scriptInstance->SetFieldValue(name, &std::string(data)) : scriptField.SetValue(std::string(data));
						break;
					}
					case ScriptFieldType::SByte:
					{
						int8_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int8_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S8, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Short:
					{
						int16_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int16_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S16, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Int:
					{
						int32_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int32_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S32, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Long:
					{
						int64_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int64_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S64, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Byte:
					{
						uint8_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint8_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U8, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &(uint8_t)data) : scriptField.SetValue((uint8_t)data);
						break;
					}
					case ScriptFieldType::UShort:
					{
						uint16_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint16_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U16, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::UInt:
					{
						uint32_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint32_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U32, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::ULong:
					{
						uint64_t data = 0;
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint64_t);
						if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U64, &data))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Vector2:
					{
						glm::vec2 data = {};
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec2);
						if (ImGui::DragFloat2(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Vector3:
					{
						glm::vec3 data = {};
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec3);
						if (ImGui::DragFloat3(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Vector4:
					{
						glm::vec4 data = {};
						GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec4);
						if (ImGui::DragFloat4(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
							sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
						break;
					}
					case ScriptFieldType::Entity:
					case ScriptFieldType::Void:
					case ScriptFieldType::None:
					default:
						FieldTypeUnsupported(field.Type);
						break;
				}
			}
		});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		Entity selectedEntity = GetSelectedEntity();
		if (!selectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				selectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void SceneHierarchyPanel::SavePrefabAs()
	{
		std::filesystem::path filepath = FileDialogs::SaveFile("Prefab (*.prefab)\0*.prefab\0");

		PrefabSerializer serializer(GetSelectedEntity(), m_Context);
		serializer.Serialize(filepath);
	}

	void SceneHierarchyPanel::CreateFromPrefab()
	{
		std::string filepath = FileDialogs::OpenFile("Prefab (*.prefab)\0*.prefab\0");
		if (filepath.empty())
			return;

		auto relativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
		CreateFromPrefab(relativePath);
	}

	void SceneHierarchyPanel::CreateFromPrefab(const std::filesystem::path& filepath)
	{
		if (filepath.extension().string() != ".prefab")
		{
			ENGINE_CORE_WARN("Could not load {0} - not a prefab file", filepath.filename().string());
			return;
		}

		PrefabSerializer serializer(GetSelectedEntity(), m_Context);
		serializer.Deserialize(filepath);
	}

	void SceneHierarchyPanel::CreateChildEntity()
	{
		if (!m_SelectionContext.IsValid())
			return;

		Entity selectedEntity = GetSelectedEntity();
		Entity childEntity = m_Context->CreateEntity("Child");
		selectedEntity.AddChild(childEntity);
	}

}
