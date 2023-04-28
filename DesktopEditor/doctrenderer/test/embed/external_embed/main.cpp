#include <iostream>

#include "MyObjectEmbed.h"

#ifndef TEST_ON_MY_ENGINE
//#include "js_internal/js_base.h"
//using namespace NSJSBase;
#endif

int main(int argc, char** argv)
{
#ifndef TEST_ON_MY_ENGINE
	// Initialize and create context
	JSSmart<CJSContext> oContext = new CJSContext;
	oContext->CreateContext();
	// Enter context scope
	CJSContextScope scope(oContext);

	// TODO: Here my object is should be embedded

	// Run JS code
	JSSmart<CJSValue> oRes = oContext->runScript(
		"var oMyObject = new CreateNativeMyObject;\n"
		"var res1 = oMyObject.getNumber();\n"
		"var res2 = oMyObject.getHalf(14);\n"
		"var res3 = oMyObject.getMin(5, 10);\n"
		"var res4 = oMyObject.getMax(5, 10);\n");

	// Get results
	JSSmart<CJSObject> oGlobal = oContext->GetGlobal();
	int nRes1 = oGlobal->get("res1")->toInt32();
	int nRes2 = oGlobal->get("res2")->toInt32();
	int nRes3 = oGlobal->get("res3")->toInt32();
	int nRes4 = oGlobal->get("res4")->toInt32();
	// Print results
	std::cout << "\nRESULTS:\n";
	std::cout << "nRes1 = " << nRes1 << std::endl;
	std::cout << "nRes2 = " << nRes2 << std::endl;
	std::cout << "nRes3 = " << nRes3 << std::endl;
	std::cout << "nRes4 = " << nRes4 << std::endl;
#else
	CMyEngine engine;
	CMyObjectEmbed myObject;
	engine.Embed(&myObject, "Creator");
#endif

	return 0;
}
