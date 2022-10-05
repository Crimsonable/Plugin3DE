#pragma once
#include <unordered_map>
#include <vector>
#include <CATLISTV_CATIMmiMechanicalFeature.h>
#include <assert.h>

class CATIMmiUseBRep;
class CATIGSMUseLoft_var;
class CATIMmiPrtContainer;

__declspec(dllexport) void getCurrentActiveContainer(CATIMmiPrtContainer*& container);

__declspec(dllexport) void updateCurentPart(CATIMmiPrtContainer* container);

__declspec(dllexport) void readXlsx(CATIMmiPrtContainer* container, const std::string& path, std::vector<std::unordered_map<std::string, double>>& params);

__declspec(dllexport) void mergeFeature(CATIMmiPrtContainer* container, std::vector<CATIMmiMechanicalFeature_var>& vols, CATIMmiMechanicalFeature_var& result, bool hide_flag = false);

__declspec(dllexport) void mergeFeature(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& vol1, CATIMmiMechanicalFeature_var& vol2, CATIMmiMechanicalFeature_var& result, bool hide_flag = false);

__declspec(dllexport) void joinSurface(CATIMmiPrtContainer* container, CATLISTV(CATIMmiMechanicalFeature_var)& surfaces, CATIMmiMechanicalFeature_var& result, bool volumn_flag = false);

__declspec(dllexport) void createClosedVol(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& closed_surf, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void createSplit(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& feat1, CATIMmiMechanicalFeature_var& feat2, CATIMmiMechanicalFeature_var& result, bool invert = false);

__declspec(dllexport) void createMultiExtract(CATIMmiPrtContainer* container, CATLISTV(CATIMmiMechanicalFeature_var)& surfaces, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void createRevol(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& section, const CATIMmiMechanicalFeature_var& axis, double angle1, double angle2, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void createLoftBasic(CATIMmiPrtContainer* container, const char* keys[], int size, const std::unordered_map<std::string, bool>& dict, const std::pair<int, int>& range, const CATLISTV(CATIMmiMechanicalFeature_var)& sections, const std::vector<std::unordered_map<std::string, CATIMmiMechanicalFeature_var>>& points, const CATLISTV(CATIMmiMechanicalFeature_var)& extremum, const CATLISTV(CATIMmiMechanicalFeature_var)& guides, CATIGSMUseLoft_var& result, CATIMmiMechanicalFeature_var& support_start, CATIMmiMechanicalFeature_var& support_end);

__declspec(dllexport) void createExtrude(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& surface, double length, CATBaseUnknown* axis, bool reverseDir, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void createPlane(CATIMmiPrtContainer* container, double offset, CATBaseUnknown* iaxis, CATBaseUnknown* iorigin, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void createLine(CATIMmiPrtContainer* container, double* origin, double* dir, CATIMmiMechanicalFeature_var& result);

__declspec(dllexport) void convertBRepAcceToFeature(std::vector<CATBaseUnknown*>& source, std::vector<CATIMmiUseBRep*>& breps);

__declspec(dllexport) void insertFeature(std::vector<CATIMmiMechanicalFeature_var>& result, CATIMmiMechanicalFeature_var dest = NULL_var);

__declspec(dllexport) void insertFeature(CATIMmiMechanicalFeature_var& result, CATIMmiMechanicalFeature_var dest = NULL_var);

__declspec(dllexport) void isolate(CATIMmiMechanicalFeature_var& feature);

__declspec(dllexport) void deleteFeature(CATIMmiMechanicalFeature_var& feature);

__declspec(dllexport) void addSurfFeatureSet(CATIMmiPrtContainer* container, const std::string& name, CATIMmiMechanicalFeature_var& set);

__declspec(dllexport) void setCurrentPath(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var destination);

__declspec(dllexport) void setObjectVisibilty(CATBaseUnknown* object, bool showflag);

__declspec(dllexport) void createCircle(CATIMmiPrtContainer* container, double* origin, double radius, CATIMmiMechanicalFeature_var& result);