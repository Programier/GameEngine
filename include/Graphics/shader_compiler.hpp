#pragma once
#include <Core/object.hpp>
#include <Core/structures.hpp>

namespace Engine
{
	class Logger;
	class Material;

	namespace ShaderCompiler
	{
		struct ENGINE_EXPORT ShaderReflection {

			struct ENGINE_EXPORT VertexAttribute {
				String name;
				VertexBufferElementType type;
				VertexAttributeInputRate rate;
				VertexBufferSemantic semantic;
				byte semantic_index;
				byte location;
				byte stream_index;
				uint16_t offset;
			};

			Vector<VertexAttribute> attributes;
			Vector<MaterialParameterInfo> uniform_member_infos;

			FORCE_INLINE ShaderReflection& clear()
			{
				uniform_member_infos.clear();
				attributes.clear();
				return *this;
			}
		};

		struct ENGINE_EXPORT ShaderSource {
			Buffer vertex_code;
			Buffer tessellation_control_code;
			Buffer tessellation_code;
			Buffer geometry_code;
			Buffer fragment_code;
			Buffer compute_code;
			ShaderReflection reflection;

			FORCE_INLINE bool has_valid_graphical_pipeline() const
			{
				return has_vertex_shader() && has_fragment_shader();
			}

			FORCE_INLINE bool has_valid_compute_pipeline() const
			{
				return has_compute_shader();
			}

			FORCE_INLINE bool has_vertex_shader() const
			{
				return !vertex_code.empty();
			}

			FORCE_INLINE bool has_tessellation_control_shader() const
			{
				return !tessellation_control_code.empty();
			}

			FORCE_INLINE bool has_tessellation_shader() const
			{
				return !tessellation_code.empty();
			}

			FORCE_INLINE bool has_geometry_shader() const
			{
				return !geometry_code.empty();
			}

			FORCE_INLINE bool has_fragment_shader() const
			{
				return !fragment_code.empty();
			}

			FORCE_INLINE bool has_compute_shader() const
			{
				return !compute_code.empty();
			}
		};

		class ENGINE_EXPORT Compiler : public Object
		{
			declare_class(Compiler, Object);

		public:
			static Compiler* static_create_compiler(const StringView& api_name = "");

			virtual bool compile(Material* material, const String& slang_source, ShaderSource& out_source) = 0;
		};
	}// namespace ShaderCompiler
}// namespace Engine
