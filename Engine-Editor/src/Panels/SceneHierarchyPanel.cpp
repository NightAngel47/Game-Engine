#include "SceneHierarchyPanel.h"

#include "Engine/UI/UI.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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

				switch (field.Type)
				{
				case ScriptFieldType::Float:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					float data = 0.0f;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, float);

					if (ImGui::DragFloat(("##" + name).c_str(), &data, 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Double:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					double data = 0.0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, double);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_Double, &data, 0.1))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Bool:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					bool data = false;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, bool);

					if (ImGui::Checkbox(("##" + name).c_str(), &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Char:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);
					
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
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

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
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					int8_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int8_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S8, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Short:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					int16_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int16_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S16, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Int:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					int32_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int32_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S32, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Long:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					int64_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, int64_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_S64, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Byte:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					uint8_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint8_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U8, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &(uint8_t)data) : scriptField.SetValue((uint8_t)data);
					break;
				}
				case ScriptFieldType::UShort:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					uint16_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint16_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U16, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::UInt:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					uint32_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint32_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U32, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::ULong:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					uint64_t data = 0;
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, uint64_t);

					if (ImGui::DragScalar(("##" + name).c_str(), ImGuiDataType_U64, &data))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector2:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec2 data = {};
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec2);

					if (ImGui::DragFloat2(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector3:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec3 data = {};
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec3);

					if (ImGui::DragFloat3(("##" + name).c_str(), glm::value_ptr(data), 0.1f))
						sceneRunning ? scriptInstance->SetFieldValue(name, &data) : scriptField.SetValue(data);
					break;
				}
				case ScriptFieldType::Vector4:
				{
					SCRIPT_FIELD_INSTANCE(name, field, sceneRunning, fieldExists, entityFields);

					glm::vec4 data = {};
					GET_FEILD_VALUE(name, data, scriptInstance, scriptField, sceneRunning, fieldExists, component.ClassName, glm::vec4);

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
