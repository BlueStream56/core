#include "MyObjectEmbed.h"


#ifndef TEST_ON_MY_ENGINE
using namespace NSJSBase;
#else
// TODO: Here shouldn't be this include!!! We need to hide specific engine implementation!!!
#include "MyEngine.h"
#endif

CMyObjectEmbed::CMyObjectEmbed() : m_pInternal(new CMyObject())
{
}

CMyObjectEmbed::~CMyObjectEmbed()
{
#ifndef TEST_ON_MY_ENGINE
	RELEASEOBJECT(m_pInternal);
#else
	delete m_pInternal;
#endif
}

void* CMyObjectEmbed::getObject()
{
	return (void*)m_pInternal;
}

#ifndef TEST_ON_MY_ENGINE
JSSmart<CJSValue> CMyObjectEmbed::getNumber()
{
	return CJSContext::createInt(m_pInternal->getNumber());
}

JSSmart<CJSValue> CMyObjectEmbed::getHalf(JSSmart<CJSValue> a)
{
	int a_ = a->toInt32();
	return CJSContext::createInt(m_pInternal->getHalf(a_));
}

JSSmart<CJSValue> CMyObjectEmbed::getMax(JSSmart<CJSValue> a, JSSmart<CJSValue> b)
{
	int a_ = a->toInt32();
	int b_ = b->toInt32();
	return CJSContext::createInt(m_pInternal->getMax(a_, b_));
}

JSSmart<CJSValue> CMyObjectEmbed::getMin(JSSmart<CJSValue> a, JSSmart<CJSValue> b)
{
	int a_ = a->toInt32();
	int b_ = b->toInt32();
	return CJSContext::createInt(m_pInternal->getMin(a_, b_));
}

#else

void CMyObjectEmbed::EmbedMethods()
{
	m_embed_internal->m_sResult << "getNumber: " << CEmbedObjectPrivate::func(&CMyObjectEmbed::getNumber, this) << std::endl;
	m_embed_internal->m_sResult << "getHalf: " << CEmbedObjectPrivate::func(&CMyObjectEmbed::getHalf, this) << std::endl;
	m_embed_internal->m_sResult << "getMax: " << CEmbedObjectPrivate::func(&CMyObjectEmbed::getMax, this) << std::endl;
	m_embed_internal->m_sResult << "getMin: " << CEmbedObjectPrivate::func(&CMyObjectEmbed::getMin, this) << std::endl;
}

ValueType CMyObjectEmbed::getNumber()
{
	return (ValueType)(m_pInternal->getNumber());
}

ValueType CMyObjectEmbed::getHalf(ValueType a)
{
	int a_ = a;
	return (ValueType)(m_pInternal->getHalf(a_));
}

ValueType CMyObjectEmbed::getMax(ValueType a, ValueType b)
{
	int a_ = a;
	int b_ = b;
	return (ValueType)(m_pInternal->getMax(a_, b_));
}

ValueType CMyObjectEmbed::getMin(ValueType a, ValueType b)
{
	int a_ = a;
	int b_ = b;
	return (ValueType)(m_pInternal->getMin(a_, b_));
}

#endif
