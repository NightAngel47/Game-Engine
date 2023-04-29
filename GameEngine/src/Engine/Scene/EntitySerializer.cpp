#include "enginepch.h"
#include "Engine/Scene/EntitySerializer.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Physics/Physics2D.h"

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Engine::UUID>
	{
		static Node encode(const Engine::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Engine::UUID& uuid)
		{
			uuid = node[0].as<uint64_t>();
			return true;
		}
	};
}

namespace Engine
{
#define WRITE_SCRIPT_FIELD(FieldType, Type)			\
	case ScriptFieldType::FieldType:				\
		out << scriptField.GetValue<Type>();		\
		break;										\

#define READ_SCRIPT_FIELD(FieldType, Type)			\
	case ScriptFieldType::FieldType:				\
	{												\
		Type data = scriptField["Data"].as<Type>();	\
		fieldInstance.SetValue(data);				\
		break;										\
	}												\

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	void EntitySerializer::Serialize(YAML::Emitter& out)
	{
		ENGINE_CORE_ASSERT(m_Entity.HasComponent<IDComponent>(), "Entity must have UUID!");

		out << YAML::BeginMap; // Entity
		UUID uuid = m_Entity.GetUUID();
		out << YAML::Key << "Entity" << YAML::Value << uuid;

		if (m_Entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			std::string& name = m_Entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << name;
			ENGINE_CORE_TRACE("Serializing entity with ID = {0}, name {1}", uuid, name);

			out << YAML::EndMap; // TagComponent
		}

		if (m_Entity.HasComponent<RelationshipComponent>())
		{
			out << YAML::Key << "RelationshipComponent";
			out << YAML::BeginMap; // RelationshipComponent

			auto& relationshipComponent = m_Entity.GetComponent<RelationshipComponent>();
			out << YAML::Key << "ChildrenCount" << YAML::Value << relationshipComponent.ChildrenCount;
			out << YAML::Key << "FirstChild" << YAML::Value << relationshipComponent.FirstChild;
			out << YAML::Key << "NextChild" << YAML::Value << relationshipComponent.NextChild;
			out << YAML::Key << "PrevChild" << YAML::Value << relationshipComponent.PrevChild;
			out << YAML::Key << "Parent" << YAML::Value << relationshipComponent.Parent;

			out << YAML::EndMap; // RelationshipComponent
		}

		if (m_Entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transformComponent = m_Entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transformComponent.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (m_Entity.HasComponent<UILayoutComponent>())
		{
			out << YAML::Key << "UILayoutComponent";
			out << YAML::BeginMap; // UILayoutComponent

			auto& uiLayoutComponent = m_Entity.GetComponent<UILayoutComponent>();
			out << YAML::Key << "Size" << YAML::Value << uiLayoutComponent.Size;
			out << YAML::Key << "AnchorMin" << YAML::Value << uiLayoutComponent.AnchorMin;
			out << YAML::Key << "AnchorMax" << YAML::Value << uiLayoutComponent.AnchorMax;

			out << YAML::EndMap; // UILayoutComponent
		}

		if (m_Entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = m_Entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera";
			out << YAML::BeginMap; // Camera

			out << YAML::Key << "ProjectionType" << YAML::Value << Utils::SceneCameraProjectionTypeToString(camera.GetProjectionType());

			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();

			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

			out << YAML::EndMap; // Camera

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap; // CameraComponent
		}

		if (m_Entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = m_Entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "Path" << YAML::Value << spriteRendererComponent.Path.string();
			out << YAML::Key << "Tiling" << YAML::Value << spriteRendererComponent.Tiling;

			out << YAML::Key << "IsSubTexture" << YAML::Value << spriteRendererComponent.IsSubTexture;
			out << YAML::Key << "SubCoords" << YAML::Value << spriteRendererComponent.SubCoords;
			out << YAML::Key << "SubCellSize" << YAML::Value << spriteRendererComponent.SubCellSize;
			out << YAML::Key << "SubSpriteSize" << YAML::Value << spriteRendererComponent.SubSpriteSize;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (m_Entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComponent = m_Entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Radius" << YAML::Value << circleRendererComponent.Radius;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (m_Entity.HasComponent<TextRendererComponent>())
		{
			out << YAML::Key << "TextRendererComponent";
			out << YAML::BeginMap; // TextRendererComponent

			auto& textRendererComponent = m_Entity.GetComponent<TextRendererComponent>();
			out << YAML::Key << "TextString" << YAML::Value << textRendererComponent.TextString;
			//out << YAML::Key << "FontAsset" << YAML::Value << textRendererComponent.FontAsset; // TODO
			out << YAML::Key << "Color" << YAML::Value << textRendererComponent.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textRendererComponent.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textRendererComponent.LineSpacing;

			out << YAML::EndMap; // TextRendererComponent
		}

		if (m_Entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent

			auto& scriptComponent = m_Entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

			// Fields
			const auto& scriptFields = ScriptEngine::GetEntityClasses().at(scriptComponent.ClassName)->GetScriptFields();
			if (scriptFields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(m_Entity);
				out << YAML::BeginSeq;
				for (auto const& [name, field] : scriptFields)
				{
					if (entityFields.find(name) == entityFields.end())
						continue;

					out << YAML::BeginMap; // ScriptField
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(field.Type);

					out << YAML::Key << "Data" << YAML::Value;
					ScriptFieldInstance& scriptField = entityFields.at(name);

					switch (field.Type)
					{
						WRITE_SCRIPT_FIELD(Float, float);
						WRITE_SCRIPT_FIELD(Double, double);
						WRITE_SCRIPT_FIELD(Bool, bool);
						WRITE_SCRIPT_FIELD(Char, char);
						WRITE_SCRIPT_FIELD(String, std::string);
						WRITE_SCRIPT_FIELD(SByte, int8_t);
						WRITE_SCRIPT_FIELD(Short, int16_t);
						WRITE_SCRIPT_FIELD(Int, int32_t);
						WRITE_SCRIPT_FIELD(Long, int64_t);
						WRITE_SCRIPT_FIELD(Byte, uint16_t); // upcasting uint8_t to unint16_t to fix yaml-cpp encode/decode
						WRITE_SCRIPT_FIELD(UShort, uint16_t);
						WRITE_SCRIPT_FIELD(UInt, uint32_t);
						WRITE_SCRIPT_FIELD(ULong, uint64_t);
						WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
						WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
						WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
						WRITE_SCRIPT_FIELD(Entity, UUID);
					default:
						ENGINE_CORE_ERROR("Script Field Type {} does not support serialization!", Utils::ScriptFieldTypeToString(field.Type));
					}

					out << YAML::EndMap; // ScriptFields
				}

				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}

		if (m_Entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rigidbody2DComponent = m_Entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "Type" << YAML::Value << Utils::Rigidbody2DBodyTypeToString(rigidbody2DComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;
			out << YAML::Key << "Smoothing" << YAML::Value << Utils::Rigidbody2DSmoothingTypeToString(rigidbody2DComponent.Smoothing);

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (m_Entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& boxCollider2DComponent = m_Entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
			out << YAML::Key << "Sensor" << YAML::Value << boxCollider2DComponent.Sensor;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (m_Entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& circleCollider2DComponent = m_Entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circleCollider2DComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circleCollider2DComponent.Radius;
			out << YAML::Key << "Sensor" << YAML::Value << circleCollider2DComponent.Sensor;
			out << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (m_Entity.HasComponent<UIButtonComponent>())
		{
			out << YAML::Key << "UIButtonComponent";
			out << YAML::BeginMap; // UIButtonComponent

			auto& uiButtonComponent = m_Entity.GetComponent<UIButtonComponent>();
			out << YAML::Key << "NormalColor" << YAML::Value << uiButtonComponent.NormalColor;
			out << YAML::Key << "HoverColor" << YAML::Value << uiButtonComponent.HoverColor;
			out << YAML::Key << "PressedColor" << YAML::Value << uiButtonComponent.PressedColor;
			out << YAML::Key << "DisabledColor" << YAML::Value << uiButtonComponent.DisabledColor;

			out << YAML::Key << "PressedEvent.InteractedEntityID" << YAML::Value << uiButtonComponent.PressedEvent.InteractedEntityID;
			out << YAML::Key << "PressedEvent.InteractedFunction" << YAML::Value << uiButtonComponent.PressedEvent.InteractedFunction;
			out << YAML::Key << "PressedEvent.Params" << YAML::Value;
			auto& entityFields = ScriptEngine::GetScriptFieldMap(m_Entity);
			out << YAML::BeginSeq;
			int i = 0;
			for (auto& Param : uiButtonComponent.PressedEvent.Params)
			{
				if (Param == nullptr || Param->Field.Type == ScriptFieldType::None)
					continue;

				ScriptFieldInstance& scriptField = *Param;
				out << YAML::BeginMap; // Param
				//out << YAML::Key << "Name" << YAML::Value << "PressedEvent.Params[" + std::to_string(i) + "]";
				out << YAML::Key << "Type" << YAML::Value << Utils::ScriptFieldTypeToString(scriptField.Field.Type);

				out << YAML::Key << "Data" << YAML::Value;

				switch (scriptField.Field.Type)
				{
					WRITE_SCRIPT_FIELD(Float, float);
					WRITE_SCRIPT_FIELD(Double, double);
					WRITE_SCRIPT_FIELD(Bool, bool);
					WRITE_SCRIPT_FIELD(Char, char);
					WRITE_SCRIPT_FIELD(String, std::string);
					WRITE_SCRIPT_FIELD(SByte, int8_t);
					WRITE_SCRIPT_FIELD(Short, int16_t);
					WRITE_SCRIPT_FIELD(Int, int32_t);
					WRITE_SCRIPT_FIELD(Long, int64_t);
					WRITE_SCRIPT_FIELD(Byte, uint16_t); // upcasting uint8_t to unint16_t to fix yaml-cpp encode/decode
					WRITE_SCRIPT_FIELD(UShort, uint16_t);
					WRITE_SCRIPT_FIELD(UInt, uint32_t);
					WRITE_SCRIPT_FIELD(ULong, uint64_t);
					WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
					WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
					WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
					WRITE_SCRIPT_FIELD(Entity, UUID);
				default:
					ENGINE_CORE_ERROR("Script Field Type {} does not support serialization!", Utils::ScriptFieldTypeToString(scriptField.Field.Type));
				}

				out << YAML::EndMap; // ScriptFields

				++i;
			}
			out << YAML::EndSeq;

			out << YAML::Key << "ReleasedEvent.InteractedEntityID" << YAML::Value << uiButtonComponent.ReleasedEvent.InteractedEntityID;
			out << YAML::Key << "ReleasedEvent.InteractedFunction" << YAML::Value << uiButtonComponent.ReleasedEvent.InteractedFunction;

			out << YAML::EndMap; // UIButtonComponent
		}

		auto& relationship = m_Entity.GetComponent<RelationshipComponent>();
		if (relationship.HasChildren())
		{
			Entity parentEntity = m_Entity;
			out << YAML::Key << "ChildEntities" << YAML::Value << YAML::BeginSeq;

			UUID childIterator = relationship.FirstChild;
			for (uint64_t i = 0; i < relationship.ChildrenCount; ++i)
			{
				m_Entity = m_Scene->GetEntityWithUUID(childIterator);
				Serialize(out);

				childIterator = m_Entity.GetComponent<RelationshipComponent>().NextChild;
				if (!childIterator.IsValid())
					break;
			}

			m_Entity = parentEntity;

			out << YAML::EndSeq; // ChildEntities
		}

		out << YAML::EndMap; // Entity
	}

	Engine::Entity EntitySerializer::Deserialize(YAML::Node& entity, bool isPrefab)
	{
		UUID uuid = isPrefab ? UUID() : entity["Entity"].as<uint64_t>();

		std::string name;
		auto tagComponent = entity["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		ENGINE_CORE_TRACE("Deserializing entity {0}: {1}", uuid, name);
		m_Entity = m_Scene->CreateEntityWithUUID(uuid, name);

		auto transformComponent = entity["TransformComponent"];
		if (transformComponent)
		{
			auto& tc = m_Entity.GetComponent<TransformComponent>();
			tc.Position = transformComponent["Position"].as<glm::vec3>();
			tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
			tc.Scale = transformComponent["Scale"].as<glm::vec3>();
		}

		auto relationshipComponent = entity["RelationshipComponent"];
		if (relationshipComponent)
		{
			auto& relationship = m_Entity.GetComponent<RelationshipComponent>();
			relationship.ChildrenCount = relationshipComponent["ChildrenCount"].as<uint64_t>();
			relationship.FirstChild = relationshipComponent["FirstChild"].as<uint64_t>();
			relationship.NextChild = relationshipComponent["NextChild"].as<uint64_t>();
			relationship.PrevChild = relationshipComponent["PrevChild"].as<uint64_t>();
			relationship.Parent = relationshipComponent["Parent"].as<uint64_t>();
		}

		auto uiLayoutComponent = entity["UILayoutComponent"];
		if (uiLayoutComponent)
		{
			auto& ui = m_Entity.AddComponent<UILayoutComponent>();
			ui.Size = uiLayoutComponent["Size"].as<glm::vec2>();
			ui.AnchorMin = uiLayoutComponent["AnchorMin"].as<glm::vec2>();
			ui.AnchorMax = uiLayoutComponent["AnchorMax"].as<glm::vec2>();
		}

		auto cameraComponent = entity["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = m_Entity.AddComponent<CameraComponent>();
			auto& camera = cc.Camera;

			auto& cameraProps = cameraComponent["Camera"];

			camera.SetProjectionType(Utils::SceneCameraProjectionTypeFromString(cameraProps["ProjectionType"].as<std::string>()));

			camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
			camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
			camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

			camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
			camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
			camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

			cc.Primary = cameraComponent["Primary"].as<bool>();
			cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
		}

		auto spriteRendererComponent = entity["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& spriteRenderer = m_Entity.AddComponent<SpriteRendererComponent>();
			spriteRenderer.Color = spriteRendererComponent["Color"].as<glm::vec4>();
			spriteRenderer.Path = spriteRendererComponent["Path"].as<std::string>();
			spriteRenderer.Tiling = spriteRendererComponent["Tiling"].as<float>();

			spriteRenderer.IsSubTexture = spriteRendererComponent["IsSubTexture"].as<bool>();
			spriteRenderer.SubCoords = spriteRendererComponent["SubCoords"].as<glm::vec2>();
			spriteRenderer.SubCellSize = spriteRendererComponent["SubCellSize"].as<glm::vec2>();
			spriteRenderer.SubSpriteSize = spriteRendererComponent["SubSpriteSize"].as<glm::vec2>();

			spriteRenderer.LoadTexture(spriteRenderer.Path);
		}

		auto circleRendererComponent = entity["CircleRendererComponent"];
		if (circleRendererComponent)
		{
			auto& circleRenderer = m_Entity.AddComponent<CircleRendererComponent>();
			circleRenderer.Color = circleRendererComponent["Color"].as<glm::vec4>();
			circleRenderer.Radius = circleRendererComponent["Radius"].as<float>();
			circleRenderer.Thickness = circleRendererComponent["Thickness"].as<float>();
			circleRenderer.Fade = circleRendererComponent["Fade"].as<float>();
		}

		auto textRendererComponent = entity["TextRendererComponent"];
		if (textRendererComponent)
		{
			auto& textRenderer = m_Entity.AddComponent<TextRendererComponent>();
			textRenderer.TextString = textRendererComponent["TextString"].as<std::string>();
			// textRenderer.FontAsset = font; // TODO
			textRenderer.Color = textRendererComponent["Color"].as<glm::vec4>();
			textRenderer.Kerning = textRendererComponent["Kerning"].as<float>();
			textRenderer.LineSpacing = textRendererComponent["LineSpacing"].as<float>();
		}

		auto scriptComponent = entity["ScriptComponent"];
		if (scriptComponent)
		{
			auto& sc = m_Entity.AddComponent<ScriptComponent>();
			sc.ClassName = scriptComponent["ClassName"].as<std::string>();

			auto scriptFields = scriptComponent["ScriptFields"];
			if (scriptFields)
			{
				Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
				if (entityClass)
				{
					const auto& fields = entityClass->GetScriptFields();
					auto& entityFields = ScriptEngine::GetScriptFieldMap(m_Entity);

					for (auto scriptField : scriptFields)
					{
						std::string name = scriptField["Name"].as<std::string>();
						std::string typeString = scriptField["Type"].as<std::string>();
						ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

						ScriptFieldInstance& fieldInstance = entityFields[name];

						//ENGINE_CORE_ASSERT(fields.find(name) != fields.end()); // extra field in saved file

						if (fields.find(name) == fields.end())
							continue;

						fieldInstance.Field = fields.at(name);

						switch (type)
						{
							READ_SCRIPT_FIELD(Float, float);
							READ_SCRIPT_FIELD(Double, double);
							READ_SCRIPT_FIELD(Bool, bool);
							READ_SCRIPT_FIELD(Char, char);
							READ_SCRIPT_FIELD(String, std::string);
							READ_SCRIPT_FIELD(SByte, int8_t);
							READ_SCRIPT_FIELD(Short, int16_t);
							READ_SCRIPT_FIELD(Int, int32_t);
							READ_SCRIPT_FIELD(Long, int64_t);
							READ_SCRIPT_FIELD(Byte, uint16_t); // upcasting uint8_t to unint16_t to fix yaml-cpp encode/decode
							READ_SCRIPT_FIELD(UShort, uint16_t);
							READ_SCRIPT_FIELD(UInt, uint32_t);
							READ_SCRIPT_FIELD(ULong, uint64_t);
							READ_SCRIPT_FIELD(Vector2, glm::vec2);
							READ_SCRIPT_FIELD(Vector3, glm::vec3);
							READ_SCRIPT_FIELD(Vector4, glm::vec4);
							READ_SCRIPT_FIELD(Entity, UUID);
						default:
							ENGINE_CORE_ERROR("Script Field Type {} does not support deserialization!", Utils::ScriptFieldTypeToString(type));
						}
					}
				}
				else
				{
					ENGINE_CORE_ERROR("Entity Class does not exists for given script component!");
				}
			}
		}

		auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
		if (rigidbody2DComponent)
		{
			auto& rigidbody2D = m_Entity.AddComponent<Rigidbody2DComponent>();
			rigidbody2D.Type = Utils::Rigidbody2DBodyTypeFromString(rigidbody2DComponent["Type"].as<std::string>());
			rigidbody2D.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			rigidbody2D.Smoothing = Utils::Rigidbody2DSmoothingTypeFromString(rigidbody2DComponent["Smoothing"].as<std::string>());
		}

		auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
		if (boxCollider2DComponent)
		{
			auto& boxCollider2D = m_Entity.AddComponent<BoxCollider2DComponent>();
			boxCollider2D.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
			boxCollider2D.Size = boxCollider2DComponent["Size"].as<glm::vec2>();

			boxCollider2D.Sensor = boxCollider2DComponent["Sensor"].as<bool>();

			boxCollider2D.Density = boxCollider2DComponent["Density"].as<float>();
			boxCollider2D.Friction = boxCollider2DComponent["Friction"].as<float>();
			boxCollider2D.Restitution = boxCollider2DComponent["Restitution"].as<float>();
			boxCollider2D.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
		}

		auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
		if (circleCollider2DComponent)
		{
			auto& circleCollider2D = m_Entity.AddComponent<CircleCollider2DComponent>();
			circleCollider2D.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
			circleCollider2D.Radius = circleCollider2DComponent["Radius"].as<float>();

			circleCollider2D.Sensor = circleCollider2DComponent["Sensor"].as<bool>();

			circleCollider2D.Density = circleCollider2DComponent["Density"].as<float>();
			circleCollider2D.Friction = circleCollider2DComponent["Friction"].as<float>();
			circleCollider2D.Restitution = circleCollider2DComponent["Restitution"].as<float>();
			circleCollider2D.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
		}

		auto uiButtonComponent = entity["UIButtonComponent"];
		if (uiButtonComponent)
		{
			auto& uiButton = m_Entity.AddComponent<UIButtonComponent>();
			uiButton.NormalColor = uiButtonComponent["NormalColor"].as<glm::vec4>();
			uiButton.HoverColor = uiButtonComponent["HoverColor"].as<glm::vec4>();
			uiButton.PressedColor = uiButtonComponent["PressedColor"].as<glm::vec4>();
			uiButton.DisabledColor = uiButtonComponent["DisabledColor"].as<glm::vec4>();

			uiButton.PressedEvent.InteractedEntityID = uiButtonComponent["PressedEvent.InteractedEntityID"].as<uint64_t>();
			uiButton.PressedEvent.InteractedFunction = uiButtonComponent["PressedEvent.InteractedFunction"].as<std::string>();

			auto scriptFields = uiButtonComponent["PressedEvent.Params"];
			if (scriptFields)
			{
				int i = 0;
				for (auto scriptField : scriptFields)
				{
					//std::string name = scriptField["Name"].as<std::string>();
					std::string typeString = scriptField["Type"].as<std::string>();
					ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

					if (type == ScriptFieldType::None)
						continue;

					uiButton.PressedEvent.Params[i] = new ScriptFieldInstance();
					uiButton.PressedEvent.Params[i]->Field = { type };

					ScriptFieldInstance& fieldInstance = *uiButton.PressedEvent.Params[i];
					switch (type)
					{
						READ_SCRIPT_FIELD(Float, float);
						READ_SCRIPT_FIELD(Double, double);
						READ_SCRIPT_FIELD(Bool, bool);
						READ_SCRIPT_FIELD(Char, char);
						READ_SCRIPT_FIELD(String, std::string);
						READ_SCRIPT_FIELD(SByte, int8_t);
						READ_SCRIPT_FIELD(Short, int16_t);
						READ_SCRIPT_FIELD(Int, int32_t);
						READ_SCRIPT_FIELD(Long, int64_t);
						READ_SCRIPT_FIELD(Byte, uint16_t); // upcasting uint8_t to unint16_t to fix yaml-cpp encode/decode
						READ_SCRIPT_FIELD(UShort, uint16_t);
						READ_SCRIPT_FIELD(UInt, uint32_t);
						READ_SCRIPT_FIELD(ULong, uint64_t);
						READ_SCRIPT_FIELD(Vector2, glm::vec2);
						READ_SCRIPT_FIELD(Vector3, glm::vec3);
						READ_SCRIPT_FIELD(Vector4, glm::vec4);
						READ_SCRIPT_FIELD(Entity, UUID);
					default:
						ENGINE_CORE_ERROR("Script Field Type {} does not support deserialization!", Utils::ScriptFieldTypeToString(type));
					}

					++i;
				}
			}

			uiButton.ReleasedEvent.InteractedEntityID = uiButtonComponent["ReleasedEvent.InteractedEntityID"].as<uint64_t>();
			uiButton.ReleasedEvent.InteractedFunction = uiButtonComponent["ReleasedEvent.InteractedFunction"].as<std::string>();
		}

		auto childEntities = entity["ChildEntities"];
		if (childEntities)
		{
			Entity parentEntity = m_Entity;

			bool firstChild = true;
			UUID prevChild = UUID::INVALID();

			for (auto childEntityNode : childEntities)
			{
				Entity childEntity = Deserialize(childEntityNode, isPrefab);

				if (isPrefab)
				{
					auto& childRelationship = childEntity.GetComponent<RelationshipComponent>();
					childRelationship.Parent = parentEntity.GetUUID();

					UUID childID = childEntity.GetUUID();

					if (firstChild)
					{
						firstChild = false;
						parentEntity.GetComponent<RelationshipComponent>().FirstChild = childID;
					}
					else
					{
						Entity prevChildEntity = m_Scene->GetEntityWithUUID(prevChild);
						prevChildEntity.GetComponent<RelationshipComponent>().NextChild = childID;
						childRelationship.PrevChild = prevChild;
					}

					prevChild = childID;
				}
			}

			m_Entity = parentEntity;
		}

		return m_Entity;
	}
}
