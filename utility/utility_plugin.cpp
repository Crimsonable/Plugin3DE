#include "utility_plugin.h"
#include <iostream>
#include <math.h>
#include <tensor.h>
#include <ftensor.h>
#include <quaternions.h>
//CAA
#include <CATIMmiPrtContainer.h>
#include <CATFrmEditor.h>
#include <CATIMmiMechanicalFeature.h>
#include <CATIPLMComponent.h>
#include <CATPLMComponentInterfacesServices.h>
#include <CATIPLMNavRepReference.h>
#include <CATIUseEntity.h>
#include <DataCommonProtocolServices.h>
#include <CATICkeRelationFactory.h>
#include <CATICkeSheet.h>
#include <CATIMmiUseGeometricalElement.h>
#include <CATIPdgUsePrtBooleanFactory.h>
#include <CATIGSMUseProceduralView.h>
#include <CATIIsolate.h>
#include <CATIMmiUseSetFactory.h>
#include <CATIMmiUsePrtPart.h>
#include <CATIUseEntity.h>
#include <CATIGSMUseFactory.h>
#include <CATIGSMUseHealing.h>
#include <CATIPdgUsePrtFactory.h>
#include <CATIPdgUsePrtBooleanOperation.h>
#include <CATIGSMUseSplit.h>
#include <CATIGSMUseRevol.h>
#include <CATGeoFactory.h>
#include <CATIGSMUseCoupling.h>
#include <CATIGSMUseLoft.h>
#include <CATIGSMUseExtrude.h>
#include <CATLine.h>
#include <CATPoint.h>
#include <CATIGSMUsePlaneEquation.h>
#include <CATIGSMUseExtractMulti.h>
#include <CATLISTV_CATIMmiUseMfBRep.h>
#include <CATIGSMUseAssemble.h>
#include <CATIGSMUseSpline.h>
#include <CATIVisProperties.h>
#include <CATModify.h>
#include <CATIRedrawEvent.h>
#include <CATIModelEvents.h>
#include <CATISktUseSketchFactory.h>
#include <CATISktUseSketchEditor.h>
#include <CATISktUse2DWFFactory.h>

void getCurrentActiveContainer(CATIMmiPrtContainer *& container)
{
	CATFrmEditor* pFrmEditor = CATFrmEditor::GetCurrentEditor();
	if (!pFrmEditor) {
		std::cout << "Failed to retrieve editor." << std::endl;
		return;
	}
	CATPathElement part = pFrmEditor->GetUIActiveObject();
	CATIMmiMechanicalFeature* partFeature = nullptr;
	part.Search(IID_CATIMmiMechanicalFeature, (void**)&partFeature);
	if (!partFeature) {
		std::cout << "Failed to get part feature." << std::endl;
		return;
	}
	CATIPLMComponent_var rep;
	CATPLMComponentInterfacesServices::GetPLMComponentOf(partFeature, rep);
	CATIPLMNavRepReference_var NavRep = rep;
	if (NavRep == NULL) {
		std::cout << "Failed to get NavRep" << std::endl;
		return;
	}
	NavRep->RetrieveApplicativeContainer("CATPrtCont", IID_CATIMmiPrtContainer, (void**)&container);
	partFeature->Release();
}

void updateCurentPart(CATIMmiPrtContainer * container)
{
	CATIMmiMechanicalFeature_var partFeature;
	container->GetMechanicalPart(partFeature);
	CATIUseEntity_var entity = partFeature;
	DataCommonProtocolServices::Update(entity);
	partFeature->Release();
}

void readXlsx(CATIMmiPrtContainer* container, const std::string& path, std::vector<std::unordered_map<std::string, double>>& params)
{
	CATICkeRelationFactory_var pFactory = container;
	if (pFactory == NULL)
		return;
	CATICkeSheet_var sheet = pFactory->CreateSheet(NULL, 1);
	sheet->SetSourceFilePath(path.c_str());
	sheet->UpdateLocalCopy();
	int rows = sheet->Rows();
	int cols = sheet->Columns();
	char* p;
	for (int i = 2; i <= rows; ++i) {
		std::unordered_map<std::string, double> buf;
		params.push_back(buf);
		for (int j = 1; j <= cols; ++j)
			params.back()[sheet->Cell(1, j).CastToCharPtr()] = strtod(sheet->Cell(i, j).CastToCharPtr(), &p);
	}
}

void mergeFeature(CATIMmiPrtContainer* container, std::vector<CATIMmiMechanicalFeature_var>& vols, CATIMmiMechanicalFeature_var& result, bool hide_flag) {
	updateCurentPart(container);
	CATIPdgUsePrtBooleanFactory_var boolFactory = container;
	CATIMmiUseGeometricalElement_var temp;
	result = vols[0];
	for (int i = 1; i < vols.size(); ++i) {
		temp = vols[i];
		result = boolFactory->CreateVolumicAdd(result, temp);
		if (i != vols.size() - 1)
			setObjectVisibilty(result, false);
		updateCurentPart(container);
	}
}

void mergeFeature(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& vol1, CATIMmiMechanicalFeature_var& vol2, CATIMmiMechanicalFeature_var& result, bool hide_flag)
{
	updateCurentPart(container);
	CATIPdgUsePrtBooleanFactory_var boolFactory = container;
	result = boolFactory->CreateVolumicAdd(vol1, vol2);
	if (hide_flag) {
		setObjectVisibilty(vol1, false);
		setObjectVisibilty(vol2, false);
	}
}

void joinSurface(CATIMmiPrtContainer* container, CATLISTV(CATIMmiMechanicalFeature_var)& surfaces, CATIMmiMechanicalFeature_var& result, bool volumn_flag) {
	updateCurentPart(container);
	CATIGSMUseFactory_var GSM_factory = container;
	CATLISTV(CATIMmiMechanicalFeature_var) dummy;
	//auto assem = GSM_factory->CreateHealing(surfaces, dummy);
	//CATIGSMUseAssemble_var assem= GSM_factory->CreateAssemble(surfaces);
	result = GSM_factory->CreateAssemble(surfaces);
}

void createMultiExtract(CATIMmiPrtContainer* container, CATLISTV(CATIMmiMechanicalFeature_var)& surfaces, CATIMmiMechanicalFeature_var& result)
{
	CATIGSMUseFactory_var GSM_factory = container;
	result = GSM_factory->CreateExtractMulti(surfaces);
}

void createClosedVol(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& closed_surf, CATIMmiMechanicalFeature_var& result) {
	updateCurentPart(container);
	CATIPdgUsePrtFactory_var prt_factory = container;
	result = prt_factory->CreateVolumicCloseSurface(closed_surf);
}

void createSplit(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& feat1, CATIMmiMechanicalFeature_var& feat2, CATIMmiMechanicalFeature_var& result, bool invert) {
	CATIGSMUseFactory_var GSM_factory = container;
	CATIGSMUseSplit_var _result;
	if (invert)
		_result = GSM_factory->CreateSplit(feat1, feat2, CATGSMOrientation::CATGSMInvertOrientation);
	else
		_result = GSM_factory->CreateSplit(feat1, feat2, CATGSMSameOrientation);
	_result->SetVolumeResult(true);
	result = _result;
}

void createRevol(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& section, const CATIMmiMechanicalFeature_var& axis, double angle1, double angle2, CATIMmiMechanicalFeature_var& result) {
	if (!container)
		return;
	CATIGSMUseFactory_var GSM_factory = container;
	if (!GSM_factory)
		return;

	CATICkeParmFactory_var param_factory = container;
	if (param_factory == NULL)
		return;
	CATICkeParm_var angle_1 = param_factory->CreateAngle("Rotate", angle1);
	CATICkeParm_var angle_2 = param_factory->CreateAngle("Rotate", angle2);

	auto revol = GSM_factory->CreateRevol(section, axis, angle_1, angle_2, true);
	revol->SetContext(CATGSMVolumeCtxt);
	result = revol;
}

void createLine(CATIMmiPrtContainer* container, double* origin, double* dir, CATIMmiMechanicalFeature_var& result) {
	if (!container)
		return;
	CATIGSMUseFactory_var GSM_factory = container;
	if (!GSM_factory)
		return;
	CATICkeParmFactory_var param_factory = container;

	result = GSM_factory->CreateLine(origin, dir);
}

void insertFeature(std::vector<CATIMmiMechanicalFeature_var>& result, CATIMmiMechanicalFeature_var dest) {
	CATIGSMUseProceduralView_var insert;
	for (int i = 0; i < result.size(); ++i) {
		insert = result[i];
		insert->InsertInProceduralView(dest);
	}
}

void convertBRepAcceToFeature(std::vector<CATBaseUnknown*>& source, std::vector<CATIMmiUseBRep*>& breps)
{
	for (auto& item : source)
	{
		breps.push_back(nullptr);
		auto rc = item->QueryInterface(IID_CATIMmiUseBRep, (void**)&(breps.back()));
	}
}

void createLoftBasic(CATIMmiPrtContainer* container, const char* keys[], int size, const std::unordered_map<std::string, bool>& dict, const std::pair<int, int>& range, const CATLISTV(CATIMmiMechanicalFeature_var)& sections, const std::vector<std::unordered_map<std::string, CATIMmiMechanicalFeature_var>>& points, const CATLISTV(CATIMmiMechanicalFeature_var)& extremum, const CATLISTV(CATIMmiMechanicalFeature_var)& guides, CATIGSMUseLoft_var& result, CATIMmiMechanicalFeature_var& support_start, CATIMmiMechanicalFeature_var& support_end)
{
	if (!container)
		return;
	CATIGSMUseFactory_var GSM_factory = container;

	CATLISTV(CATIMmiMechanicalFeature_var) _sections;
	for (int i = range.first; i <= range.second; ++i)
		_sections.Append(sections[i + 1]);

	CATListOfInt ori;
	CATIGSMUseLoft_var loft = GSM_factory->CreateLoft(_sections, ori, extremum, support_start, support_end, guides);

	if (dict.size()) {
		CATIGSMUseCoupling_var coupling = loft;
		int count = 0;
		for (int j = 0; j < size; ++j) {
			if (dict.at(keys[j])) {
				coupling->InsertCoupling(0);
				count++;
				for (int i = range.first; i <= range.second; ++i) {
					coupling->InsertCouplingPoint(count, 0, points[i].at(keys[j]));
				}
			}
		}
	}
	result = loft;
}

void createExtrude(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var& surface, double length, CATBaseUnknown* axis, bool reverseDir, CATIMmiMechanicalFeature_var& result)
{
	CATIGSMUseFactory_var GSM_factory = container;
	CATICkeParmFactory_var param_factory = container;

	CATIGSMUseDirection_var dir = GSM_factory->CreateDirection(axis);
	CATICkeParm_var L = param_factory->CreateLength("L", length);
	CATICkeParm_var L2 = param_factory->CreateLength("L", 0);

	auto extrude = GSM_factory->CreateExtrude(surface, dir, L, L2, reverseDir);
	extrude->SetContext(CATGSMVolumeCtxt);
	result = extrude;
}

void createPlane(CATIMmiPrtContainer* container, double offset, CATBaseUnknown* iaxis, CATBaseUnknown* iorigin, CATIMmiMechanicalFeature_var& result)
{
	CATIGSMUseFactory_var GSM_factory = container;
	CATICkeParmFactory_var param_factory = container;

	Expblas::FTensor<double, 3> _dir, _origin;
	CATLine_var axis = iaxis;
	CATPoint_var origin = iorigin;

	axis->GetDirection().GetCoord(_dir.data);
	origin->GetCoord(_origin[0], _origin[1], _origin[2]);
	_origin = _origin + offset*_dir;

	CATICkeParm_var A = param_factory->CreateReal("A", _dir[0]);
	CATICkeParm_var B = param_factory->CreateReal("B", _dir[1]);
	CATICkeParm_var C = param_factory->CreateReal("C", _dir[2]);
	CATICkeParm_var D = param_factory->CreateLength("D", Expblas::dot(_origin, _dir));

	result = GSM_factory->CreatePlane(A, B, C, D);
}

//void extractFace(CATIMmiMechanicalFeature_var& feature) {
//	CATIMmiProcReport_var report = feature;
//	if (report == NULL_var)
//		return;
//	CATGeoFactory_var GEO_factory;
//	auto rc = report->GetGeoFactoryFromFeature(GEO_factory);
//	CATCGMJournalList* journal = nullptr;
//	rc = report->GetCGMJournalList(journal);
//	if (!journal) return;
//	//CATCGMCreateTopExtractFace(GEO_factory,journal,);
//}

void insertFeature(CATIMmiMechanicalFeature_var& result, CATIMmiMechanicalFeature_var dest) {
	CATIGSMUseProceduralView* _insert = nullptr;
	auto rc = result->QueryInterface(IID_CATIGSMUseProceduralView, (void**)&_insert);
	CATIGSMUseProceduralView_var insert = result;
	if (insert != NULL_var)
		insert->InsertInProceduralView(dest);
}

void isolate(CATIMmiMechanicalFeature_var& feature) {
	CATIIsolate_var isolate = feature;
	auto rc = isolate->Isolate();
}

void deleteFeature(CATIMmiMechanicalFeature_var& feature) {
	CATIUseEntity_var entity = feature;
	DataCommonProtocolServices::Delete(entity);
}

void addSurfFeatureSet(CATIMmiPrtContainer* container, const std::string& name, CATIMmiMechanicalFeature_var& set) {
	CATIMmiMechanicalFeature_var partFeature;
	container->GetMechanicalPart(partFeature);
	CATIMmiUseSetFactory_var setFactory = container;
	setFactory->CreateGeometricalSet(name.c_str(), partFeature, set);
}

void setCurrentPath(CATIMmiPrtContainer* container, CATIMmiMechanicalFeature_var destination) {
	CATIMmiMechanicalFeature_var partFeature;
	container->GetMechanicalPart(partFeature);
	CATIMmiUsePrtPart_var usePrt = partFeature;
	usePrt->SetInWorkObject(destination);
}

void setObjectVisibilty(CATBaseUnknown* object, bool showflag) {
	CATIVisProperties_var visProp = object;
	if (visProp == NULL_var)
		return;
	CATVisPropertiesValues prop;
	CATVisPropertyType PropType = CATVPShow;
	CATVisGeomType geoType = CATVPGlobalType;
	prop.SetShowAttr((showflag ? CATShowAttr : CATNoShowAttr));
	visProp->SetPropertiesAtt(prop, PropType, geoType);

	CATIRedrawEvent_var redraw_event = object;
	if (redraw_event == NULL_var)
		return;
	redraw_event->Redraw();

	CATModify parent_nmodify(object);
	CATIModelEvents_var modify_event = object;
	if (modify_event == NULL_var)
		return;
	modify_event->Dispatch(parent_nmodify);
}

void createCircle(CATIMmiPrtContainer * container, double * origin, double radius, CATIMmiMechanicalFeature_var & result)
{
	if (!container)
		return;
	CATISktUseSketchFactory_var sketch = container;
	CATIMmiMechanicalFeature_var sketchFeature;
	CATISktUseSketchEditor_var sketchEditor;

	Expblas::FTensor<double, 3> x{ 1,0,0 }, y{ 0,1,0 };
	sketchFeature = sketch->CreateSketch(origin, x.data, y.data);
	sketchEditor = sketchFeature;
	if (!SUCCEEDED(sketchEditor->OpenEdition())) {
		std::cout << "Failed to open sketch edition." << std::endl;
		return;
	}
	CATISktUse2DWFFactory_var sketchFactory = sketchEditor;
	sketchFactory->CreateCircle(origin, radius);
	sketchEditor->CloseEdition();
}