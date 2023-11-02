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

	static void SerializeUIInteraction(YAML::Emitter& out, const Interaction& interaction, const std::string& eventName = "interaction")
	{

		out << YAML::Key << eventName + ".InteractedEntityID" << YAML::Value << interaction.InteractedEntityID;
		out << YAML::Key << eventName + ".InteractedFunction" << YAML::Value << interaction.InteractedFunction;
		out << YAML::Key << eventName + ".Params" << YAML::Value;
		{
			out << YAML::BeginSeq;
			int i = 0;
			for (auto& Param : interaction.Params)
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
					WRITE_SCRIPT_FIELD(Entity, uint64_t);
					WRITE_SCRIPT_FIELD(Prefab, uint64_t);
				default:
					ENGINE_CORE_ERROR("Script Field Type {} does not support serialization!", Utils::ScriptFieldTypeToString(scriptField.Field.Type));
				}

				out << YAML::EndMap; // ScriptFields

				++i;
			}
			out << YAML::EndSeq;
		}
	}

	static void DeserializeUIInteraction(YAML::Node& uiButtonComponent, Interaction& interaction, const std::string& eventName = "interaction")
	{
		interaction.InteractedEntityID = uiButtonComponent[eventName + ".InteractedEntityID"].as<uint64_t>();
		interaction.InteractedFunction = uiButtonComponent[eventName + ".InteractedFunction"].as<std::string>();

		auto scriptFields = uiButtonComponent[eventName + ".Params"];
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

				interaction.Params[i] = new ScriptFieldInstance();
				interaction.Params[i]->Field = { type };

				ScriptFieldInstance& fieldInstance = *interaction.Params[i];
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
					READ_SCRIPT_FIELD(Entity, uint64_t);
					READ_SCRIPT_FIELD(Prefab, uint64_t);
				default:
					ENGINE_CORE_ERROR("Script Field Type {} does not support deserialization!", Utils::ScriptFieldTypeToString(type));
				}

				++i;
			}
		}
	}

	void EntitySerializer::Serialize(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene)
	{
		ENGINE_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity must have UUID!");

		out << YAML::BeginMap; // Entity
		UUID uuid = entity.GetUUID();
		out << YAML::Key << "Entity" << YAML::Value << uuid;

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			std::string& name = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << name;
			ENGINE_CORE_TRACE("Serializing entity with ID = {0}, name {1}", uuid, name);

			out << YAML::EndMap; // TagComponent
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			out << YAML::Key << "RelationshipComponent";
			out << YAML::BeginMap; // RelationshipComponent

			auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();
			out << YAML::Key << "ChildrenCount" << YAML::Value << relationshipComponent.ChildrenCount;
			out << YAML::Key << "FirstChild" << YAML::Value << relationshipComponent.FirstChild;
			out << YAML::Key << "NextChild" << YAML::Value << relationshipComponent.NextChild;
			out << YAML::Key << "PrevChild" << YAML::Value << relationshipComponent.PrevChild;
			out << YAML::Key << "Parent" << YAML::Value << relationshipComponent.Parent;

			out << YAML::EndMap; // RelationshipComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // TransformComponent

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << transformComponent.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;

			out << YAML::EndMap; // TransformComponent
		}

		if (entity.HasComponent<PrefabComponent>())
		{
			out << YAML::Key << "PrefabComponent";
			out << YAML::BeginMap; // PrefabComponent

			AssetHandle handle = entity.GetComponent<PrefabComponent>().PrefabHandle;
			out << YAML::Key << "PrefabHandle" << YAML::Value << handle;

			out << YAML::EndMap; // PrefabComponent
		}

		if (entity.HasComponent<UILayoutComponent>())
		{
			out << YAML::Key << "UILayoutComponent";
			out << YAML::BeginMap; // UILayoutComponent

			auto& uiLayoutComponent = entity.GetComponent<UILayoutComponent>();
			out << YAML::Key << "Size" << YAML::Value << uiLayoutComponent.Size;
			out << YAML::Key << "AnchorMin" << YAML::Value << uiLayoutComponent.AnchorMin;
			out << YAML::Key << "AnchorMax" << YAML::Value << uiLayoutComponent.AnchorMax;

			out << YAML::EndMap; // UILayoutComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
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

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // SpriteRendererComponent

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Texture" << YAML::Value << spriteRendererComponent.Texture;
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			//out << YAML::Key << "Path" << YAML::Value << spriteRendererComponent.Path.string();
			out << YAML::Key << "Tiling" << YAML::Value << spriteRendererComponent.Tiling;

			out << YAML::Key << "IsSubTexture" << YAML::Value << spriteRendererComponent.IsSubTexture;
			out << YAML::Key << "SubCoords" << YAML::Value << spriteRendererComponent.SubCoords;
			out << YAML::Key << "SubCellSize" << YAML::Value << spriteRendererComponent.SubCellSize;
			out << YAML::Key << "SubSpriteSize" << YAML::Value << spriteRendererComponent.SubSpriteSize;

			out << YAML::EndMap; // SpriteRendererComponent
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // CircleRendererComponent

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Radius" << YAML::Value << circleRendererComponent.Radius;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap; // CircleRendererComponent
		}

		if (entity.HasComponent<TextRendererComponent>())
		{
			out << YAML::Key << "TextRendererComponent";
			out << YAML::BeginMap; // TextRendererComponent

			auto& textRendererComponent = entity.GetComponent<TextRendererComponent>();
			out << YAML::Key << "TextString" << YAML::Value << textRendererComponent.TextString;
			//out << YAML::Key << "FontAsset" << YAML::Value << textRendererComponent.FontAsset; // TODO
			out << YAML::Key << "Color" << YAML::Value << textRendererComponent.Color;
			out << YAML::Key << "Kerning" << YAML::Value << textRendererComponent.Kerning;
			out << YAML::Key << "LineSpacing" << YAML::Value << textRendererComponent.LineSpacing;

			out << YAML::EndMap; // TextRendererComponent
		}

		if (entity.HasComponent<ScriptComponent>())
		{
			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap; // ScriptComponent

			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.ClassName;

			// Fields
			const auto& scriptFields = ScriptEngine::GetEntityClasses().at(scriptComponent.ClassName)->GetScriptFields();
			if (scriptFields.size() > 0)
			{
				out << YAML::Key << "ScriptFields" << YAML::Value;
				auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
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
						WRITE_SCRIPT_FIELD(Entity, uint64_t);
						WRITE_SCRIPT_FIELD(Prefab, uint64_t);
					default:
						ENGINE_CORE_ERROR("Script Field Type {} does not support serialization!", Utils::ScriptFieldTypeToString(field.Type));
					}

					out << YAML::EndMap; // ScriptFields
				}

				out << YAML::EndSeq;
			}

			out << YAML::EndMap; // ScriptComponent
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2DComponent

			auto& rigidbody2DComponent = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "Type" << YAML::Value << Utils::Rigidbody2DBodyTypeToString(rigidbody2DComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;
			out << YAML::Key << "Smoothing" << YAML::Value << Utils::Rigidbody2DSmoothingTypeToString(rigidbody2DComponent.Smoothing);
			out << YAML::Key << "GravityScale" << YAML::Value << rigidbody2DComponent.GravityScale;

			out << YAML::EndMap; // Rigidbody2DComponent
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
			out << YAML::Key << "Sensor" << YAML::Value << boxCollider2DComponent.Sensor;
			out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; // BoxCollider2DComponent
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // CircleCollider2DComponent

			auto& circleCollider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circleCollider2DComponent.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circleCollider2DComponent.Radius;
			out << YAML::Key << "Sensor" << YAML::Value << circleCollider2DComponent.Sensor;
			out << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}

		if (entity.HasComponent<UIButtonComponent>())
		{
			out << YAML::Key << "UIButtonComponent";
			out << YAML::BeginMap; // UIButtonComponent

			auto& uiButtonComponent = entity.GetComponent<UIButtonComponent>();
			out << YAML::Key << "NormalColor" << YAML::Value << uiButtonComponent.NormalColor;
			out << YAML::Key << "HoverColor" << YAML::Value << uiButtonComponent.HoverColor;
			out << YAML::Key << "PressedColor" << YAML::Value << uiButtonComponent.PressedColor;
			out << YAML::Key << "DisabledColor" << YAML::Value << uiButtonComponent.DisabledColor;

			SerializeUIInteraction(out, uiButtonComponent.PressedEvent, "PressedEvent");
			SerializeUIInteraction(out, uiButtonComponent.ReleasedEvent, "ReleasedEvent");

			out << YAML::EndMap; // UIButtonComponent
		}

		auto& relationship = entity.GetComponent<RelationshipComponent>();
		if (relationship.HasChildren())
		{
			Entity parentEntity = entity;
			out << YAML::Key << "ChildEntities" << YAML::Value << YAML::BeginSeq;

			UUID childIterator = relationship.FirstChild;
			for (uint64_t i = 0; i < relationship.ChildrenCount; ++i)
			{
				entity = scene->GetEntityWithUUID(childIterator);
				Serialize(out, entity, scene);

				childIterator = entity.GetComponent<RelationshipComponent>().NextChild;
				if (!childIterator.IsValid())
					break;
			}

			entity = parentEntity;

			out << YAML::EndSeq; // ChildEntities
		}

		out << YAML::EndMap; // Entity
	}

	Entity EntitySerializer::Deserialize(YAML::Node& entityOut, Entity entity, Ref<Scene>& scene, bool isPrefab)
	{
		UUID uuid = isPrefab ? UUID() : entityOut["Entity"].as<uint64_t>();

		std::string name;
		auto tagComponent = entityOut["TagComponent"];
		if (tagComponent)
			name = tagComponent["Tag"].as<std::string>();

		ENGINE_CORE_TRACE("Deserializing entity {0}: {1}", uuid, name);
		entity = scene->CreateEntityWithUUID(uuid, name);

		auto transformComponent = entityOut["TransformComponent"];
		if (transformComponent)
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			tc.Position = transformComponent["Position"].as<glm::vec3>();
			tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
			tc.Scale = transformComponent["Scale"].as<glm::vec3>();
		}

		auto relationshipComponent = entityOut["RelationshipComponent"];
		if (relationshipComponent)
		{
			auto& relationship = entity.GetComponent<RelationshipComponent>();
			relationship.ChildrenCount = relationshipComponent["ChildrenCount"].as<uint64_t>();
			relationship.FirstChild = relationshipComponent["FirstChild"].as<uint64_t>();
			relationship.NextChild = relationshipComponent["NextChild"].as<uint64_t>();
			relationship.PrevChild = relationshipComponent["PrevChild"].as<uint64_t>();
			relationship.Parent = relationshipComponent["Parent"].as<uint64_t>();
		}

		auto prefabComponent = entityOut["PrefabComponent"];
		if (prefabComponent)
		{
			auto& pc = entity.AddComponent<PrefabComponent>();
			pc.PrefabHandle = prefabComponent["PrefabHandle"].as<uint64_t>();
		}

		auto uiLayoutComponent = entityOut["UILayoutComponent"];
		if (uiLayoutComponent)
		{
			auto& ui = entity.AddComponent<UILayoutComponent>();
			ui.Size = uiLayoutComponent["Size"].as<glm::vec2>();
			ui.AnchorMin = uiLayoutComponent["AnchorMin"].as<glm::vec2>();
			ui.AnchorMax = uiLayoutComponent["AnchorMax"].as<glm::vec2>();
		}

		auto cameraComponent = entityOut["CameraComponent"];
		if (cameraComponent)
		{
			auto& cc = entity.AddComponent<CameraComponent>();
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

		auto spriteRendererComponent = entityOut["SpriteRendererComponent"];
		if (spriteRendererComponent)
		{
			auto& spriteRenderer = entity.AddComponent<SpriteRendererComponent>();
			spriteRenderer.Texture = spriteRendererComponent["Texture"].as<uint64_t>();
			spriteRenderer.Color = spriteRendererComponent["Color"].as<glm::vec4>();
			//spriteRenderer.Path = spriteRendererComponent["Path"].as<std::string>();
			spriteRenderer.Tiling = spriteRendererComponent["Tiling"].as<float>();

			spriteRenderer.IsSubTexture = spriteRendererComponent["IsSubTexture"].as<bool>();
			spriteRenderer.SubCoords = spriteRendererComponent["SubCoords"].as<glm::vec2>();
			spriteRenderer.SubCellSize = spriteRendererComponent["SubCellSize"].as<glm::vec2>();
			spriteRenderer.SubSpriteSize = spriteRendererComponent["SubSpriteSize"].as<glm::vec2>();

			//spriteRenderer.LoadTexture(spriteRenderer.Path);
			spriteRenderer.AssignTexture(spriteRenderer.Texture);
			//AssetManager::GetAsset<Texture2D>(spriteRenderer.Texture);
		}

		auto circleRendererComponent = entityOut["CircleRendererComponent"];
		if (circleRendererComponent)
		{
			auto& circleRenderer = entity.AddComponent<CircleRendererComponent>();
			circleRenderer.Color = circleRendererComponent["Color"].as<glm::vec4>();
			circleRenderer.Radius = circleRendererComponent["Radius"].as<float>();
			circleRenderer.Thickness = circleRendererComponent["Thickness"].as<float>();
			circleRenderer.Fade = circleRendererComponent["Fade"].as<float>();
		}

		auto textRendererComponent = entityOut["TextRendererComponent"];
		if (textRendererComponent)
		{
			auto& textRenderer = entity.AddComponent<TextRendererComponent>();
			textRenderer.TextString = textRendererComponent["TextString"].as<std::string>();
			// textRenderer.FontAsset = font; // TODO
			textRenderer.Color = textRendererComponent["Color"].as<glm::vec4>();
			textRenderer.Kerning = textRendererComponent["Kerning"].as<float>();
			textRenderer.LineSpacing = textRendererComponent["LineSpacing"].as<float>();
		}

		auto scriptComponent = entityOut["ScriptComponent"];
		if (scriptComponent)
		{
			auto& sc = entity.AddComponent<ScriptComponent>();
			sc.ClassName = scriptComponent["ClassName"].as<std::string>();

			auto scriptFields = scriptComponent["ScriptFields"];
			if (scriptFields)
			{
				Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
				if (entityClass)
				{
					const auto& fields = entityClass->GetScriptFields();
					auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

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
							READ_SCRIPT_FIELD(Entity, uint64_t);
							READ_SCRIPT_FIELD(Prefab, uint64_t);
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

		auto rigidbody2DComponent = entityOut["Rigidbody2DComponent"];
		if (rigidbody2DComponent)
		{
			auto& rigidbody2D = entity.AddComponent<Rigidbody2DComponent>();
			rigidbody2D.Type = Utils::Rigidbody2DBodyTypeFromString(rigidbody2DComponent["Type"].as<std::string>());
			rigidbody2D.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			rigidbody2D.Smoothing = Utils::Rigidbody2DSmoothingTypeFromString(rigidbody2DComponent["Smoothing"].as<std::string>());
			rigidbody2D.GravityScale = rigidbody2DComponent["GravityScale"].as<float>();
		}

		auto boxCollider2DComponent = entityOut["BoxCollider2DComponent"];
		if (boxCollider2DComponent)
		{
			auto& boxCollider2D = entity.AddComponent<BoxCollider2DComponent>();
			boxCollider2D.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
			boxCollider2D.Size = boxCollider2DComponent["Size"].as<glm::vec2>();

			boxCollider2D.Sensor = boxCollider2DComponent["Sensor"].as<bool>();

			boxCollider2D.Density = boxCollider2DComponent["Density"].as<float>();
			boxCollider2D.Friction = boxCollider2DComponent["Friction"].as<float>();
			boxCollider2D.Restitution = boxCollider2DComponent["Restitution"].as<float>();
			boxCollider2D.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
		}

		auto circleCollider2DComponent = entityOut["CircleCollider2DComponent"];
		if (circleCollider2DComponent)
		{
			auto& circleCollider2D = entity.AddComponent<CircleCollider2DComponent>();
			circleCollider2D.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
			circleCollider2D.Radius = circleCollider2DComponent["Radius"].as<float>();

			circleCollider2D.Sensor = circleCollider2DComponent["Sensor"].as<bool>();

			circleCollider2D.Density = circleCollider2DComponent["Density"].as<float>();
			circleCollider2D.Friction = circleCollider2DComponent["Friction"].as<float>();
			circleCollider2D.Restitution = circleCollider2DComponent["Restitution"].as<float>();
			circleCollider2D.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
		}

		auto uiButtonComponent = entityOut["UIButtonComponent"];
		if (uiButtonComponent)
		{
			auto& uiButton = entity.AddComponent<UIButtonComponent>();
			uiButton.NormalColor = uiButtonComponent["NormalColor"].as<glm::vec4>();
			uiButton.HoverColor = uiButtonComponent["HoverColor"].as<glm::vec4>();
			uiButton.PressedColor = uiButtonComponent["PressedColor"].as<glm::vec4>();
			uiButton.DisabledColor = uiButtonComponent["DisabledColor"].as<glm::vec4>();

			DeserializeUIInteraction(uiButtonComponent, uiButton.PressedEvent, "PressedEvent");
			DeserializeUIInteraction(uiButtonComponent, uiButton.ReleasedEvent, "ReleasedEvent");
		}

		auto childEntities = entityOut["ChildEntities"];
		if (childEntities)
		{
			Entity parentEntity = entity;

			bool firstChild = true;
			UUID prevChild = UUID::INVALID();

			for (auto childEntityNode : childEntities)
			{
				Entity childEntity = Deserialize(childEntityNode, entity, scene, isPrefab);

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
						Entity prevChildEntity = scene->GetEntityWithUUID(prevChild);
						prevChildEntity.GetComponent<RelationshipComponent>().NextChild = childID;
						childRelationship.PrevChild = prevChild;
					}

					prevChild = childID;
				}
			}

			entity = parentEntity;
		}

		return entity;
	}
}
