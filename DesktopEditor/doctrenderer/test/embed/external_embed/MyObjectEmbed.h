#ifndef MYOBJECTEMBED_H
#define MYOBJECTEMBED_H

// this object I want to embed in JS
// for now I just made some simple imitation classes for simplify debug process

#ifndef TEST_ON_MY_ENGINE
#include "js_internal/js_base.h"
#else
#include "MyJSBase.h"
#endif

class CMyObject
{
public:
	int getNumber()
	{
		return 42;
	}

	int getHalf(int a)
	{
		return a / 2;
	}

	int getMax(int a, int b)
	{
		return std::max(a, b);
	}

	int getMin(int a, int b)
	{
		return std::min(a, b);
	}
};

//using namespace NSJSBase;
class CMyObjectEmbed : public CEmbedObject
{
public:
	CMyObjectEmbed();
	~CMyObjectEmbed();

	void* getObject() override;


#ifndef TEST_ON_MY_ENGINE

#else
	void EmbedMethods() override;
#endif

#ifndef TEST_ON_MY_ENGINE
	JSSmart<CJSValue> getNumber();
	JSSmart<CJSValue> getHalf(JSSmart<CJSValue> a);
	JSSmart<CJSValue> getMax(JSSmart<CJSValue> a, JSSmart<CJSValue> b);
	JSSmart<CJSValue> getMin(JSSmart<CJSValue> a, JSSmart<CJSValue> b);
#else
	ValueType getNumber();
	ValueType getHalf(ValueType a);
	ValueType getMax(ValueType a, ValueType b);
	ValueType getMin(ValueType a, ValueType b);
#endif

private:
	CMyObject* m_pInternal;
};

#endif // MYOBJECTEMBED_H
