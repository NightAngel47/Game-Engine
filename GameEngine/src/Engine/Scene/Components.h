#pragma once
#include "Engine/Asset/AssetManager.h"
#include "Engine/Core/UUID.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Scene/SceneCamera.h"
#include "Engine/Project/Project.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/UI/UIEngine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{

#pragma region Entity Components
	// Default Components for All Entities

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct RelationshipComponent
	{
		uint64_t ChildrenCount{};
		UUID FirstChild = UUID::INVALID();
		UUID NextChild = UUID::INVALID();
		UUID PrevChild = UUID::INVALID();
		UUID Parent = UUID::INVALID();

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent&) = default;

		bool const HasChildren() const { return ChildrenCount > 0; }
	};

	struct TransformComponent
	{
		glm::vec3 Position{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position)
			: Position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

#pragma endregion Entity Components

#pragma region Game Components
	// Components Available to Entities

	struct SpriteRendererComponent
	{
		AssetHandle Texture = AssetHandle::INVALID();
		glm::vec4 Color{ 1.0f };
		float Tiling = 1.0f;

		//Sub Texture
		bool IsSubTexture = false;
		Ref<SubTexture2D> SubTexture = nullptr;
		glm::vec2 SubCoords{ 0.0f };
		glm::vec2 SubCellSize{ 0.0f };
		glm::vec2 SubSpriteSize{ 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}

		void AssignTexture(AssetHandle handle)
		{
			if (AssetManager::IsAssetHandleValid(handle))
			{
				Texture = handle;

				GenerateSubTexture();
			}
		}

		void GenerateSubTexture()
		{
			if (IsSubTexture && Texture)
			{
				SubTexture = SubTexture2D::CreateFromCoords(Texture, SubCoords, SubCellSize, SubSpriteSize);
			}
		}

		void ClearTexture()
		{
			Texture = AssetHandle::INVALID();
		}

		const Ref<Texture2D> GetTexture2D()
		{
			if (!AssetManager::IsAssetHandleValid(Texture))
				return nullptr;

			return AssetManager::GetAsset<Texture2D>(Texture);
		}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f };
		float Radius = 0.5f;
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct TextRendererComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();

		glm::vec4 Color{ 1.0f };

		float Kerning = 0.0f;
		float LineSpacing = 0.0f;

		TextRendererComponent() = default;
		TextRendererComponent(const TextRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// forward declaration
	class ScriptFieldDataBase;

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Forward declaration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	// Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType {
			Static = 0, Dynamic, Kinematic
		};
		BodyType Type = BodyType::Static;

		bool FixedRotation = false;

		enum class SmoothingType {
			None = 0, Interpolation, Extrapolation
		};
		SmoothingType Smoothing = SmoothingType::Interpolation;
		glm::vec2 previousPosition;
		float previousAngle;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset{ 0.0f };
		glm::vec2 Size{ 0.5f };

		bool Sensor = false;

		// TODO maybe make physics material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset{ 0.0f };
		float Radius = 0.5f;

		bool Sensor = false;

		// TODO maybe make physics material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

#pragma endregion GameComponents

#pragma region GameUIComponents

	struct UILayoutComponent
	{
		glm::vec2 Size{ 100.0f };
		glm::vec2 AnchorMin{ 0.0f };
		glm::vec2 AnchorMax{ 0.0f };

		UILayoutComponent() = default;
		UILayoutComponent(const UILayoutComponent&) = default;
	};

	struct UIButtonComponent
	{
		ButtonStates ButtonState;

		glm::vec4 NormalColor{ 0.75f, 0.75f, 0.75f, 1.0f };
		glm::vec4 HoverColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 PressedColor{ 0.55f, 0.55f, 0.55f, 1.0f };
		glm::vec4 DisabledColor{ 0.15f, 0.15f, 0.15f, 1.0f };

		glm::vec4 const GetButtonTint()
		{
			if (ButtonState.Interactable)
			{
				if (ButtonState.Pressed) return PressedColor;
				if (ButtonState.Hovered) return HoverColor;

				return NormalColor;
			}

			return DisabledColor;
		}

		Interaction PressedEvent;
		Interaction ReleasedEvent;

		UIButtonComponent() = default;
		UIButtonComponent(const UIButtonComponent&) = default;

		void OnPressed()
		{
			if (ButtonState.Interactable && !ButtonState.Pressed)
			{
				ButtonState.Pressed = true;
				PressedEvent.Interacted();
			}
		}

		void OnReleased()
		{
			if (ButtonState.Interactable && ButtonState.Pressed)
			{
				ButtonState.Pressed = false;
				ReleasedEvent.Interacted();
			}
		}
	};

#pragma endregion GameUIComponents

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<
		TransformComponent, 
		SpriteRendererComponent, CircleRendererComponent, TextRendererComponent,
		CameraComponent, 
		NativeScriptComponent, ScriptComponent, 
		Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
		UILayoutComponent, UIButtonComponent>;
}
