#include "realsim/graphics/RootSignatureFileDeserializer.h"


namespace RSim::Graphics
{
	RootSignatureFileDeserializer::RootSignatureFileDeserializer(const std::filesystem::path& path)
	{
		rsim_trace("Attempting to deserialize root signature file : '{0}'", path.string());

		YAML::Node root;
		try
		{
			root = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException const& e)
		{
			rsim_error("Error deserializing root signature description file: {0}", path.string(), e.what());
			return;
		}

		YAML::Node const rootSigNode = root["root_signature"];

		if (!rootSigNode)
		{
			rsim_error("No 'root_signature' node has been found.");
			return;
		}


		YAML::Node const rootSigFlagsNode = rootSigNode["root_signature_flags"];

		if (!rootSigFlagsNode)
		{
			rsim_error("No 'root_signature_flags' node has been found.");
			return;
		}

		if (!rootSigFlagsNode.IsSequence())
		{
			rsim_error("'root_signature_flags' node needs to be a sequence.");
			return;
		}

		if (auto const rootSignatureFlags = RootSignatureFileDeserializer::GetRootSignatureFlags(rootSigFlagsNode))
		{
			m_RootSignatureFlags = *rootSignatureFlags;
		}
		else
		{
			return;
		}

		std::string rootSigName{};

		if (!rootSigNode["root_signature_name"])
		{
			rsim_warn("'root_signature_name' has not been set. Defaulting to 'Unnamed Root Signature'");
			rootSigName = "Unnamed Root Signature";
		}

		if (!rootSigNode["root_parameters"])
		{
			rsim_error("No 'root_parameters' node has been found in the root signature file. File:\n{0}",
			           path.string());
			return;
		}

		YAML::Node const rootParameters = rootSigNode["root_parameters"];

		if (!rootParameters.IsSequence())
		{
			rsim_error("'root_parameters' needs to be sequence.");
			return;
		}

		size_t i = 0;

		for (auto param : rootParameters)
		{
			YAML::Node paramNameNode = param["parameter_name"];
			if (!paramNameNode)
			{
				rsim_error("Parameter name needs to be set for the root parameter at index {0}.", i);
				return;
			}

			auto paramNameStr = Serialization::AsIf<std::string>(paramNameNode);
			if (!paramNameStr)
			{
				rsim_error("Parameter name is not a string.");
				return;
			}

			YAML::Node typeNode = param["type"];
			if (!typeNode)
			{
				rsim_error("No root parameter 'type' has been set for root parameter '{0}'.", paramNameStr.value());
				return;
			}
			YAML::Node visibilityNode = param["visibility"];
			if (!visibilityNode)
			{
				rsim_error("No root parameter 'visibility' has been set for root parameter '{0}'.",
				           paramNameStr.value());
				return;
			}

			auto typeStr = Serialization::AsIf<std::string>(typeNode);
			auto paramType = StringToRootParameterType(*typeStr);


			/* Check the 'type' string values to see if they are valid. */
			{
				if (!typeStr)
				{
					rsim_error("Current type is not a string for the parameter '{0}'", paramNameStr.value());
					return;
				}
				if (!paramType)
				{
					rsim_error("Type '{1}' is not a valid type for the root parameter '{0}'", paramNameStr.value(),
					           *typeStr);
					return;
				}
			}

			auto visibilityStr = Serialization::AsIf<std::string>(visibilityNode);
			auto visibility = StringToShaderVisibility(*visibilityStr);

			/* Check the 'visibility' string values to see if they are valid. */
			{
				if (!visibilityStr)
				{
					rsim_error("Current visibility setting is not a string for the parameter '{0}'",
					           paramNameStr.value());
					return;
				}

				if (!visibility)
				{
					rsim_error("Visibility setting '{1}' is not a valid value for the parameter '{0}'.",
					           paramNameStr.value(), *visibilityStr);
					return;
				}
			}

			std::optional<CD3DX12_ROOT_PARAMETER1> d3d12RootParameter;

			switch (*paramType)
			{
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				{
					m_DescriptorRanges.emplace_back();
					d3d12RootParameter = GetD3D12DescriptorTableRootParameter(
						param, i, *paramNameStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				{
					d3d12RootParameter = GetD3D1232BitConstantsRootParameter(
						param, *paramNameStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
			case D3D12_ROOT_PARAMETER_TYPE_SRV:
			case D3D12_ROOT_PARAMETER_TYPE_UAV:
				{
					d3d12RootParameter = GetD3D12CBV_SRV_UAV_RootParameter(
						param, *paramNameStr, *typeStr, *visibilityStr, *visibility);
					if (!d3d12RootParameter)
						return;
					break;
				}

			default: break;
			}

			auto& p = m_RootParameters.emplace_back(RootParameter{ *d3d12RootParameter });
			++i;
		}

		m_ParseSucceeded = true;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D1232BitConstantsRootParameter(
		YAML::Node const& node, std::string_view parameterName, std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		auto shaderRegister_RegisterSpacePair = ValidateShaderRegisterAndRegisterSpace(node, parameterName);
		if (!shaderRegister_RegisterSpacePair)
			return std::nullopt;
		auto [shaderRegister, registerSpace] = *shaderRegister_RegisterSpacePair;

		YAML::Node const num32bitValNode = node["num_32_bit_val"];
		std::optional<int32_t> const num32BitVal = Serialization::AsIf<int32_t>(num32bitValNode);

		if (!(num32bitValNode && num32BitVal))
		{
			rsim_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'num_32_bit_val')",
				parameterName);
			return std::nullopt;
		}

		//acc3d_debug(
		//	"root_parameter_name={}\ntype=32bit_constants\nvisibility={}\nnum_32_bit_val={}\nshader_register={}\nregister_space={}\n",
		//	parameterName, visibilityStr, *num32BitVal, shaderRegister,
		//	registerSpace);
		CD3DX12_ROOT_PARAMETER1 param1{};
		param1.InitAsConstants(*num32BitVal, shaderRegister, registerSpace, visibility);
		return param1;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D12CBV_SRV_UAV_RootParameter(
		YAML::Node const& node, std::string_view parameterName, std::string_view typeStr,
		std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		auto shaderRegister_RegisterSpacePair = ValidateShaderRegisterAndRegisterSpace(node, parameterName);
		if (!shaderRegister_RegisterSpacePair)
			return std::nullopt;
		auto [shaderRegister, registerSpace] = *shaderRegister_RegisterSpacePair;

		YAML::Node const rootDescriptorFlagNode = node["root_descriptor_flags"];
		std::optional<std::string> const rootDescriptorFlagStr = Serialization::AsIf<std::string>(
			rootDescriptorFlagNode);

		if (!rootDescriptorFlagNode)
		{
			rsim_error(R"(Ill-formed root parameter named '{0}'. Check if you have defined 'root_descriptor_flags'.)",
			           parameterName);
			return std::nullopt;
		}

		if (!rootDescriptorFlagStr)
		{
			rsim_error("Root descriptor flag is not a string in root parameter '{0}.'", parameterName);
			return std::nullopt;
		}

		auto const rootDescriptorFlag = StringToRootDescriptorFlag(*rootDescriptorFlagStr);

		if (!(rootDescriptorFlagNode && rootDescriptorFlag))
		{
			rsim_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'root_descriptor_flag.')",
				parameterName);
			return std::nullopt;
		}
		//acc3d_debug(
		//	"root_parameter_name={}\ntype={}\nvisibility={}\nroot_descriptor_flags={}\nshader_register={}\nregister_space={}\n",
		//	parameterName, typeStr, visibilityStr, *rootDescriptorFlagStr, shaderRegister,
		//	registerSpace);
		CD3DX12_ROOT_PARAMETER1 param1{};

		// SWITCH
		param1.InitAsConstantBufferView(shaderRegister, registerSpace, *rootDescriptorFlag, visibility);
		return param1;
	}

	std::optional<CD3DX12_ROOT_PARAMETER1> RootSignatureFileDeserializer::GetD3D12DescriptorTableRootParameter(
		YAML::Node const& node, size_t index, std::string_view parameterName, std::string_view visibilityStr,
		D3D12_SHADER_VISIBILITY visibility)
	{
		YAML::Node const descriptorRangesNode = node["descriptor_ranges"];

		if (!(descriptorRangesNode && descriptorRangesNode.IsSequence()))
		{
			rsim_error(
				"Ill-formed root parameter named '{0}', Check if you have defined a 'ranges' sequence or it is a sequence.",
				parameterName);
			return std::nullopt;
		}

		size_t i = 0;

		for (auto rangeNode : descriptorRangesNode)
		{
			if (!rangeNode.IsMap())
			{
				rsim_error("Descriptor range node at index '{0}' in the root parameter '{1}' is not a map.", i,
				           parameterName);
				return std::nullopt;
			}
			YAML::Node rangeNameNode = rangeNode["range_name"];
			YAML::Node numDescriptorsNode = rangeNode["num_descriptors"];
			YAML::Node baseShaderRegisterNode = rangeNode["base_shader_register"];
			YAML::Node registerSpaceNode = rangeNode["register_space"];
			YAML::Node descriptorRangeTypeNode = rangeNode["descriptor_range_type"];
			YAML::Node descriptorRangeFlagsNode = rangeNode["descriptor_range_flags"];

			if (!(rangeNameNode && numDescriptorsNode && baseShaderRegisterNode &&
				registerSpaceNode && descriptorRangeTypeNode && descriptorRangeFlagsNode))
			{
				rsim_error(
					R"(Ill-formed descriptor range at index '{0}' in the root parameter '{1}'. Check if you have defined 'range_name', 'num_descriptors', 'base_shader_register', 'register_space','descriptor_range_type','descriptor_range_flags'.)",
					i, parameterName);
				return std::nullopt;
			}

			auto rangeName = Serialization::AsIf<std::string>(rangeNameNode);
			auto numDescriptors = Serialization::AsIf<int32_t>(numDescriptorsNode);
			auto baseShaderRegister = Serialization::AsIf<int32_t>(baseShaderRegisterNode);
			auto registerSpace = Serialization::AsIf<int32_t>(registerSpaceNode);
			auto descriptorRangeTypeStr = Serialization::AsIf<std::string>(descriptorRangeTypeNode);
			auto descriptorRangeFlagsStr = Serialization::AsIf<std::string>(descriptorRangeFlagsNode);

			if (!(rangeName && numDescriptors && baseShaderRegister &&
				registerSpace && descriptorRangeTypeStr && descriptorRangeFlagsStr))
			{
				rsim_error(
					R"(Ill-formed descriptor range at index '{0}' in the root parameter '{1}'. Check if you have correctly defined,respecting the types, 'range_name', 'num_descriptors', 'base_shader_register', 'register_space','descriptor_range_type','descriptor_range_flags'.)",
					i, parameterName);
				return std::nullopt;
			}

			auto descriptorRangeType = StringToDescriptorRangeType(*descriptorRangeTypeStr);
			auto descriptorRangeFlags = StringToDescriptorRangeFlag(*descriptorRangeFlagsStr);

			if (!(descriptorRangeType && descriptorRangeFlags))
			{
				rsim_error(
					"The values of 'descriptor_range_type' or 'descriptor_range_flags' are not correctly set in the range name '{0}'in the root parameter '{1}'.",
					*rangeName, parameterName);
				return std::nullopt;
			}

			CD3DX12_DESCRIPTOR_RANGE1 range{};
			range.Init(*descriptorRangeType, (UINT)*numDescriptors, (UINT)*baseShaderRegister, (UINT)*registerSpace,
			           *descriptorRangeFlags, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

			//acc3d_debug(
			//	"root_parameter_name={7}\nrange_name={0}\nindex={1}\nnum_descriptors={2}\nbase_shader_register={3}\nregister_space={4}\ndescriptor_range_type={5}\ndescriptor_range_flags={6}\n"
			//	, *rangeName, i, *numDescriptors, *baseShaderRegister, *registerSpace, *descriptorRangeTypeStr,
			//	*descriptorRangeFlagsStr, parameterName);


			m_DescriptorRanges[index].push_back(range);
			++i;
		}

		CD3DX12_ROOT_PARAMETER1 param1{};
		param1.InitAsDescriptorTable((UINT)descriptorRangesNode.size(), m_DescriptorRanges[index].data(), visibility);
		return param1;
	}


	std::optional<std::pair<int32_t, int32_t>> RootSignatureFileDeserializer::ValidateShaderRegisterAndRegisterSpace(
		YAML::Node const& node, std::string_view parameterName)
	{
		YAML::Node const shaderRegisterNode = node["shader_register"];
		std::optional<int32_t> const shaderRegister = Serialization::AsIf<int32_t>(shaderRegisterNode);
		YAML::Node const registerSpaceNode = node["register_space"];
		std::optional<int32_t> const registerSpace = Serialization::AsIf<int32_t>(registerSpaceNode);

		if (!(shaderRegisterNode && registerSpaceNode && shaderRegister && registerSpace))
		{
			rsim_error(
				R"(Ill-formed root parameter named '{0}'. Check if you have defined or correctly defined 'shader_register', 'register_space')",
				parameterName);
			return std::nullopt;
		}
		return {{*shaderRegister, *registerSpace}};
	}

	std::vector<RootParameter> const& RootSignatureFileDeserializer::GetRootParameters() const
	{
		return m_RootParameters;
	}

	D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFileDeserializer::GetDeserializedRootSignatureFlags() const
	{
		return m_RootSignatureFlags;
	}

	std::optional<D3D12_ROOT_SIGNATURE_FLAGS> RootSignatureFileDeserializer::GetRootSignatureFlags(
		YAML::Node const& node)
	{
		D3D12_ROOT_SIGNATURE_FLAGS rootSigFlags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		for (auto flagNode : node)
		{
			auto str = Serialization::AsIf<std::string>(flagNode);
			if (!(flagNode.IsScalar() && str))
			{
				rsim_error("Ill-formed 'root_signature_flags', check if you have defined a valid string.");
				return std::nullopt;
			}
			std::optional<D3D12_ROOT_SIGNATURE_FLAGS> const currentFlag = StringToRootSignatureFlags(*str);
			if (!currentFlag)
			{
				rsim_error("Ill-formed 'root_signature_flags', check if you have defined a valid flag option.");
				return std::nullopt;
			}

			rootSigFlags |= *currentFlag;
		}

		return rootSigFlags;
	}

	std::optional<D3D12_ROOT_PARAMETER_TYPE> RootSignatureFileDeserializer::StringToRootParameterType(
		std::string_view str)
	{
		if (str == "32bit_constants")
			return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		else if (str == "descriptor_table")
			return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		else if (str == "constant_buffer_view")
			return D3D12_ROOT_PARAMETER_TYPE_CBV;
		else if (str == "shader_resource_view")
			return D3D12_ROOT_PARAMETER_TYPE_SRV;
		else if (str == "unordered_access_view")
			return D3D12_ROOT_PARAMETER_TYPE_UAV;
		else
			return std::nullopt;
	}

	std::optional<D3D12_SHADER_VISIBILITY> RootSignatureFileDeserializer::StringToShaderVisibility(std::string_view str)
	{
		if (str == "vertex")
			return D3D12_SHADER_VISIBILITY_VERTEX;
		else if (str == "pixel")
			return D3D12_SHADER_VISIBILITY_PIXEL;
		else if (str == "hull")
			return D3D12_SHADER_VISIBILITY_HULL;
		else if (str == "mesh")
			return D3D12_SHADER_VISIBILITY_MESH;
		else if (str == "domain")
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		else if (str == "amplification")
			return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		else if (str == "geometry")
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
		else if (str == "all")
			return D3D12_SHADER_VISIBILITY_ALL;
		else
			return std::nullopt;
	}

	std::optional<D3D12_ROOT_DESCRIPTOR_FLAGS> RootSignatureFileDeserializer::StringToRootDescriptorFlag(
		std::string_view str)
	{
		if (str == "none")
			return D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		else if (str == "data_static")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;
		else if (str == "data_static_while_set_at_execute")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
		else if (str == "data_volatile")
			return D3D12_ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE;
		else
			return std::nullopt;
	}

	std::optional<D3D12_DESCRIPTOR_RANGE_FLAGS> RootSignatureFileDeserializer::StringToDescriptorRangeFlag(
		std::string_view str)
	{
		if (str == "none")
			return D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
		else if (str == "data_static")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		else if (str == "data_static_while_set_at_execute")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
		else if (str == "data_volatile")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
		else if (str == "descriptors_static_keeping_buffer_bounds_checks")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS;
		else if (str == "descriptors_volatile")
			return D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		else
			return std::nullopt;
	}

	std::optional<D3D12_DESCRIPTOR_RANGE_TYPE> RootSignatureFileDeserializer::StringToDescriptorRangeType(
		std::string_view str)
	{
		if (str == "descriptor_range_cbv")
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		else if (str == "descriptor_range_srv")
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		else if (str == "descriptor_range_uav")
			return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		else if (str == "descriptor_range_sampler")
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		else
			return std::nullopt;
	}

	std::optional<D3D12_ROOT_SIGNATURE_FLAGS> RootSignatureFileDeserializer::StringToRootSignatureFlags(
		std::string_view str)
	{
		if (str == "none")
			return D3D12_ROOT_SIGNATURE_FLAG_NONE;
		else if (str == "allow_input_assembler_input_layout")
			return D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		else if (str == "deny_vertex_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
		else if (str == "deny_hull_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		else if (str == "deny_domain_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		else if (str == "deny_geometry_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
		else if (str == "deny_pixel_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
		else if (str == "allow_stream_output")
			return D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
		else if (str == "local_root_signature")
			return D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		else if (str == "deny_amplification_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
		else if (str == "deny_mesh_shader_root_access")
			return D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

		return std::nullopt;
	}
}
