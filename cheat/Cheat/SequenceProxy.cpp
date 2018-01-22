#include "SequenceProxy.h"
#include "../Core/Hooks.h"

inline int RandomSequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

// Map of animation fixes
// unfortunately can't be constexpr
const static std::unordered_map<std::string, int(*)(int)> animation_fix_map
{
	{ "models/weapons/v_knife_butterfly.mdl", [](int sequence) -> int
{
	switch (sequence)
	{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
	}
} },
{ "models/weapons/v_knife_falchion_advanced.mdl", [](int sequence) -> int
{
	switch (sequence)
	{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
	}
} },
{ "models/weapons/v_knife_push.mdl", [](int sequence) -> int
{
	switch (sequence)
	{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
	}
} },
{ "models/weapons/v_knife_survival_bowie.mdl", [](int sequence) -> int
{
	switch (sequence)
	{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
	}
} }
};

// Replacement function that will be called when the view model animation sequence changes.
void __cdecl Hooks::hkSequenceProxyFn(const CRecvProxyData* proxy_data_const, void* entity, void* output)
{
	static auto ofunc = SequenceHook->GetOriginalFunction();

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return ofunc(proxy_data_const, entity, output);

	// Remove the constness from the proxy data allowing us to make changes.
	auto proxy_data = const_cast<CRecvProxyData*>(proxy_data_const);

	auto pViewModel = static_cast<C_BaseViewModel*>(entity);

	if (pViewModel && pViewModel->m_hOwner() && pViewModel->m_hOwner().IsValid())
	{
		auto pOwner = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntityFromHandle(pViewModel->m_hOwner()));

		if (pOwner == g_LocalPlayer)
		{
			// Get the filename of the current view model.
			auto knife_model = g_ModelInfo->GetModel(pViewModel->m_nModelIndex());
			auto model_name = g_ModelInfo->GetModelName(knife_model);

			if (animation_fix_map.count(model_name))
			{
				proxy_data->m_Value.m_Int = animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int);
				//pViewModel->SendViewModelMatchingSequence(animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int));
			}
		}
	}

	// Call the original function with our edited data.
	ofunc(proxy_data, entity, output);
}
