#include "SceneHierarchyPanel.h"

#include "Engine/UI/UI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <cstring>

namespace Engine
{
#define SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields)					\
	ScriptFieldInstance& scriptField = fieldExists ? entityFields.at(name) : entityFields[name];	\
	if (!sceneRunning && !fieldExists)																\
		scriptField.Field = field;																	\

	static void FieldTypeUnsupported(const ScriptFieldType& fieldType)
	{
		const char* typeName = Utils::ScriptFieldTypeToString(fieldType);
		ImGui::Text(typeName);
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{

			m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					m_Context->CreateEntity();
				}
				else if (ImGui::MenuItem("Create Sprite"))
				{
					m_Context->CreateEntity("Sprite").AddComponent<SpriteRendererComponent>();
				}
				else if (ImGui::MenuItem("Create Circle"))
				{
					m_Context->CreateEntity("Circle").AddComponent<CircleRendererComponent>();
				}
				else if (ImGui::MenuItem("Create Camera"))
				{
					m_Context->CreateEntity("Camera").AddComponent<CameraComponent>();
				}

				ImGui::EndPopup();
			}
		}
		
		ImGui::End();

		ImGui::Begin("Properties");
		if(m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		
		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = (m_SelectionContext == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if(ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if(opened)
		{
			ImGui::TreePop();
		}

		if(entityDeleted)
		{
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
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
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if(ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
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
		});
		
		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.0001f, 0.0f, 1.0f);
		});
		
		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[3] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; ++i)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						component.Type = (Rigidbody2DComponent::BodyType)i;

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

			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, std::numeric_limits<float>::infinity());
		});
		
		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset), 0.1f);
			ImGui::DragFloat("Radius", &component.Radius, 0.1f);

			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, std::numeric_limits<float>::infinity());
		});

		DrawComponent<ScriptComponent>("Script Component", entity, [&](auto& component)
		{
			bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

			UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

			if (ImGui::InputText("ScriptName", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				component.ClassName = std::string(buffer);
				return;
			}

			if (!scriptClassExists) return;

			// Fields
			bool sceneRunning = m_Context->IsRunning();

			Ref<ScriptInstance> scriptInstance = sceneRunning ? ScriptEngine::GetEntityInstance(entity) : nullptr;
			std::unordered_map<std::string, ScriptField> fields = sceneRunning ? scriptInstance->GetScriptClass()->GetScriptFields() : ScriptEngine::GetEntityClass(component.ClassName)->GetScriptFields();
			auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

			for (auto& [name, field] : fields)
			{
				if (!field.IsPublic())
					continue;

				ImGui::Text(name.c_str());
				ImGui::SameLine();

				bool fieldExists = entityFields.find(name) != entityFields.end(); // TODO make entity fields exists func

				switch (field.Type)
				{
				case ScriptFieldType::Float:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					float data = sceneRunning ? scriptInstance->GetFieldValue<float>(name) : fieldExists ? scriptField.GetValue<float>() : 0.0f; // default 0.0f TODO read script default value
					if (ImGui::DragFloat(("##" + name).c_str(), &data, 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Double:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					double data = sceneRunning ? scriptInstance->GetFieldValue<double>(name) : fieldExists ? scriptField.GetValue<double>() : 0.0; // default 0.0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_Double, &data, 0.1))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Bool:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					bool data = sceneRunning ? scriptInstance->GetFieldValue <bool> (name) : fieldExists ? scriptField.GetValue<bool>() : false; // default false TODO read script default value
					if (ImGui::Checkbox(("##" + name).c_str(), &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Char:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);
					
					char buffer[2];
					memset(buffer, 0, sizeof(buffer));
					buffer[0] = sceneRunning ? scriptInstance->GetFieldValue<char>(name) : fieldExists ? scriptField.GetValue<char>() : ' '; // default ' ' TODO read script default value
					if (ImGui::InputText(("##" + name).c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
						sceneRunning ? scriptInstance->SetFieldValue(name, &buffer[0]) : scriptField.SetValue(buffer[0]);
					break;
				}
				case ScriptFieldType::String:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					char buffer[64];
					memset(buffer, 0, sizeof(buffer));
					strcpy_s(buffer, sizeof(buffer), sceneRunning ? scriptInstance->GetFieldValue<std::string>(name).c_str() : fieldExists ? scriptField.GetValue<std::string>().c_str() : ""); // default "" TODO read script default value
					if (ImGui::InputText(("##" + name).c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
						sceneRunning ? scriptInstance->SetFieldValue(name, &std::string(buffer)) : scriptField.SetValue(std::string(buffer));
					break;
				}
				case ScriptFieldType::SByte:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					int8_t data = sceneRunning ? scriptInstance->GetFieldValue<int8_t>(name) : fieldExists ? scriptField.GetValue<int8_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S8, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Short:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					int16_t data = sceneRunning ? scriptInstance->GetFieldValue<int16_t>(name) : fieldExists ? scriptField.GetValue<int16_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S16, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Int:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					int32_t data = sceneRunning ? scriptInstance->GetFieldValue<int32_t>(name) : fieldExists ? scriptField.GetValue<int32_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S32, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Long:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					int64_t data = sceneRunning ? scriptInstance->GetFieldValue<int64_t>(name) : fieldExists ? scriptField.GetValue<int64_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S64, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Byte:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					uint8_t data = sceneRunning ? scriptInstance->GetFieldValue<uint8_t>(name) : fieldExists ? scriptField.GetValue<uint8_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U8, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &(uint8_t)data) : scriptField.SetValue((uint8_t)data);
					break;
				}
				case ScriptFieldType::UShort:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					uint16_t data = sceneRunning ? scriptInstance->GetFieldValue<uint16_t>(name) : fieldExists ? scriptField.GetValue<uint16_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U16, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::UInt:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					uint32_t data = sceneRunning ? scriptInstance->GetFieldValue<uint32_t>(name) : fieldExists ? scriptField.GetValue<uint32_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U32, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::ULong:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					uint64_t data = sceneRunning ? scriptInstance->GetFieldValue<uint64_t>(name) : fieldExists ? scriptField.GetValue<uint64_t>() : 0; // default 0 TODO read script default value
					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U64, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector2:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec2 data = sceneRunning ? scriptInstance->GetFieldValue<glm::vec2>(name) : fieldExists ? scriptField.GetValue<glm::vec2>() : glm::vec2{}; // default {} TODO read script default value
					if (ImGui::DragFloat2(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector3:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec3 data = sceneRunning ? scriptInstance->GetFieldValue<glm::vec3>(name) : fieldExists ? scriptField.GetValue<glm::vec3>() : glm::vec3{}; // default {} TODO read script default value
					if (ImGui::DragFloat3(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector4:
				{
					SETUP_SCRIPT_FIELD(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec4 data = sceneRunning ? scriptInstance->GetFieldValue<glm::vec4>(name) : fieldExists ? scriptField.GetValue<glm::vec4>() : glm::vec4{}; // default {} TODO read script default value
					if (ImGui::DragFloat4(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Entity:
					FieldTypeUnsupported(field.Type);
					break;
				case ScriptFieldType::None:
				default:
					FieldTypeUnsupported(field.Type);
					break;
				}
			}
		});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}
