#pragma once
#include "Engine/Asset/Assets.h"

namespace Engine
{
	class AssetSerializer
	{
	public:
		virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, Ref<Asset> asset) const = 0;

		//template<typename T>
		//constexpr Ref<T> As()
		//{
		//	return std::static_pointer_cast<T>(Ref<AssetSerializer>(this));
		//}
	};

	using AssetSerializerMap = std::unordered_map<AssetType, Ref<AssetSerializer>>;

	class AssetImporter
	{
	public:
		void Init();
		void Shutdown();

		AssetSerializerMap& GetAssetSerializerMap();
	};

}
