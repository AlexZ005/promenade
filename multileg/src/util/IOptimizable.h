#pragma once
#include <vector>

// =======================================================================================
//                                      IOptimizable
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Interface for optimizable data class for which optimizable vars
///			can be serialized
///        
/// # IOptimizable
/// 
/// 16-7-2014 Jarl Larsson
///---------------------------------------------------------------------------------------

class IOptimizable
{
public:
	virtual std::vector<float> getParamsMax() = 0;
	virtual std::vector<float> getParamsMin() = 0;
	virtual std::vector<float> getParams() = 0;
	virtual void consumeParams(const std::vector<float>& p_other) = 0;
private:
	IOptimizable(const IOptimizable& p_other) {}
};