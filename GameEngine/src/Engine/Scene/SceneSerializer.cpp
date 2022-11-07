#include "enginepch.h"
#include "Engine/Scene/SceneSerializer.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

#include "Engine/Scripting/ScriptEngine.h"

#include <yaml-cpp/yaml.h>

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
			uuid = node[0].as<float>();
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

	static std::string SceneCameraProjectionTypeToString(SceneCamera::ProjectionType projectionType)
	{
		switch (projectionType)
		{
			case SceneCamera::ProjectionType::Perspective:	return "Perspective";
			case SceneCamera::ProjectionType::Orthographic:	return "Orthographic";
		}

		ENGINE_CORE_ASSERT(false, "Unknown projection type!");
		return {};
	}

	static SceneCamera::ProjectionType SceneCameraProjectionTypeFromString(const std::string& projectionTypeString)
	{
		if (projectionTypeString == "Perspective")	return SceneCamera::ProjectionType::Perspective;
		if (projectionTypeString == "Orthographic")	return SceneCamera::ProjectionType::Orthographic;

		ENGINE_CORE_ASSERT(false, "Unknown projection type!");
		return SceneCamera::ProjectionType::Orthographic;
	}

	static std::string Rigidbody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:	return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:	return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic:	return "Kinematic";
		}

		ENGINE_CORE_ASSERT(false, "Unknown body type!");
		return {};
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static")		return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic")	return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic")	return Rigidbody2DComponent::BodyType::Kinematic;

		ENGINE_CORE_ASSERT(false, "Unknown body type!");
		return Rigidbody2DComponent::BodyType::Static;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ENGINE_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity must have UUID!");

		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // TagComponent

			out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().Tag;
			ENGINE_CORE_TRACE("Serializing entity with ID = {0}, name {1}", entity.GetUUID(), entity.GetComponent<TagComponent>().Tag);

			out << YAML::EndMap; // TagComponent
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
		
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap; // CameraComponent

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;
			
			out << YAML::Key << "Camera";
			out << YAML::BeginMap; // Camera
			
			out << YAML::Key << "ProjectionType" << YAML::Value << SceneCameraProjectionTypeToString(camera.GetProjectionType());
			
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
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
			out << YAML::Key << "Path" << YAML::Value << spriteRendererComponent.Path;
			out << YAML::Key << "Tiling" << YAML::Value << spriteRendererComponent.Tiling;

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
						WRITE_SCRIPT_FIELD(Float,	float		);
						WRITE_SCRIPT_FIELD(Double,	double		);
						WRITE_SCRIPT_FIELD(Bool,	bool		);
						WRITE_SCRIPT_FIELD(Char,	char		);
						WRITE_SCRIPT_FIELD(String,	std::string	);
						WRITE_SCRIPT_FIELD(Byte,	int8_t		);
						WRITE_SCRIPT_FIELD(Short,	int16_t		);
						WRITE_SCRIPT_FIELD(Int,		int32_t		);
						WRITE_SCRIPT_FIELD(Long,	int64_t		);
						WRITE_SCRIPT_FIELD(UByte,	uint8_t		);
						WRITE_SCRIPT_FIELD(UShort,	uint16_t	);
						WRITE_SCRIPT_FIELD(UInt,	uint32_t	);
						WRITE_SCRIPT_FIELD(ULong,	uint64_t	);
						WRITE_SCRIPT_FIELD(Vector2,	glm::vec2	);
						WRITE_SCRIPT_FIELD(Vector3,	glm::vec3	);
						WRITE_SCRIPT_FIELD(Vector4,	glm::vec4	);
						WRITE_SCRIPT_FIELD(Entity,	UUID		);
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
			out << YAML::Key << "Type" << YAML::Value << Rigidbody2DBodyTypeToString(rigidbody2DComponent.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;

			out << YAML::EndMap; // Rigidbody2DComponent
		}
		
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2DComponent

			auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
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
			out << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider2DComponent.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider2DComponent.RestitutionThreshold;

			out << YAML::EndMap; // CircleCollider2DComponent
		}
		
		out << YAML::EndMap; // Entity
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->m_Name;
		ENGINE_CORE_TRACE("Serializing scene '{0}'", m_Scene->m_Name);
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if(!entity) return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap; // Scene

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented yet
		ENGINE_CORE_ASSERT(false, "Not implemented yet");
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .scene file '{0}'\n {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		m_Scene->m_Name = data["Scene"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing scene '{0}'", m_Scene->m_Name);

		auto entities = data["Entities"];
		if(entities)
		{
			for (auto entity : entities)
			{
				UUID uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				
				ENGINE_CORE_TRACE("Deserializing entity with ID = {0}, name {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities are created with a transform
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Position = transformComponent["Position"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}
				else // So if there is none then remove from Entity
				{
					deserializedEntity.RemoveComponent<TransformComponent>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& camera = cc.Camera;

					auto& cameraProps = cameraComponent["Camera"];
					
					camera.SetProjectionType(SceneCameraProjectionTypeFromString(cameraProps["ProjectionType"].as<std::string>()));

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
					auto& spriteRenderer = deserializedEntity.AddComponent<SpriteRendererComponent>();
					spriteRenderer.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					spriteRenderer.Path = spriteRendererComponent["Path"].as<std::string>();
					spriteRenderer.Tiling = spriteRendererComponent["Tiling"].as<float>();

					if(!spriteRenderer.Path.empty()) spriteRenderer.LoadTexture(spriteRenderer.Path);
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& circleRenderer = deserializedEntity.AddComponent<CircleRendererComponent>();
					circleRenderer.Color = circleRendererComponent["Color"].as<glm::vec4>();
					circleRenderer.Radius = circleRendererComponent["Radius"].as<float>();
					circleRenderer.Thickness = circleRendererComponent["Thickness"].as<float>();
					circleRenderer.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						ENGINE_CORE_ASSERT(entityClass, "Entity Class does not exists for given script component!");
						const auto& fields = entityClass->GetScriptFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

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
								READ_SCRIPT_FIELD(Float,	float		);
								READ_SCRIPT_FIELD(Double,	double		);
								READ_SCRIPT_FIELD(Bool,		bool		);
								READ_SCRIPT_FIELD(Char,		char		);
								READ_SCRIPT_FIELD(String,	std::string	);
								READ_SCRIPT_FIELD(Byte,		int8_t		);
								READ_SCRIPT_FIELD(Short,	int16_t		);
								READ_SCRIPT_FIELD(Int,		int32_t		);
								READ_SCRIPT_FIELD(Long,		int64_t		);
								READ_SCRIPT_FIELD(UByte,	uint8_t		);
								READ_SCRIPT_FIELD(UShort,	uint16_t	);
								READ_SCRIPT_FIELD(UInt,		uint32_t	);
								READ_SCRIPT_FIELD(ULong,	uint64_t	);
								READ_SCRIPT_FIELD(Vector2,	glm::vec2	);
								READ_SCRIPT_FIELD(Vector3,	glm::vec3	);
								READ_SCRIPT_FIELD(Vector4,	glm::vec4	);
								READ_SCRIPT_FIELD(Entity,	UUID		);
							default:
								ENGINE_CORE_ERROR("Script Field Type {} does not support deserialization!", Utils::ScriptFieldTypeToString(type));
							}
						}
					}
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rigidbody2D = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rigidbody2D.Type = Rigidbody2DBodyTypeFromString(rigidbody2DComponent["Type"].as<std::string>());
					rigidbody2D.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& boxCollider2D = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					boxCollider2D.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					boxCollider2D.Size = boxCollider2DComponent["Size"].as<glm::vec2>();

					boxCollider2D.Density = boxCollider2DComponent["Density"].as<float>();
					boxCollider2D.Friction = boxCollider2DComponent["Friction"].as<float>();
					boxCollider2D.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					boxCollider2D.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& circleCollider2D = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					circleCollider2D.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					circleCollider2D.Radius = circleCollider2DComponent["Radius"].as<float>();

					circleCollider2D.Density = circleCollider2DComponent["Density"].as<float>();
					circleCollider2D.Friction = circleCollider2DComponent["Friction"].as<float>();
					circleCollider2D.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					circleCollider2D.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented yet
		ENGINE_CORE_ASSERT(false, "Not implemented yet");

		return false;
	}
}
