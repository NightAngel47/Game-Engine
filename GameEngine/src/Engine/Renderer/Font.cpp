#include "enginepch.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Renderer/MSDFData.h" 

#define THREAD_COUNT 8

namespace Engine
{
	template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	static Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName, float fontSize, const std::vector<msdf_atlas::GlyphGeometry>& glyphs, const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height)
	{
		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
		generator.setAttributes(attributes);
		generator.setThreadCount(THREAD_COUNT); // TODO check what's available later
		generator.generate(glyphs.data(), (int)glyphs.size());

		msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

		TextureSpecification fontTextureSpec;
		fontTextureSpec.Width = (uint32_t)bitmap.width;
		fontTextureSpec.Height = (uint32_t)bitmap.height;
		fontTextureSpec.Format = ImageFormat::RGB8;
		fontTextureSpec.GenerateMips = false;

		Ref<Texture2D> texture = Texture2D::Create(fontTextureSpec, Buffer((void*)bitmap.pixels, bitmap.width * bitmap.height * 3));
		return texture;
	}

	Font::Font(const std::filesystem::path& filepath)
		:m_Data(new MSDFData())
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		ENGINE_CORE_ASSERT(ft);
		if (!ft)
		{
			return;
		}

		std::string fileString = filepath.string();
		// TODO use msdfgen::loadFontData to load from buffer eventually
		msdfgen::FontHandle* font = msdfgen::loadFont(ft, fileString.c_str());
		if (!font)
		{
			ENGINE_CORE_ERROR("Failed to load font: {}", fileString);
			return;
		}

		struct CharsetRange
		{
			uint32_t Begin, End;
		};

		// From imgui_draw.cpp
		static const CharsetRange charsetRange[] =
		{
			{0x0020, 0x00FF} // Basic Lation + Latin Supplement
		};

		msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRange)
		{
			for (uint32_t c = range.Begin; c <= range.End; ++c)
			{
				charset.add(c);
			}
		}

		double fontScale = 1.0;
		m_Data->FontGeo = msdf_atlas::FontGeometry(&m_Data->Glyphs);
		int glyphsLoaded = m_Data->FontGeo.loadCharset(font, fontScale, charset);
		ENGINE_CORE_TRACE("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

		double emSize = 40.0;

		msdf_atlas::TightAtlasPacker atlasPacker;
		//atlasPacker.setDimensionsConstraint();
		atlasPacker.setPixelRange(2.0);
		atlasPacker.setMiterLimit(1.0);
		atlasPacker.setPadding(0);
		atlasPacker.setScale(emSize);
		int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
		ENGINE_CORE_ASSERT(remaining == 0);
		
		int width, height;
		atlasPacker.getDimensions(width, height);
		emSize = atlasPacker.getScale();

		// if MSDF || MTSDF
		// Edge coloring

#define DEFAULT_ANGEL_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull

		uint64_t coloringSeed = 0;
		bool expensiveColoring = false;
		if (expensiveColoring) {
			msdf_atlas::Workload([&glyphs = m_Data->Glyphs, &coloringSeed](int i, int threadNo) -> bool
				{
					unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
					glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGEL_THRESHOLD, glyphSeed);
					return true;
				}, m_Data->Glyphs.size()).finish(THREAD_COUNT);
		}
		else {
			unsigned long long glyphSeed = coloringSeed;
			for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
			{
				glyphSeed *= LCG_MULTIPLIER;
				glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGEL_THRESHOLD, glyphSeed);
			}
		}
		
		m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>("Test", (float)emSize, m_Data->Glyphs, m_Data->FontGeo, width, height);

#if 0
		msdfgen::Shape shape;
		if (msdfgen::loadGlyph(shape, font, 'A'))
		{
			shape.normalize();
			//                      max. angle
			msdfgen::edgeColoringSimple(shape, 3.0);
			//           image width, height
			msdfgen::Bitmap<float, 3> msdf(32, 32);
			//                     range, scale, translation
			msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));
			msdfgen::savePng(msdf, "output.png");
		}
#endif

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

	Font::~Font()
	{
		delete m_Data;
	}


	Ref<Font> Font::GetDefault()
	{
		static Ref<Font> DefaultFont;
		if (!DefaultFont)
			DefaultFont = CreateRef<Font>("assets/fonts/OpenSans/OpenSans-Regular.ttf");

		return DefaultFont;
	}

}
