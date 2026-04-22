#include "OWLHelper.h"

void OWLHelper::InitOWL()
{
	OWLContext context = owlContextCreate(nullptr, 1);
	OWLModule module = owlModuleCreate(context, "pt.cu");
}